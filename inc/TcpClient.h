/***********************************************************************
ģ����		: tcpcomm
�ļ���		: ITcpClient.h
����			: DW
��������		: 2016/09/20
��ע			: ������TcpClient�Ľӿ���
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
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

