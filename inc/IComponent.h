/***********************************************************************
模块名		: tcpcomm
文件名		: IComponent.h
作者			: DW
创建日期		: 2016/09/20
备注			: 定义了TcpComm所有内部组件的基类
-----------------------------------------------------------------------
修改记录 :
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y  <作者或修改者名>       <修改内容>
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


