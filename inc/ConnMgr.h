#pragma once

#include <map>
#include "types.h"
#include "IComponent.h"

namespace tcpcomm2
{
    class CConnCtx;
    typedef boost::shared_ptr<CConnCtx> CConnCtxPtr;
    typedef std::map<size_t, CConnCtxPtr> ConnCtxMap;
    typedef ConnCtxMap::iterator ConnCtxMapIter;

    class CConnMgr
        : public IComponent
    {
    public:
        CConnMgr(IService& Srv);
        virtual ~CConnMgr();

        CConnCtxPtr NewCtx();
        CConnCtxPtr GetCtx(size_t nCtxId);
        void DelCtx(size_t nCtxId);
        void DelAllCtx();

    private:
        size_t m_nMgrCtxId;    
        _RWMutex m_CtxMutex;
        ConnCtxMap m_CtxMap;
    };
}

