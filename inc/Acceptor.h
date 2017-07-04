#pragma once

#include "types.h"
#include "IComponent.h"
#include "IOthreadpool.h"

namespace tcpcomm2
{
    class CConnCtx;
    typedef boost::shared_ptr<CConnCtx> ConnCtxPtr;

    class CAcceptor
        : public IComponent
        , public boost::enable_shared_from_this<CAcceptor>
    {
    public:
        CAcceptor(IService& Srv);
        virtual ~CAcceptor();

        bool Run(unsigned short nPort);
        bool AsynRun(unsigned short nPort);
        void Stop();

    private:
        void AsynAccept();
        void OnAsynAcceptDone(const Syserr_t& e, ConnCtxPtr pCtx);

        _ThreadPtr m_pThread;
        _IOServicePtr m_pTcpSrv;
        _IOServiceWorkPtr m_pTcpWork;
        _TcpAcceptorPtr m_pTcpAcceptor;
    };
}

