#include <ArduinoOTA.h>
#include "Debug.hpp"

#include "WypOTA.hpp"

namespace wyp
{

WypOTA_c::WypOTA_c(String password, String hostname, int port)
{
    SetPassword(password);
    SetHostname(hostname);
    SetPort(port);
}

WypOTA_c::WypOTA_c()
    : WypOTA_c("", "", 8266)
{
}

void WypOTA_c::Begin()
{
    _Init();
    ArduinoOTA.begin();
}

void WypOTA_c::Handle()
{
    ArduinoOTA.handle();
}

//Getters & Setters
void WypOTA_c::SetHostname(String hostname)
{
    _hostname = hostname;
}

String WypOTA_c::GetHostname()
{
    return _hostname;
}

void WypOTA_c::SetPassword(String password)
{
    _password = password;
}

String WypOTA_c::GetPassword()
{
    return _password;
}

void WypOTA_c::SetPort(int port)
{
    _port = port;
}

int WypOTA_c::GetPort()
{
    return _port;
}

void WypOTA_c::SetLed(Led_c *led)
{
    _led = led;
}

//privates

void WypOTA_c::_Init()
{
    ArduinoOTA.onStart(std::bind(&WypOTA_c::_OnStart, this));

    ArduinoOTA.onEnd(std::bind(&WypOTA_c::_OnEnd, this));

    ArduinoOTA.onProgress(std::bind(&WypOTA_c::_OnProgress, this, std::placeholders::_1, std::placeholders::_2));

    ArduinoOTA.onError(std::bind(&WypOTA_c::_OnError, this, std::placeholders::_1));
}

void WypOTA_c::_OnStart()
{
    if (_led != NULL)
    {
        _led->SetLow();
    }
   debugln(debug_flags_t::OTA, "Start");
}

void WypOTA_c::_OnEnd()
{
    if (_led != NULL)
    {
        _led->FadeOff();
    }

    debugln(debug_flags_t::OTA, _ProgressIndicator(100).c_str());
    debugln(debug_flags_t::OTA, "End");
}

void WypOTA_c::_OnProgress(unsigned int progress, unsigned int total)
{
    int percent = progress / (total / 100);

    if (_led != NULL)
    {
        int mapped = map(percent, 0, 100, 0, _led->MAX);
        _led->Write(mapped);
    }

    debug(debug_flags_t::OTA, "%s\r", _ProgressIndicator(percent).c_str());
}

void WypOTA_c::_OnError(ota_error_t error)
{
    if (_led != NULL)
    {
        _led->FadeOff();
    }
   debugln(debug_flags_t::OTA,"Error: %s", error);

    if (error == OTA_AUTH_ERROR)
       debugln(debug_flags_t::OTA,"Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
       debugln(debug_flags_t::OTA,"Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
       debugln(debug_flags_t::OTA,"Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
       debugln(debug_flags_t::OTA,"Receive Failed");
    else if (error == OTA_END_ERROR)
       debugln(debug_flags_t::OTA,"End Failed");
}

String WypOTA_c::_ProgressIndicator(int percent)
{
    uint8_t size = 10;
    int barMap = map(percent, 0, 100, 0, size);

    String progressBar = "[";

    for (uint8_t ii = 0; ii < size; ++ii)
    {
        progressBar += ii <= barMap ? "=" : " ";
    }

    progressBar += "]";

    return "Receiving: " + progressBar + " " + String(percent) + "%";
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYP_OTA)
WypOTA_c WypOTA;
#endif
}