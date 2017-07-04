#pragma once

#include <deque>
#include <boost/shared_ptr.hpp>
#include "tcpcomm2_define.h"
using namespace std;

namespace tcpcomm2
{
    typedef boost::shared_ptr<TcpMessage> TcpMessagePtr;
    typedef deque<TcpMessagePtr> TcpMessageQueue;
    typedef pair<char*, size_t> TcpSendBuff;

    //·¢ËÍ»º³åÇø
    class CSockSendBuffer
        
    {
    public:
        CSockSendBuffer();
        ~CSockSendBuffer();

        bool Write(const TcpMessage* pMsg);
        bool OnSendCompleted(size_t nByteTrans);
        TcpSendBuff GetCurrentBuff();

    private:
        void NotifyMessageStatus(const TcpMessagePtr& pMsg, bool bStatus);
        CSockSendBuffer(const CSockSendBuffer& rhs);
        CSockSendBuffer& operator =(const CSockSendBuffer& rhs);

    private:
        size_t m_nSentLen;
        TcpMessagePtr m_pSending;
        TcpMessageQueue m_WaitSendQueue;
    };
}

