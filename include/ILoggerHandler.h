#pragma once
#include "tcpcomm2_export.h"

namespace tcpcomm2
{
    class TCPCOMM2_API ILoggerHandler
    {
    public:
        virtual ~ILoggerHandler(){};
        virtual void Debug(const char* pszString) = 0;
        virtual void Info(const char* pszString) = 0;
        virtual void Warn(const char* pszString) = 0;
        virtual void Error(const char* pszString) = 0;
        virtual void Fatal(const char* pszString) = 0;
    };
}
