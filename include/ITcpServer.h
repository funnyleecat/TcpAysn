/***********************************************************************
ģ����		: tcpcomm
�ļ���		: ITcpServer.h
����			: DW
��������		: 2016/09/20
��ע			: ������TcpServer�Ľӿ���
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
**************************************************************************/

#pragma once
#include "tcpcomm2_export.h"
#include "tcpcomm2_define.h"

namespace tcpcomm2
{
    class ILoggerHandler;
    class IMessageHandler;

    class TCPCOMM2_API ITcpServer
    {
    public:
        static ITcpServer* GetInstance(int nIothread, int nWorkthread, int nKeepalive = 30);
        static void Release(ITcpServer* pInstance);

        virtual void SetSrvOption(ESrvOpt eOpt, size_t nOptData) = 0;
        virtual bool StartService(unsigned short nPort) = 0;
        virtual void StopService() = 0;
        virtual void Close(size_t nCtxId) = 0;
        virtual bool SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg) = 0;

    protected:
        ITcpServer(){};
        virtual ~ITcpServer(){};
        ITcpServer(ITcpServer& rhs);
        ITcpServer& operator =(ITcpServer& rhs);
    };
}


