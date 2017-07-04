#include "stdafx.h"
#include "Workthreadpool.h"
#include "Workthread.h"

namespace tcpcomm2
{
    CWorkthreadpool::CWorkthreadpool(IService& Srv)
        : IComponent(Srv), m_nCurrent(0)
    {
        
    }

    CWorkthreadpool::~CWorkthreadpool()
    {
        Stop();
    }

    bool CWorkthreadpool::Run(size_t nPoolSize)
    {
        bool bReturn = false;

        try
        {
            for (size_t i = 0; i < nPoolSize; ++i)
            {
                CWorkthreadPtr wp(new CWorkthread(m_Srv));
                wp->Run();
                m_ThreadPool.push_back(wp);
            }

            bReturn = true;
        }
        catch (std::exception&)
        {

        }

        if (!bReturn) Stop();
        return bReturn;
    }

    void CWorkthreadpool::Stop()
    {
        for (size_t i = 0; i < m_ThreadPool.size(); ++i)
            m_ThreadPool[i]->Stop();

        m_ThreadPool.clear();
    }

    void CWorkthreadpool::Push(TcpWorkData* pWorkData)
    {
/*        size_t temp = (++m_nCurrent) % m_ThreadPool.size();*/

        /*
        * ʹ��nCtxIdȡģ������֤ÿ��nCtxId�����ݶ��ᱻ���䵽�ƶ��Ĺ����̵߳���
        * �Դ�ȷ��ҵ�����ݵ�ʱ����
        */

        size_t temp = pWorkData->nCtxId % m_ThreadPool.size();
        return m_ThreadPool[temp]->WakeAndPush(pWorkData);
    }
}
