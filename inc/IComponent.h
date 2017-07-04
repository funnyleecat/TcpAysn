/***********************************************************************
ģ����		: tcpcomm
�ļ���		: IComponent.h
����			: DW
��������		: 2016/09/20
��ע			: ������TcpComm�����ڲ�����Ļ���
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
**************************************************************************/

#pragma once

#include "IService.h"

namespace tcpcomm2
{
    class IComponent
    {
    public:
        IComponent(IService& Srv) :m_Srv(Srv){}
        virtual ~IComponent(){};

    protected:
        IService& m_Srv;
        IComponent(IComponent& chs);
        IComponent& operator =(IComponent& chs);
    };
}


