#pragma once

#include <vector>
#include "types.h"
#include "IComponent.h"
using namespace std;

namespace tcpcomm2
{
    class CIOthreadpool
        : public IComponent
    {
    public:
        CIOthreadpool(IService& Srv);
        virtual ~CIOthreadpool();
        bool Run(size_t nPoolSize);
        void Stop();
        _IOService& GetIOService();

    private:
        AtomicSize_t m_nCurrent;
        vector<_IOServicePtr> m_SrvPool;
        vector<_IOServiceWorkPtr> m_WorkPool;
        vector<_ThreadPtr> m_ThreadPool;
    };
}

