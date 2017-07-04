#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include "IComponent.h"
#include "types.h"
using namespace std;

namespace tcpcomm2
{
    class CWorkthread;
    typedef boost::shared_ptr<CWorkthread> CWorkthreadPtr;

    class CWorkthreadpool
        : public IComponent
    {
    public:
        CWorkthreadpool(IService& Srv);
        ~CWorkthreadpool();
        bool Run(size_t nPoolSize);
        void Stop();
        void Push(TcpWorkData* pWorkData);

    private:
        AtomicSize_t m_nCurrent;
        vector<CWorkthreadPtr> m_ThreadPool;
    };
}

