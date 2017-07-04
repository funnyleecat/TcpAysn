#pragma once

#include "types.h"
#include "ConnCtx.h"
#include "IComponent.h"

namespace tcpcomm2
{
    class CTimeEntry;
    class CConnCtx;

    typedef boost::weak_ptr<CConnCtx> CConnCtxWeakPtr;
    typedef boost::shared_ptr<CConnCtx> CConnCtxPtr;

    typedef boost::weak_ptr<CTimeEntry> TimeEntryWeakPtr;
    typedef boost::shared_ptr<CTimeEntry> TimeEntryPtr;
    typedef boost::unordered_set<TimeEntryPtr> TimeEntrySet;
    typedef boost::circular_buffer<TimeEntrySet> TimeEntryWheel;

    class CTimeEntry
        : public IComponent
    {
    public:
        CTimeEntry(IService& Srv, std::size_t dwConnId);
        ~CTimeEntry();
    private:
        std::size_t	m_dwConnId;
    };

    class CTimeWheel
        : public IComponent
    {
    public:
        CTimeWheel(IService& Srv);
        virtual ~CTimeWheel();

        bool Run();
        void Stop();
        void TimeWheelLoop();
        void OnUpdateConn(CConnCtxPtr pCtx);
        void OnUpdateWheel();

    private:
        _ThreadPtr      m_pThread;
        _Mutex		    m_WheelMutex;
        TimeEntryWheel  m_EntryWheel;
    };
}
