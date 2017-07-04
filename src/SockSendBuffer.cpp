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
        //���ͻ���������ʱ��������û�з���
        //����Щδ�������ݵĿ���Ȩ�黹���û�������ʾ�û�δ���ͳɹ�
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

        //��ǰ��������û��ȫ��������� - ����true��ʾ����������Ҫ����
        if (m_nSentLen < m_pSending->nDataLen)
            return true;

        //��ǰ��������ȫ��������� - ֪ͨ�û�������������
        NotifyMessageStatus(m_pSending, true);
        m_nSentLen = 0;
        m_pSending.reset();

        //�жϴ����Ͷ����Ƿ���δ�������� - ����false��ʾ�������ݶ��ѷ������
        if (m_WaitSendQueue.empty())
            return false;

        //�Ӵ����Ͷ�����ȡ����һ����������Ϣ����������
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

