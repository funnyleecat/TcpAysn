#include <stdafx.h>
#include "ConnCtx.h"
#include "ConnMgr.h"
#include "Utility.h"
#include "IMessageHandler.h"
#include "Workthreadpool.h"
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>

namespace tcpcomm2
{
    CConnCtx::CConnCtx(IService& Srv, _IOService& Ios, size_t nId)
        : IComponent(Srv)
        , m_Socket(Ios)
        , m_bIsClosing(false)
        , m_nCtxId(nId)
        , m_bWriteCompleted(true)
        , m_RecvBuff(SOCK_RCVBUF_MIN, Srv.m_nSockRcvBuffMax)
    {

    }

    CConnCtx::~CConnCtx()
    {

    }

    size_t CConnCtx::GetCtxID()
    {
        return m_nCtxId;
    }

    _TcpSocket& CConnCtx::GetSocket()
    {
        return m_Socket;
    }

    void CConnCtx::Close()
    {
        //io�̼߳�⵽����ʱ�����̼߳�⵽��ʱ��������ô˺��������ctx�Ѿ��رջ������ڹرգ�����Ҫ�ظ�����
        bool bExpValue = false;
        if (!m_bIsClosing.compare_exchange_strong(bExpValue, true)) return;

        if (m_Socket.is_open())
        {
            //���Ƚ����������Ķ���Ӷ����������ɾ��
            //���������̲߳����ڹر��ڼ��ȡ���˶���
            m_Srv.m_pConnMgr->DelCtx(m_nCtxId);

            try
            {
                OUTPUT_INFO_LOG("Ctx[%lu] closed,last rcv-time:%s", m_nCtxId,
                    boost::posix_time::to_simple_string(m_LastRcvTime).c_str());
                m_Socket.shutdown(_TcpSocket::shutdown_both);
                m_Socket.close();
            }
            catch (std::exception&)
            {
            }

            //��RequestHandle�������ӶϿ�֪ͨ
            /*m_Srv.m_pRuntimeHandler->HandleCommDisconnect(m_nCtxId);*/

            //��WORK�߳�֪ͨ�����ӶϿ�֤��Ϣʱ��
            SendWorkData(WORKEVT_DISCONNECTED);
        }
    }

