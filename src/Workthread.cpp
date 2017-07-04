#include "stdafx.h"
#include "Workthread.h"
#include "ILoggerHandler.h"
#include "IMessageHandler.h"
#include "ConnMgr.h"
#include "ConnCtx.h"
#include "Utility.h"

namespace tcpcomm2
{
    CWorkthread::CWorkthread(IService& Srv)
        : IComponent(Srv)
    {
        
    }

    CWorkthread::~CWorkthread()
    {
        Stop();
    }

    bool CWorkthread::Run()
    {
        m_pThread.reset(new _Thread(boost::bind(&CWorkthread::WorkLoop, this)));
        return 0 != m_pThread;
    }

    void CWorkthread::Stop()
    {
        if (m_pThread)
        {
            m_pThread->interrupt();
            m_pThread->join();
            m_pThread.reset();
        }
    }

    void CWorkthread::WakeAndPush(TcpWorkData* pWorkData)
    {
        {
            _Autolock lock(m_Mutex);
            m_WorkQueue.push_back(pWorkData);          
        }

        m_Condition.notify_one();
    }

    void CWorkthread::WaitAndPop(_WorkQueue& tmpQueue)
    {
        _Autolock lock(m_Mutex);
        if (m_WorkQueue.empty())
            m_Condition.wait(lock);

        m_WorkQueue.swap(tmpQueue);
    }

    void CWorkthread::WorkLoop()
    {
        try
        {
            //�����������д�work���ж�ȡ��Ϣ
            //���ⴴ��������ʱ���н��������ڴ�new/delete����
            _WorkQueue SwapQueue;

            //��Ϊÿ�ε���ConnMgr��ѯ���Ӷ����м�������
            //ʹ���߳��ھֲ�set�����Ѿ��Ͽ������ӿ��Լ��ټ�������
            //ȱ��work�߳�����Ƚ϶�Ļ����������ͬ�������ڴ���Դ�ϻ���һ�����˷�
            set<size_t> tmpDisconnectSet;

            while (1)
            {
                boost::this_thread::interruption_point();
                WaitAndPop(SwapQueue);

                _WorkQueue::iterator It = SwapQueue.begin();
                for (; It != SwapQueue.end(); ++It)
                {
                    if (WORKEVT_CONNECTED == (*It)->wEvent)
                    {
                        m_Srv.m_pRuntimeHandler->HandleCommConnected((*It)->nCtxId);
                    }                      
                    else if (WORKEVT_DISCONNECTED == (*It)->wEvent)
                    {
                        m_Srv.m_pRuntimeHandler->HandleCommDisconnect((*It)->nCtxId);
                    }
                        
                    else if (WORKEVT_RECVDATA == (*It)->wEvent)
                    {
                        //���߳��ھֲ�set�м�������Ƿ�Ͽ�
                        if (tmpDisconnectSet.find((*It)->nCtxId) == tmpDisconnectSet.end())
                        {
                            //��ȫ��map�м�������Ƿ�Ͽ�
                            if (m_Srv.m_pConnMgr->GetCtx((*It)->nCtxId))
                            {
                                //����û�жϿ���������
                                DoWork(*It);
                            }                             
                            else
                            {
                                //��ȫ��map�м�鵽�����ѶϿ����¼���߳��ھֲ�set
                                tmpDisconnectSet.insert((*It)->nCtxId);
                                OUTPUT_DEBUG_LOG("CTX[%d] ALREADY DISCONNECTED", (*It)->nCtxId);
                            }
                        }
                        else
                        {
                            OUTPUT_DEBUG_LOG("CTX[%d] ALREADY DISCONNECTED", (*It)->nCtxId);
                        }
                            
                    }

                    //�����ڴ���Դ
                    TcpWorkDataDestory(*It);
                }

                SwapQueue.clear();
            }
        }
        catch (std::exception&)
        {
            OUTPUT_INFO_LOG("CWorkthread::WorkLoop HAS BEEN INTERRUPT");
        }
    }

    void CWorkthread::DoWork(TcpWorkData* pWorkData)
    {
        size_t nByteParse = 0;

        while (nByteParse < pWorkData->nReqLen)
        {
            char* pReadData = pWorkData->ReqData + nByteParse;
            size_t nByteRead = m_Srv.m_pRuntimeHandler->HandleCommRecvBuffer(\
                pWorkData->nCtxId, pReadData, pWorkData->nReqLen - nByteParse);

            if (((size_t)-1) == nByteRead || (0 == nByteRead))
            {
                //��IO�߳̽��������Ѿ�����һ������У�飬�����ڴ˴����ݱ���Ϸ���
                //����˴��������쳣�����ֻ�ܰ����ӹر���
                m_Srv._Close(pWorkData->nCtxId);
                return;
            }

            m_Srv.m_pRuntimeHandler->HandleCommRequest(\
                pWorkData->nCtxId, pReadData, nByteRead);

            nByteParse += nByteRead;
        }
    }
}
