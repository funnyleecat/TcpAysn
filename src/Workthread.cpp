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
            //创建交换队列从work队列读取消息
            //避免创建创建临时队列交换导致内存new/delete操作
            _WorkQueue SwapQueue;

            //因为每次调用ConnMgr查询链接都会有加锁操作
            //使用线程内局部set保存已经断开的链接可以减少加锁次数
            //缺点work线程如果比较多的话会有许多相同副本，内存资源上会有一定的浪费
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
                        //在线程内局部set中检查链接是否断开
                        if (tmpDisconnectSet.find((*It)->nCtxId) == tmpDisconnectSet.end())
                        {
                            //在全局map中检查链接是否断开
                            if (m_Srv.m_pConnMgr->GetCtx((*It)->nCtxId))
                            {
                                //链接没有断开则处理数据
                                DoWork(*It);
                            }                             
                            else
                            {
                                //在全局map中检查到链接已断开则记录到线程内局部set
                                tmpDisconnectSet.insert((*It)->nCtxId);
                                OUTPUT_DEBUG_LOG("CTX[%d] ALREADY DISCONNECTED", (*It)->nCtxId);
                            }
                        }
                        else
                        {
                            OUTPUT_DEBUG_LOG("CTX[%d] ALREADY DISCONNECTED", (*It)->nCtxId);
                        }
                            
                    }

                    //回收内存资源
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
                //在IO线程接收数据已经做过一次数据校验，所以在此处数据必须合法的
                //如果此处还出现异常情况，只能把链接关闭了
                m_Srv._Close(pWorkData->nCtxId);
                return;
            }

            m_Srv.m_pRuntimeHandler->HandleCommRequest(\
                pWorkData->nCtxId, pReadData, nByteRead);

            nByteParse += nByteRead;
        }
    }
}
