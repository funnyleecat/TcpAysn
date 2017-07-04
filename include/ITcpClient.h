/***********************************************************************
模块名		: tcpcomm
文件名		: ITcpClient.h
作者			: DW
创建日期		: 2016/09/20
备注			: 定义了TcpClient的接口类
-----------------------------------------------------------------------
修改记录 :
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y  <作者或修改者名>       <修改内容>
**************************************************************************/

#pragma once

#include "tcpcomm2_export.h"
#include "tcpcomm2_define.h"

namespace tcpcomm2
{
    class ILoggerHandler;
    class IMessageHandler;

    class TCPCOMM2_API ITcpClient
    {
    public:
        static ITcpClient* GetInstance(int nIothread, int nWorkthread);
        static void Release(ITcpClient* pInstance);

        virtual void SetSrvOption(ESrvOpt eOpt, size_t nOptData) = 0;
        virtual bool StartService() = 0;
        virtual void StopService() = 0;
        virtual size_t Connect(const char* pszAddress, unsigned short nPort) = 0;
        virtual void Close(size_t nCtxId) = 0;
        virtual bool SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg) = 0;

    protected:
        ITcpClient(){};
        virtual ~ITcpClient(){};
        ITcpClient(ITcpClient& rhs);
        ITcpClient& operator =(ITcpClient& rhs);
    };
}

