#pragma once

#include <string>
#include "tcpcomm2_export.h"
using namespace std;

/*
* 类名：CRequestHandler
* 说明：默认实现echo功能，用户可以继承此类解析自定义通信数据结构。
*/

namespace tcpcomm2
{
    class TCPCOMM2_API IMessageHandler
    {
    public:
        virtual ~IMessageHandler(){}

        /*
        * 函数名称： HandleCommRecvBuffer
        * 使用说明： 1、框架内会循环调用此函数直到函数返回0或者<0，每次调用最多只能读取一个包长。
        2、框架在收到一个完整的数据包时会将数据包投递至框架内的消息队列。
        * 输入参数： <size_t nCtxId> 标识唯一客户端的ID。
        * 输入参数： <const char* pReq> 服务器接收到来自客户端的数据。
        * 输入参数： <size_t nReqLen> 服务器接收到来自客户端的数据长度。
        * 输出参数：
        *	返回值 > 0 表示服务器已接收到一个完整数据包， 返回值表示数据包长度。
        *	返回值 = 0 表示服务器未接收到一个完整的数据包， 需要继续接收数据。
        *	返回值 < 0 表示服务器接收一个非法的数据包， 服务器会立即关闭链接、停止接收此客户端数据。
        */
        virtual size_t HandleCommRecvBuffer(size_t nCtxId, const char* pReq, size_t nReqLen) = 0;

        /*
        * 函数名称： HandleCommRequest
        * 使用说明： 框架从消息队列中读取到数据包后调用。
        * 输入参数： <size_t nCtxId> 标识唯一客户端的ID。
        * 输入参数： <const char* pReq> 客户端发过来的数据包。
        * 输入参数： <int nReqLen> 客户端发过来的数据包长度。
        */
        virtual void HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen) = 0;

        /*
        *函数名称：HandlCommConnected
        *当有客户端连接时被调用
        *输入参数：<size_t nCtxId> 链接的上下文ID
        */
        virtual void HandleCommConnected(size_t nCtxId) = 0;

        /*
        *函数名称：HandlCommDisConnect
        *当有客户端断开时被调用
        *输入参数：<size_t nCtxId> 链接的上下文ID
        */
        virtual void HandleCommDisconnect(size_t nCtxId) = 0;
    };
}



