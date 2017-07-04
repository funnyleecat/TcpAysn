#pragma once

#include <deque>
#include "types.h"
#include "IComponent.h"
using namespace std;

typedef deque<TcpWorkData*> _WorkQueue;

namespace tcpcomm2
{
    class CWorkthread
        : public IComponent
    {
    public:
        CWorkthread(IService& Srv);
        ~CWorkthread();
        bool Run();
        void Stop();
        void WorkLoop();
        void WakeAndPush(TcpWorkData* pWorkData);        

    private:
        void WaitAndPop(_WorkQueue& tmpQueue);
        void DoWork(TcpWorkData* pWorkData);

    private:
        _Mutex m_Mutex;
        _Condition m_Condition;
        _ThreadPtr m_pThread;
        _WorkQueue m_WorkQueue;
    };
}