    bool CConnCtx::Connect(const char* pszAddress, unsigned short nPort)
    {
        try
        {
            using boost::asio::ip::address;
            _TcpEndpoint ep(address::from_string(pszAddress), nPort);
            m_Socket.connect(ep);
            m_ConnInfo = Vsprintf_t("%s:%d", pszAddress, nPort);
            AsynRead();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    void CConnCtx::OnAsynConnectDone(const Syserr_t& e)
    {
        if (!e)
        {
            //��¼SOCK������Ϣ
            Syserr_t ec;
            _TcpEndpoint ep = m_Socket.remote_endpoint(ec);
            std::string strIp = ep.address().to_string();
            m_ConnInfo = Vsprintf_t("%s:%d", strIp.c_str(), ep.port());
            OUTPUT_CONNCTX_LOG("CONNECTED", LOGLV_INFO);

            //����ʱ����Ƭʱ��
            m_Srv.OnUpdateConn(shared_from_this());

            //��RequestHandle��������֪ͨ
/*            m_Srv.m_pRuntimeHandler->HandleCommConnected(m_nCtxId);*/
            SendWorkData(WORKEVT_CONNECTED);
            return AsynRead();
        }
        else
        {
            string strMessage("ASYN CONNECT ERROR :");
            strMessage += boost::lexical_cast<string>(e.value());
			strMessage += ":" + e.message();
            OUTPUT_CONNCTX_LOG(strMessage.c_str(), LOGLV_ERROR);
        }
    }

    bool CConnCtx::Write(const TcpMessage* pMsg)
    {
        _Autolock lock(m_WriteMutex);
        //������������д�뵽�����������첽�������֮��֪ͨ�û�
        if (!m_SendBuff.Write(pMsg))
            return false;

        //����Ҫȷ��ͬһʱ��ֻ��һ���첽д��Ϣ��IOService
        //m_bWriteCompleted ˵����һ���첽д��Ϣ��û�����
        //��ǰд����Ϣ����Ҫ��֮ǰ������Ϣ�������֮��Żᱻ����
        if (m_bWriteCompleted)
            AsynWrite();

        return true;
    }

    void CConnCtx::AsynWrite()
    {
        m_bWriteCompleted = false;

        //��ȡ���ͻ�������δ��������
        TcpSendBuff tmpBuff = m_SendBuff.GetCurrentBuff();
        if ((tmpBuff.first == 0) || (tmpBuff.second == 0))
            return;

        //Ͷ���첽������Ϣ���ں�
        boost::asio::async_write(m_Socket,
            boost::asio::buffer(tmpBuff.first, tmpBuff.second),
            boost::bind(&CConnCtx::OnAsynWriteDone, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void CConnCtx::OnAsynWriteDone(const Syserr_t& e, size_t nByteTrans)
    {
        if (!e)
        {
            _Autolock lock(m_WriteMutex);
            if (m_SendBuff.OnSendCompleted(nByteTrans))
                AsynWrite();
            else
                m_bWriteCompleted = true;
        }
        else
        {
            string strMessage("ASYN WRITE ERROR :");
            strMessage += boost::lexical_cast<string>(e.value());
			strMessage += ":" + e.message();
            OUTPUT_CONNCTX_LOG(strMessage.c_str(), LOGLV_ERROR);
        }
    }

    void CConnCtx::AsynRead()
    {
        m_Socket.async_read_some(boost::asio::buffer(m_pFastRecvBuff, sizeof(m_pFastRecvBuff)),
            boost::bind(&CConnCtx::OnAsynReadDone, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void CConnCtx::OnAsynReadDone(const Syserr_t& e, size_t nByteTrans)
    {
        if (!e)
        {
            //���ӻ�Ծ
            m_Srv.OnUpdateConn(shared_from_this());

            //�����ջ�������û������ʱ��
            if (m_RecvBuff.GetReadableLen() == 0)
            {
                //ֱ�Ӵ�����ٻ��������ݣ������ڴ濽��
                if (ParseFastRecvBuff(nByteTrans) != (size_t)-1)
                    return AsynRead();
            }
            else
            {
                //���ջ����������ϴν��յ���δ���������
                //�����ν��յ�������׷�ӵ��ϴν��յ����ݵ�β��(ƴ��)
                if (ParseRecvBuff(nByteTrans) != (size_t)-1)
                    return AsynRead();
            }
        }
        else
        {
            string strMessage("ASYN READ ERROR :");
            strMessage += boost::lexical_cast<string>(e.value());
			strMessage += ":"+e.message();
            OUTPUT_CONNCTX_LOG(strMessage.c_str(), LOGLV_ERROR);
        }
    }

    size_t CConnCtx::ParseRecvBuff(size_t nByteTrans)
    {
        size_t nByteWrite = 0;
        while (nByteWrite < nByteTrans)
        {
            size_t nWriteLen = m_RecvBuff.Write( \
                m_pFastRecvBuff + nByteWrite, nByteTrans - nByteWrite);

            if (0 == nWriteLen)
            {
                OUTPUT_CONNCTX_LOG("WRITE RECV BUFFER ERROR", LOGLV_ERROR);
                return -1;
            }

            nByteWrite += nWriteLen;

            size_t nByteParse = ParseRecvData(\
                m_RecvBuff.GetData(), m_RecvBuff.GetReadableLen());

			if (nByteParse > 0 && nByteParse != (size_t)-1)
            {
                SendWorkData(WORKEVT_RECVDATA, m_RecvBuff.GetData(), nByteParse);
                m_RecvBuff.Erase(nByteParse);
                continue;
            }
            else /*if (nByteParse == (size_t)-1 || nByteParse == 0) */
            {
                return nByteParse;
            }
        }

        return nByteWrite;
    }

    size_t CConnCtx::ParseFastRecvBuff(size_t nByteTrans)
    {
        size_t nByteParse = ParseRecvData(m_pFastRecvBuff, nByteTrans);

        if (nByteParse == (size_t)-1)
            return -1;
        else if (nByteParse == 0)
        {
            if (0 == m_RecvBuff.Write(m_pFastRecvBuff, nByteTrans))                
            {
                OUTPUT_CONNCTX_LOG("WRITE RECV BUFFER ERROR", LOGLV_ERROR);
                return -1;
            }

            return 0;
        }
        else //if (nByteParse > 0)
        {
            //����ʣ�������(���)û�д���
            //��ʣ������(���)д�뵽���ջ������������������
            if (nByteParse < nByteTrans)
            {
                if (0 == m_RecvBuff.Write(\
                    m_pFastRecvBuff + nByteParse, nByteTrans - nByteParse))
                {
                    OUTPUT_CONNCTX_LOG("WRITE RECV BUFFER ERROR", LOGLV_ERROR);
                    return -1;
                }
            }

            SendWorkData(WORKEVT_RECVDATA, m_pFastRecvBuff, nByteParse);
            return nByteParse;
        }      
    }

    size_t CConnCtx::ParseRecvData(const char* pData, size_t nByteTrans)
    {
        size_t nByteParse = 0;

        while (nByteParse < nByteTrans)
        {
            size_t nByteRead = m_Srv.m_pRuntimeHandler->HandleCommRecvBuffer(
                m_nCtxId, pData, nByteTrans - nByteParse);

            if (nByteRead == (size_t)-1)
            {
                OUTPUT_CONNCTX_LOG("PARSE ERROR PACKAGE, CLOSE CONNECTION", LOGLV_ERROR);
                return -1;
            }
            else if(nByteRead > m_Srv.m_nSockRcvBuffMax)
            {
                OUTPUT_CONNCTX_LOG("OUT OF SOCK_RCVBUF_MAX", LOGLV_ERROR);
                return -1;
            }           
            else if (nByteRead > 0)
            {
                nByteParse += nByteRead;
                pData += nByteRead;
                continue;
            }
            else if (nByteRead == 0)
            {
                break;
            }
        }

        return nByteParse;
    }

    void CConnCtx::SendWorkData(WORKEVENT wEvent, const char* pData, size_t nByte)
    {
        //56��73��Ϊ�˹��˵�inbound��outbound�����������д������ѧ��Ϊ�˵�bug����������
        if (WORKEVT_RECVDATA == wEvent && nByte != 56 && nByte != 73)
        {
            m_LastRcvTime = boost::posix_time::microsec_clock::local_time();
        }
        TcpWorkData* pWorkData = TcpWorkDataCreate(m_nCtxId, pData, nByte, wEvent);
        if (!pWorkData)
        {
            OUTPUT_CONNCTX_LOG("ALLOCETE WORKDATA OUT OF MEMORY", LOGLV_ERROR);
            return;
        }
                      
        m_Srv.m_pWorkthreadpool->Push(pWorkData);              
    }

    void CConnCtx::OUTPUT_CONNCTX_LOG(const char* pszMessage, LOG_LEVEL lv)
    {
        static const char* format = "CTX[%d] %s %s";

        switch (lv)
        {
        case LOGLV_DEBUG:
            OUTPUT_DEBUG_LOG(format, m_nCtxId, m_ConnInfo.c_str(), pszMessage);
            break;
        case LOGLV_INFO:
            OUTPUT_INFO_LOG(format, m_nCtxId, m_ConnInfo.c_str(), pszMessage);
            break;
        case LOGLV_WARN:
            OUTPUT_WARN_LOG(format, m_nCtxId, m_ConnInfo.c_str(), pszMessage);
            break;
        case LOGLV_ERROR:
            OUTPUT_ERROR_LOG(format, m_nCtxId, m_ConnInfo.c_str(), pszMessage);
            Close();
            break;
        case LOGLV_FATAL:
            OUTPUT_FATAL_LOG(format, m_nCtxId, m_ConnInfo.c_str(), pszMessage);
            Close();
            break;
        }
    }
}
