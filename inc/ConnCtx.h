#pragma once

#include <string>
#include "types.h"
#include "IComponent.h"
#include "SockSendBuffer.h"
#include "SockRecvBuffer.h"
using namespace std;

namespace tcpcomm2
{
    class CTimeEntry;
    typedef boost::weak_ptr<CTimeEntry> TimeEntryWeakPtr;

    class CConnCtx
        : public IComponent
        , public boost::enable_shared_from_this<CConnCtx>
    {
    public:
        CConnCtx(IService& Srv, _IOService& Ios, size_t nId);
        virtual ~CConnCtx();

        size_t GetCtxID();
        _TcpSocket& GetSocket();

        void Close();
        bool Connect(const char* pszAddress, unsigned short nPort);
        bool Write(const TcpMessage* pMsg);

        void OnAsynConnectDone(const Syserr_t& e);
        void OnAsynReadDone(const Syserr_t& e, size_t nByteTrans);
        void OnAsynWriteDone(const Syserr_t& e, size_t nByteTrans);
        void OUTPUT_CONNCTX_LOG(const char* pszMessage, LOG_LEVEL lv);

        TimeEntryWeakPtr m_pWeakEntry;
    private:
        void AsynRead();
        void AsynWrite();
        void SendWorkData(WORKEVENT wEvent, const char* pData = 0, size_t nByte = 0);
        //将接收到数据写入到接收缓冲区，然后再处理数据
        size_t ParseRecvBuff(size_t nByteTrans);
        size_t ParseFastRecvBuff(size_t nByteTrans);
        size_t ParseRecvData(const char* pData, size_t nByteTrans);

    private:
        boost::atomic<bool> m_bIsClosing;
        size_t m_nCtxId;
        string m_ConnInfo;  //ip:port       
        _TcpSocket m_Socket;
        boost::posix_time::ptime m_LastRcvTime;
        
        _Mutex m_WriteMutex;
        bool m_bWriteCompleted;
        CSockSendBuffer m_SendBuff;
        
        CSockRecvBuffer m_RecvBuff;
        char m_pFastRecvBuff[SOCK_RCVBUF_MIN];        
    };
}
