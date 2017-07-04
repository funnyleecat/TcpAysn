#pragma once

#include <cstddef>  //for size_t

#ifdef __cplusplus
extern "C" {
#endif

    enum ESrvOpt
    {
        //���ý��ջ�������С
        //Ĭ����󳤶�Ϊ64K����С����Ϊ8K��������õ�ֵС��8K����Ч
        OPT_SOCK_RCVBUF = 1, 

        //���÷��ͻ�������С��Ŀǰû���õ�    
        OPT_SOCK_SNDBUF = 2,

        //������־�ֻص����
        //�ص������tcpcomm2::ILoggerHandler�̳У���ѡ��������ã�����ʵ������ʧ��
        OPT_LOGGER_HADNLER = 3,

        //������Ϣ�ֻص����
        //�ص������tcpcomm2::IMessageHandler�̳У���ѡ��������ã�����ʵ������ʧ��
        OPT_MESSAGE_HADNLER = 4,

        //�������ӽ���������ģʽ����TcpServer������Ч
        //0 : �첽����ģʽ(Ĭ��)������TcpServer->StartSerivce()��ǰ�̻߳᷵��
        //1 : ͬ������ģʽ������TcpServer->StartSerivce()��ǰ�߳̽��ᱻ����
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
