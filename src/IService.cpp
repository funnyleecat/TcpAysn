#include "stdafx.h"
#include <assert.h>
#include "IService.h"
#include "ConnCtx.h"
#include "ConnMgr.h"
#include "IOthreadpool.h"
#include "Workthreadpool.h"

namespace tcpcomm2
{
    IService::IService(int nIothread, int nWorkthread, int nKeepalive)
        : m_nIOthread(nIothread)
        , m_nWorkthread(nWorkthread)
        , m_nKeepalive(nKeepalive)
        , m_nSockRcvBuffMax(SOCK_RCVBUF_MAX)
        , m_nSockSndBuffMax(SOCK_RCVBUF_MAX)
        , m_pRuntimeLogger(0)
        , m_pRuntimeHandler(0)
    {

    }

    IService::~IService()
    {
        _StopService();
    }

    void IService::_StopService()
    {
        if (m_pConnMgr)
        { 
            m_pConnMgr->DelAllCtx(); 
            m_pConnMgr.reset(); 
        }

        if (m_pIOthreadpool)
        {
            m_pIOthreadpool->Stop();
            m_pIOthreadpool.reset();
        }

        if (m_pWorkthreadpool)
        {
            m_pWorkthreadpool->Stop();
            m_pWorkthreadpool.reset();
        }
    }

    bool IService::_StartService()
    {
        bool bReturn = false;

        try
        {
            do 
            {
                if (!m_pRuntimeHandler || !m_pRuntimeLogger ||
                    (m_nWorkthread <= 0) || (m_nIOthread <= 0) || (m_nKeepalive <= 0))
                    break;

                m_pConnMgr = CConnMgrPtr(new CConnMgr(*this));
                if (!m_pConnMgr) 
                    break;

                m_pIOthreadpool = CIOthreadpoolPtr(new CIOthreadpool(*this));
                if (!m_pIOthreadpool || !m_pIOthreadpool->Run(m_nIOthread)) 
                    break;

                m_pWorkthreadpool = CWorkthreadpoolPtr(new CWorkthreadpool(*this));
                if (!m_pWorkthreadpool || !m_pWorkthreadpool->Run(m_nWorkthread)) 
                    break;

                bReturn = true;
            } while (0);
        }
        catch (std::exception&)
        {
        	
        }

        if (!bReturn) _StopService();
        return bReturn;       
    }

    void IService::_Close(size_t nCtxId)
    {
        CConnCtxPtr pCtx = m_pConnMgr->GetCtx(nCtxId);
        if (pCtx) pCtx->Close();
    }

    bool IService::_SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg)
    {
        CConnCtxPtr pCtx = m_pConnMgr->GetCtx(nCtxId);
        if (pCtx)
            return pCtx->Write(pMsg);
        return false;
    }

    void IService::_SetSrvOption(ESrvOpt eOpt, size_t nOptData)
    {
        switch (eOpt)
        {
        case OPT_SOCK_RCVBUF:
        {
            assert(SOCK_RCVBUF_MAX == m_nSockRcvBuffMax);
            m_nSockRcvBuffMax = std::max(size_t(SOCK_RCVBUF_MIN * 2), nOptData);
            break;
        }
        case OPT_SOCK_SNDBUF:
        {
            assert(SOCK_RCVBUF_MAX == m_nSockSndBuffMax);
            m_nSockSndBuffMax = std::max(size_t(SOCK_RCVBUF_MIN * 2), nOptData);
            break;
        }
        case OPT_LOGGER_HADNLER:
        {
            assert(0 == m_pRuntimeLogger);
            m_pRuntimeLogger = (ILoggerHandler*)nOptData;
            break;
        }
        case OPT_MESSAGE_HADNLER:
        {
            assert(0 == m_pRuntimeHandler);
            m_pRuntimeHandler = (IMessageHandler*)nOptData;
            break;
        }
        default:
            break;
        }
    }
}
