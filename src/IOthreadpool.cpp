#include "stdafx.h"
#include "IOthreadpool.h"

namespace tcpcomm2
{
    CIOthreadpool::CIOthreadpool(IService& Srv)
        : IComponent(Srv), m_nCurrent(0)
    {
    }

    CIOthreadpool::~CIOthreadpool()
    {
        Stop();
    }

    bool CIOthreadpool::Run(size_t nPoolSize)
    {
        bool bReturn = false;

        try
        {
            for (size_t i = 0; i < nPoolSize; ++i)
            {
                _IOServicePtr sp(new _IOService);
                _IOServiceWorkPtr wp(new _IOServiceWork(*sp));
                _ThreadPtr tp(new _Thread(boost::bind(&_IOService::run, sp)));

                m_SrvPool.push_back(sp);
                m_WorkPool.push_back(wp);
                m_ThreadPool.push_back(tp);
            }

            bReturn = true;
        }
        catch (std::exception&)
        {
        	
        }

        if (!bReturn) Stop();
        return bReturn;
    }

    void CIOthreadpool::Stop()
    {
        for (size_t i = 0; i < m_SrvPool.size(); ++i)
            m_SrvPool[i]->stop();

        for (size_t i = 0; i < m_ThreadPool.size(); ++i)
            m_ThreadPool[i]->join();

        m_ThreadPool.clear();
        m_WorkPool.clear();
        m_SrvPool.clear();        
    }

    _IOService& CIOthreadpool::GetIOService()
    {
        _IOService& Ios = *(m_SrvPool[m_nCurrent]);

        if (++m_nCurrent == m_SrvPool.size())
            m_nCurrent = 0;

        return Ios;
    }
}
