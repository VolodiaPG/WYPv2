#include <Arduino.h>
#include "Debug.hpp"
#include "WypWebServer.hpp"

namespace wyp
{

#ifdef WYP_DEBUG

#define MAX_PRINTF_LEN 64

Debug_c::Debug_c()
{
    ;
}

void Debug_c::SetDebugFlags(uint16_t flags)
{
    _flags = flags;
}

size_t Debug_c::Print(debug_flags_t flag, const char *endline, const char *format, ...)
{
    //     String msg = "[" + (String)ESP.getFreeHeap() + "]" + before + ((before != "") ? " " : "") + txt + endline;
    //     Serial.print(msg);

    // #if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYPWEBSERVER) && !defined(NO_WEB_OUTPUT)
    //     WypWebServer.TriggerEvent(msg.c_str(), WypServerEvents::debug);
    // #endif

    if (_flags & flag)
    {
        va_list arg;
        va_start(arg, format);
        char *temp = new char[MAX_PRINTF_LEN];
        if (!temp)
        {
            return 0;
        }
        char *buffer = temp;
        size_t len = vsnprintf(temp, MAX_PRINTF_LEN, format, arg);
        va_end(arg);

        if (len > (MAX_PRINTF_LEN - 1))
        {
            buffer = new char[len + 1];
            if (!buffer)
            {
                delete[] temp;
                return 0;
            }
            va_start(arg, format);
            vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }

        _Output(flag, buffer, endline);

        if (buffer != temp)
        {
            delete[] buffer;
        }
        delete[] temp;
        return len;
    }

    return 0;
}

void Debug_c::_Output(debug_flags_t flag, const char *str, const char *endline)
{
    const char *prefix = DebugFlagsToString(flag);
    if (prefix)
    {
        Serial.printf("[%s] ", prefix);        
    }
    Serial.print(str);
    Serial.print(endline);
}

const char *DebugFlagsToString(debug_flags_t key)
{
    switch (key)
    {
#define ITEM(X, Y) \
    case X:        \
        return #X;
#include "DebugKeyList"
    }

    return NULL;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DEBUG)

Debug_c Debug;

#endif
#endif
} // namespace wyp