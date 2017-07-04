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

#include "IService.h"
#include "ITcpClient.h"

namespace tcpcomm2
{
    class CTcpClient
        : public IService
        , public ITcpClient
    {
        friend class ITcpClient;
    public:
        virtual void SetSrvOption(ESrvOpt eOpt, size_t nOptData);
        virtual bool StartService();
        virtual void StopService();
        virtual size_t Connect(const char* pszAddress, unsigned short nPort);
        virtual void Close(size_t nCtxId);
        virtual bool SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg);
        virtual void OnUpdateConn(CConnCtxPtr pCtx);

    private:
        CTcpClient(int nIothread, int nWorkthread);
        virtual ~CTcpClient();
    };
}

