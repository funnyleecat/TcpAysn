#include "stdafx.h"
#include "Acceptor.h"
#include "ConnMgr.h"
#include "ConnCtx.h"

namespace tcpcomm2
{
    CAcceptor::CAcceptor(IService& Srv)
        : IComponent(Srv)
    {

    }

    CAcceptor::~CAcceptor()
    {
        Stop();
    }

    void CAcceptor::AsynAccept()
    {
        CConnCtxPtr pCtx = m_Srv.m_pConnMgr->NewCtx();
        assert(pCtx);

        m_pTcpAcceptor->async_accept(pCtx->GetSocket(),
            boost::bind(&CAcceptor::OnAsynAcceptDone, shared_from_this(),
            boost::asio::placeholders::error, pCtx)
            );
    }

    void CAcceptor::OnAsynAcceptDone(const Syserr_t& e, ConnCtxPtr pCtx)
    {
        pCtx->OnAsynConnectDone(e);
        AsynAccept();
    }

    bool CAcceptor::Run(unsigned short nPort)
    {
        try
        {
            m_pTcpSrv = _IOServicePtr(new _IOService);
            m_pTcpWork = _IOServiceWorkPtr(new _IOServiceWork(*m_pTcpSrv));
            m_pTcpAcceptor = _TcpAcceptorPtr(new _TcpAcceptor(*m_pTcpSrv));
           
            _TcpEndpoint ep(boost::asio::ip::tcp::v4(), nPort);
            m_pTcpAcceptor->open(ep.protocol());
            m_pTcpAcceptor->set_option(_TcpAcceptor::reuse_address(true));
            m_pTcpAcceptor->bind(ep);
            m_pTcpAcceptor->listen();
            AsynAccept();

            m_pTcpSrv->run();
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

    bool CAcceptor::AsynRun(unsigned short nPort)
    {
        try
        {
            m_pTcpSrv = _IOServicePtr(new _IOService);
            m_pTcpWork = _IOServiceWorkPtr(new _IOServiceWork(*m_pTcpSrv));
            m_pTcpAcceptor = _TcpAcceptorPtr(new _TcpAcceptor(*m_pTcpSrv));

            _TcpEndpoint ep(boost::asio::ip::tcp::v4(), nPort);
            m_pTcpAcceptor->open(ep.protocol());
            m_pTcpAcceptor->set_option(_TcpAcceptor::reuse_address(true));
            m_pTcpAcceptor->bind(ep);
            m_pTcpAcceptor->listen();
            AsynAccept();

            m_pThread = _ThreadPtr(new _Thread(
                boost::bind(&_IOService::run, m_pTcpSrv)));
        }
        catch (const std::exception& e)
        {
            printf("listen exception,port:%d,%s\n", nPort, e.what());
            return false;
        }

        return true;
    }

    void CAcceptor::Stop()
    {
        if (m_pTcpSrv)
            m_pTcpSrv->stop();

        if (m_pThread)
            m_pThread->join();
    }
}
