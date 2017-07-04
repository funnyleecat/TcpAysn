#include "stdafx.h"
#include "TcpClient.h"
#include "ConnMgr.h"
#include "ConnCtx.h"

namespace tcpcomm2
{
    ITcpClient* ITcpClient::GetInstance(int nIothread, int nWorkthread)
    {
        return new CTcpClient(nIothread, nWorkthread);
    }

    void ITcpClient::Release(ITcpClient* pInstance)
    {
        if (pInstance) delete pInstance;
    }

    CTcpClient::CTcpClient(int nIothread, int nWorkthread)
        : IService(nIothread, nWorkthread, 1)
    {

    }

    CTcpClient::~CTcpClient()
    {

    }

    void CTcpClient::SetSrvOption(ESrvOpt eOpt, size_t nOptData)
    {
        return IService::_SetSrvOption(eOpt, nOptData);
    }

    bool CTcpClient::SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg)
    {
        return IService::_SendTcpMessage(nCtxId, pMsg);
    }

    bool CTcpClient::StartService()
    {
        return IService::_StartService();
    }

    void CTcpClient::StopService()
    {
        return IService::_StopService();
    }

    void CTcpClient::Close(size_t nCtxId)
    {
        return IService::_Close(nCtxId);
    }

    size_t CTcpClient::Connect(const char* pszAddress, unsigned short nPort)
    {
        CConnCtxPtr pCtx = m_pConnMgr->NewCtx();
        if (pCtx && pCtx->Connect(pszAddress, nPort))
        {
            return pCtx->GetCtxID();
        }
        else
        {
            if (pCtx) m_pConnMgr->DelCtx(pCtx->GetCtxID());
            return 0;
        }
    }

    void CTcpClient::OnUpdateConn(CConnCtxPtr pCtx)
    {

    }
}


