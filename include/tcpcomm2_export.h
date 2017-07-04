#pragma once

#ifdef WIN32

#ifdef TCPCOMM2_EXPORTS
#   define TCPCOMM2_API __declspec(dllexport)
#else
#   define TCPCOMM2_API __declspec(dllimport)
#endif //TCPCOMM_EXPORTS

#else

#define TCPCOMM2_API

#endif	//WIN32
