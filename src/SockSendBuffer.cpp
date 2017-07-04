#include "stdafx.h"
#include "SockSendBuffer.h"

namespace tcpcomm2
{
    CSockSendBuffer::CSockSendBuffer()
        : m_nSentLen(0)
    {

    }

    CSockSendBuffer::~CSockSendBuffer()
    {
        //发送缓冲区析构时还有数据没有发送
        //将这些未发送数据的控制权归还给用户，并提示用户未发送成功
        if (m_pSending)
            NotifyMessageStatus(m_pSending, false);

        deque<TcpMessagePtr>::iterator It = m_WaitSendQueue.begin();
        for (; It != m_WaitSendQueue.end(); ++It)
            NotifyMessageStatus(*It, false);
    }

    bool CSockSendBuffer::Write(const TcpMessage* pMsg)
    {
        bool bReturn = false;

        try
        {
            do 
            {
                if (!pMsg->pData || !pMsg->nDataLen)
                    break;

                TcpMessagePtr mp(new (std::nothrow) TcpMessage);
                if (!mp) break;
                    
                memcpy(mp.get(), pMsg, sizeof(TcpMessage));

                if (!m_pSending)
                    m_pSending = mp;
                else
                    m_WaitSendQueue.push_back(mp);

                bReturn = true;
            } while (0);        
        }
        catch (std::exception&)
        {
        }
        
        return bReturn;
    }

    bool CSockSendBuffer::OnSendCompleted(size_t nByteTrans)
    {
        m_nSentLen += nByteTrans;

        //当前发送数据没有全部发送完成 - 返回true表示还有数据需要发送
        if (m_nSentLen < m_pSending->nDataLen)
            return true;

        //当前发送数据全部发送完成 - 通知用户后清理缓存数据
        NotifyMessageStatus(m_pSending, true);
        m_nSentLen = 0;
        m_pSending.reset();

        //判断待发送队列是否还有未发送数据 - 返回false表示所有数据都已发送完成
        if (m_WaitSendQueue.empty())
            return false;

        //从待发送队列中取出下一条待发送消息，继续发送
        m_pSending = m_WaitSendQueue.front();
        m_WaitSendQueue.pop_front();
        return true;
    }

    TcpSendBuff CSockSendBuffer::GetCurrentBuff()
    {
        if (m_pSending && m_pSending->pData)
            return make_pair(m_pSending->pData, m_pSending->nDataLen - m_nSentLen);
        else
            return make_pair((char*)0, 0);
    }

    void CSockSendBuffer::NotifyMessageStatus(const TcpMessagePtr& pMsg, bool bStatus)
    {
        if (pMsg->pCallback)
            pMsg->pCallback(pMsg->pData, pMsg->nDataLen, bStatus);
    }
}

