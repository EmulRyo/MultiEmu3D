#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "Utils.h"

void PrintfVisualStudioOutput(const char* szFormat, ...)
{
    char szBuff[1024];
    va_list arg;
    va_start(arg, szFormat);
    _vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
    va_end(arg);

    OutputDebugStringA(szBuff);
}