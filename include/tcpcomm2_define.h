#pragma once

#include <cstddef>  //for size_t

#ifdef __cplusplus
extern "C" {
#endif

    enum ESrvOpt
    {
        //设置接收缓冲区大小
        //默认最大长度为64K、最小长度为8K，如果设置的值小于8K则不生效
        OPT_SOCK_RCVBUF = 1, 

        //设置发送缓冲区大小，目前没有用到    
        OPT_SOCK_SNDBUF = 2,

        //设置日志局回调句柄
        //回调对象从tcpcomm2::ILoggerHandler继承，此选项必须设置，否则实例启动失败
        OPT_LOGGER_HADNLER = 3,

        //设置消息局回调句柄
        //回调对象从tcpcomm2::IMessageHandler继承，此选项必须设置，否则实例启动失败
        OPT_MESSAGE_HADNLER = 4,

        //设置链接接收器工作模式，仅TcpServer对象有效
        //0 : 异步接收模式(默认)，调用TcpServer->StartSerivce()后当前线程会返回
        //1 : 同步接收模式，调用TcpServer->StartSerivce()后当前线程将会被阻塞
        OPT_ACCEPTOR_MODE = 5,
    };

    typedef void(*pfnSentCallback)(char*, size_t, bool);
    typedef struct tagTcpMessage
    {
        char* pData;
        size_t nDataLen;
        pfnSentCallback pCallback;
    }TcpMessage;

#ifdef __cplusplus
}
#endif
