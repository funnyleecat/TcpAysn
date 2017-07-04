#include "stdafx.h"
#include "ConnMgr.h"
#include "ConnCtx.h"
#include "IOthreadpool.h"


namespace tcpcomm2
{
    CConnMgr::CConnMgr(IService& Srv)
        : IComponent(Srv), m_nMgrCtxId(1)
    {

    }

    CConnMgr::~CConnMgr()
    {
        DelAllCtx();
    }

    CConnCtxPtr CConnMgr::NewCtx()
    {
        CConnCtxPtr cp;

        try
        {
            _IOService& Ios = m_Srv.m_pIOthreadpool->GetIOService();
            _Writelock wlock(m_CtxMutex);
            cp.reset(new CConnCtx(m_Srv, Ios, m_nMgrCtxId++));
            m_CtxMap.insert(std::make_pair(cp->GetCtxID(), cp));
        }
        catch (std::exception& )
        {
        	
        }
        
        return cp;
    }

    CConnCtxPtr CConnMgr::GetCtx(size_t nCtxId)
    {
        _Readlock rlock(m_CtxMutex);
        ConnCtxMapIter It = m_CtxMap.find(nCtxId);
        return It != m_CtxMap.end() ? It->second : CConnCtxPtr();
    }

    void CConnMgr::DelCtx(size_t nCtxId)
    {
        _Writelock wlock(m_CtxMutex);
        m_CtxMap.erase(nCtxId);
    }

    void CConnMgr::DelAllCtx()
    {
        _Writelock wlock(m_CtxMutex);
        m_CtxMap.clear();
    }

}

