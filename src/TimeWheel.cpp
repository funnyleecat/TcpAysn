#include "stdafx.h"
#include "TimeWheel.h"
#include "Utility.h"
#include "ConnMgr.h"

namespace tcpcomm2
{

    CTimeEntry::CTimeEntry(IService& Srv, std::size_t dwConnId)
        :IComponent(Srv), m_dwConnId(dwConnId)
    {
    }

    CTimeEntry::~CTimeEntry()
    {
        CConnCtxPtr pConn = m_Srv.m_pConnMgr->GetCtx(m_dwConnId);
        if (pConn)
            pConn->OUTPUT_CONNCTX_LOG("HAS BEEN KICK", LOGLV_ERROR);
    }
    CTimeWheel::CTimeWheel(IService& Srv)
        : IComponent(Srv)
        , m_EntryWheel(m_Srv.m_nKeepalive)
    {
        OnUpdateWheel();
    }

    CTimeWheel::~CTimeWheel()
    {
        Stop();
    }

    bool CTimeWheel::Run()
    {
        bool bReturn = false;
        try
        {
            m_pThread.reset(new _Thread(boost::bind(&CTimeWheel::TimeWheelLoop, this)));
            bReturn = true;
        }
        catch (std::exception&)
        {
        	
        }

        return bReturn;
    }

    void CTimeWheel::Stop()
    {
        if (m_pThread)
        {
            m_pThread->interrupt();
            m_pThread->join();
            m_pThread.reset();
        }
    }

    void CTimeWheel::TimeWheelLoop()
    {
        try
        {
            do 
            {
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                OnUpdateWheel();
            } while (1);
        }
        catch (std::exception&)
        {
            OUTPUT_INFO_LOG("CTimeWheel::TimeWheelLoop HAS BEEN INTERRUPT");
        }
    }

    void CTimeWheel::OnUpdateConn(CConnCtxPtr pCtx)
    {
        TimeEntryPtr pEntry = pCtx->m_pWeakEntry.lock();

        try
        {
            if (!pEntry)
            {
                pEntry = TimeEntryPtr(new CTimeEntry(m_Srv,pCtx->GetCtxID()));
                pCtx->m_pWeakEntry = pEntry;
            }
            
            _Autolock lock(m_WheelMutex);
            m_EntryWheel.back().insert(pEntry);
        }
        catch (std::exception&)
        {

        }       
    }

    void CTimeWheel::OnUpdateWheel()
    {
        TimeEntrySet EntrySet;
        _Autolock lock(m_WheelMutex);
        m_EntryWheel.push_back(EntrySet);   
    }
}
