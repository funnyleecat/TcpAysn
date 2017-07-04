#include "stdafx.h"
#include "TcpServer.h"
#include "Acceptor.h"
#include "TimeWheel.h"

namespace tcpcomm2
{
    ITcpServer* ITcpServer::GetInstance(int nIothread, int nWorkthread, int nKeepalive)
    {
        return new CTcpServer(nIothread, nWorkthread, nKeepalive);
    }

    void ITcpServer::Release(ITcpServer* pInstance)
    {
        if (pInstance) delete pInstance;
    }

    CTcpServer::CTcpServer(int nIothread, int nWorkthread, int nKeepalive)
        : IService(nIothread, nWorkthread, nKeepalive)
        , m_nAcceptorMode(0) //默认为异步接收模式
    {

    }

    CTcpServer::~CTcpServer()
    {
        if (m_pAcceptor)
        {
            m_pAcceptor.reset();
        }

        if (m_pTimeWheel)
        {
            m_pTimeWheel->Stop();
            m_pTimeWheel.reset();
        }
    }

    void CTcpServer::SetSrvOption(ESrvOpt eOpt, size_t nOptData)
    {
        switch (eOpt)
        {
        case OPT_ACCEPTOR_MODE:
            m_nAcceptorMode = (nOptData == 0) ? 0 : 1;
            break;
        default:
            IService::_SetSrvOption(eOpt, nOptData);
            break;
        }
    }

    bool CTcpServer::SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg)
    {
        return IService::_SendTcpMessage(nCtxId, pMsg);
    }

    void CTcpServer::Close(size_t nCtxId)
    {
        return IService::_Close(nCtxId);
    }

    bool CTcpServer::StartService(unsigned short nPort)
    {
        bool bReturn = false;

        try
        {
            do
            {
                if (!IService::_StartService())
                    break;

                m_pTimeWheel = CTimeWheelPtr(new CTimeWheel(*this));
                if (!m_pTimeWheel || !m_pTimeWheel->Run())
                    break;

                m_pAcceptor = CAcceptorPtr(new CAcceptor(*this));
                if (0 == m_nAcceptorMode)
                {
                    //启动接收器异步模式
                    if (!m_pAcceptor || !m_pAcceptor->AsynRun(nPort))
                        break;
                }
                else
                {
                    //启动接收器同步步模式， 此处将会被阻塞
                    if (!m_pAcceptor || !m_pAcceptor->Run(nPort))
                        break;
                }

                bReturn = true;
            } while (0);
        }
        catch (std::exception&)
        {

        }

        if (!bReturn) StopService();
        return bReturn;
    }

    void CTcpServer::StopService()
    {
        if (m_pAcceptor)
        {
            m_pAcceptor.reset();
        }

        if (m_pTimeWheel)
        {
            m_pTimeWheel->Stop();
            m_pTimeWheel.reset();
        }

        return IService::_StopService();
    }

    void CTcpServer::OnUpdateConn(CConnCtxPtr pCtx)
    {
        return m_pTimeWheel->OnUpdateConn(pCtx);
    }
}


