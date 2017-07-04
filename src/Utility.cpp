#include "stdafx.h"
#include <stdarg.h>
#include "Utility.h"

#ifdef WIN32
#define __Vsprintf_t vsprintf_s
#else
#define __Vsprintf_t vsnprintf
#endif

namespace tcpcomm2
{
    string Vsprintf_t(const char* format, ...)
    {
        string strReturn;

        int size = 4096;
        char* buffer = new char[size];

        while (1)
        {
            va_list args;
            va_start(args, format);
            int n = __Vsprintf_t(buffer, size, format, args);
            va_end(args);

            if ((n > 0) && (n < size))
            {
                strReturn = buffer;
                delete[] buffer;
                break;
            }
            else
            {
                size *= 2;
                delete[] buffer;
                buffer = new char[size];
            }
        }

        return strReturn;
    }
}
