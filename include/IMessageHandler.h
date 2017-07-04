#pragma once

#include <string>
#include "tcpcomm2_export.h"
using namespace std;

/*
* ������CRequestHandler
* ˵����Ĭ��ʵ��echo���ܣ��û����Լ̳д�������Զ���ͨ�����ݽṹ��
*/

namespace tcpcomm2
{
    class TCPCOMM2_API IMessageHandler
    {
    public:
        virtual ~IMessageHandler(){}

        /*
        * �������ƣ� HandleCommRecvBuffer
        * ʹ��˵���� 1������ڻ�ѭ�����ô˺���ֱ����������0����<0��ÿ�ε������ֻ�ܶ�ȡһ��������
        2��������յ�һ�����������ݰ�ʱ�Ὣ���ݰ�Ͷ��������ڵ���Ϣ���С�
        * ��������� <size_t nCtxId> ��ʶΨһ�ͻ��˵�ID��
        * ��������� <const char* pReq> ���������յ����Կͻ��˵����ݡ�
        * ��������� <size_t nReqLen> ���������յ����Կͻ��˵����ݳ��ȡ�
        * ���������
        *	����ֵ > 0 ��ʾ�������ѽ��յ�һ���������ݰ��� ����ֵ��ʾ���ݰ����ȡ�
        *	����ֵ = 0 ��ʾ������δ���յ�һ�����������ݰ��� ��Ҫ�����������ݡ�
        *	����ֵ < 0 ��ʾ����������һ���Ƿ������ݰ��� �������������ر����ӡ�ֹͣ���մ˿ͻ������ݡ�
        */
        virtual size_t HandleCommRecvBuffer(size_t nCtxId, const char* pReq, size_t nReqLen) = 0;

        /*
        * �������ƣ� HandleCommRequest
        * ʹ��˵���� ��ܴ���Ϣ�����ж�ȡ�����ݰ�����á�
        * ��������� <size_t nCtxId> ��ʶΨһ�ͻ��˵�ID��
        * ��������� <const char* pReq> �ͻ��˷����������ݰ���
        * ��������� <int nReqLen> �ͻ��˷����������ݰ����ȡ�
        */
        virtual void HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen) = 0;

        /*
        *�������ƣ�HandlCommConnected
        *���пͻ�������ʱ������
        *���������<size_t nCtxId> ���ӵ�������ID
        */
        virtual void HandleCommConnected(size_t nCtxId) = 0;

        /*
        *�������ƣ�HandlCommDisConnect
        *���пͻ��˶Ͽ�ʱ������
        *���������<size_t nCtxId> ���ӵ�������ID
        */
        virtual void HandleCommDisconnect(size_t nCtxId) = 0;
    };
}



