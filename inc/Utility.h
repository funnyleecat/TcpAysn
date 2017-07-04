#pragma once

#include <string>
#include <stdarg.h>
#include "ILoggerHandler.h"
using namespace std;

namespace tcpcomm2
{
    string Vsprintf_t(const char* format, ...);

#define OUTPUT_DEBUG_LOG(format, ...) { \
    string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
    m_Srv.m_pRuntimeLogger->Debug(tmp.c_str()); \
    }

#define OUTPUT_INFO_LOG(format, ...) { \
    string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
    m_Srv.m_pRuntimeLogger->Info(tmp.c_str()); \
    }

#define OUTPUT_WARN_LOG(format, ...) { \
    string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
    m_Srv.m_pRuntimeLogger->Warn(tmp.c_str()); \
    }

#define OUTPUT_ERROR_LOG(format, ...) { \
    string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
    m_Srv.m_pRuntimeLogger->Error(tmp.c_str()); \
    }

#define OUTPUT_FATAL_LOG(format, ...) { \
    string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
    m_Srv.m_pRuntimeLogger->Fatal(tmp.c_str()); \
    }
}
