/***********************************************************************
ģ����		: tcpcomm
�ļ���		: IService.h
����			: DW
��������		: 2016/09/20
��ע			: ������tcpcomm�ⲿ�������
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
**************************************************************************/

#pragma once

#include "tcpcomm2_define.h"

namespace tcpcomm2
{
    class ILoggerHandler;
    class IMessageHandler;

    class CConnCtx;
    typedef boost::shared_ptr<CConnCtx> CConnCtxPtr;

    class CConnMgr;
    typedef boost::shared_ptr<CConnMgr> CConnMgrPtr;

    class CIOthreadpool;
    typedef boost::shared_ptr<CIOthreadpool> CIOthreadpoolPtr;

    class CWorkthreadpool;
    typedef boost::shared_ptr<CWorkthreadpool> CWorkthreadpoolPtr;

    class IService
    {
    public:
        IService(int nIothread, int nWorkthread, int nKeepalive);
        virtual ~IService();

        int m_nKeepalive;
        int m_nIOthread;
        int m_nWorkthread;

        size_t m_nSockRcvBuffMax;
        size_t m_nSockSndBuffMax;

        CConnMgrPtr m_pConnMgr;
        CIOthreadpoolPtr m_pIOthreadpool;
        CWorkthreadpoolPtr m_pWorkthreadpool;
        ILoggerHandler* m_pRuntimeLogger;
        IMessageHandler* m_pRuntimeHandler;

        virtual void OnUpdateConn(CConnCtxPtr pCtx) = 0;

    public:
        void _StopService();
        bool _StartService();
        void _SetSrvOption(ESrvOpt eOpt, size_t nOptData);           
        void _Close(size_t nCtxId);
        bool _SendTcpMessage(size_t nCtxId, const TcpMessage* pMsg);

    private:
        IService(const IService& rhs);
        IService& operator =(const IService& rhs);
    };
}
