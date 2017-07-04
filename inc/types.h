#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0501 
#endif // _WIN32

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_set.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/enable_shared_from_this.hpp>

typedef boost::asio::io_service				_IOService;
typedef boost::shared_ptr<_IOService>		_IOServicePtr;

typedef boost::asio::io_service::work		_IOServiceWork;
typedef boost::shared_ptr<_IOServiceWork>	_IOServiceWorkPtr;

typedef boost::asio::ip::tcp::socket		_TcpSocket;
typedef boost::shared_ptr<_TcpSocket>		_TcpSocketPtr;

typedef boost::asio::ip::tcp::acceptor		_TcpAcceptor;
typedef boost::shared_ptr<_TcpAcceptor>     _TcpAcceptorPtr;

typedef boost::asio::ip::tcp::endpoint		_TcpEndpoint;

typedef boost::thread						_Thread;
typedef boost::shared_ptr<_Thread>		    _ThreadPtr;

typedef boost::mutex						_Mutex;
typedef boost::shared_mutex					_RWMutex;
typedef boost::condition_variable_any		_Condition;

typedef boost::mutex::scoped_lock			_Autolock;
typedef boost::shared_lock<_RWMutex>	    _Readlock;
typedef boost::unique_lock<_RWMutex>	    _Writelock;

typedef boost::system::error_code			Syserr_t;
typedef boost::atomics::atomic_ulong		AtomicSize_t;

enum 
{
    //--默认接收缓冲区最小长度
    SOCK_RCVBUF_MIN = 1024 * 4, 

    //--默认接收缓冲区最大长度
    SOCK_RCVBUF_MAX = 1024 * 64, 
};

enum LOG_LEVEL
{
    LOGLV_DEBUG,
    LOGLV_INFO,
    LOGLV_WARN,
    LOGLV_ERROR,
    LOGLV_FATAL,
};

enum WORKEVENT
{
    WORKEVT_CONNECTED,
    WORKEVT_RECVDATA,
    WORKEVT_DISCONNECTED,
    WORKEVT_UNKNOWN,
};

typedef struct tagTcpWorkData
{
    WORKEVENT wEvent;
    size_t nCtxId;
    size_t nReqLen;
    char ReqData[1];
}TcpWorkData;

namespace tcpcomm2
{
    __inline TcpWorkData* TcpWorkDataCreate(size_t nCtxId, const char* pData, size_t nByte, WORKEVENT wEvent)
    {
        size_t nWorkData = sizeof(TcpWorkData) + nByte;
        TcpWorkData* pWorkData = (TcpWorkData*)new(std::nothrow) char[nWorkData];

        if (!pWorkData)
            return 0;

        pWorkData->wEvent = wEvent;
        pWorkData->nCtxId = nCtxId;
        pWorkData->nReqLen = nByte;

        if (pData)
            memcpy(pWorkData->ReqData, pData, nByte);

        return pWorkData;
    }

    __inline void TcpWorkDataDestory(TcpWorkData* pWorkData)
    {
        if (pWorkData)
        {
            char* p = (char*)pWorkData;
            delete[] p;
        }
    }
}

    
