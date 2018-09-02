#include "WypWiFi.hpp"
#include "ESP8266WiFi.h"
#include "Debug.hpp"
#include "Preferences.hpp"
#include <painlessMesh.h>

#define AP_PASS "espressif8266"
static IPAddress apIP(42, 42, 42, 42);

namespace wyp
{

WypWiFi_c::WypWiFi_c()
{
}

void WypWiFi_c::Begin(WypMesh_c *mesh)
{
    Preferences.Get(preferences_key_t::mode, &_mode);
    _mesh = mesh;
}

void WypWiFi_c::Update()
{
    if (_setupDone)
    {
        if (Mode() & bridge & node)
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                debugln(debug_flags_t::WIFI, "disconnection");
                if (!_doOnce)
                {
                    debugln(debug_flags_t::WIFI, "WiFi Disconnected");
                    // EmbeddedLed.FadeContinuously();
                    _doOnce = true;
                }
            }
            else
            {
                if (_doOnce)
                {
                    debugln(debug_flags_t::WIFI, "Ip Address: %s", WiFi.localIP().toString().c_str());
                    debugln(debug_flags_t::WIFI, "Hostname: %s", WiFi.hostname().c_str());
                    // EmbeddedLed.FadeOff();

                    _doOnce = false;
                }
            }
        }
    }
    else
    {
        _Setup();
    }
}

void WypWiFi_c::_Setup()
{
    WiFi.setAutoReconnect(false);

    bool correctSetup = false;

    switch (Mode())
    {
    case bridge:
        correctSetup = _SetupBridge();
        break;
    case node:
        correctSetup = _SetupNode();
        break;
    case setup:
        break;
    }

    if (!correctSetup)
    {
        _SetupAp();
    }

    _setupDone = true;
    _currentMode = Mode();
}

void WypWiFi_c::_SetupAp()
{
    debugln(WIFI, "setuping AP");
    if (_mesh)
    {
        _mesh->Stop();
    }

    WiFi.mode(WiFiMode::WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    String label = String("WYP-" + String(ESP.getChipId()));
    WiFi.softAP(label.c_str(), AP_PASS);
    debugln(WIFI, "Setup AP '%s'", label.c_str());
}

bool WypWiFi_c::_SetupBridge()
{
    debugln(WIFI, "setuping Bridge");
    bool ret = IsChannelConform(Channel());

    if (ret)
        ret = IsSsidConform(Wifi_Ssid());
    if (ret)
        ret = IsPasswordConform(Wifi_Password());

    if (ret)
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(Wifi_Ssid(), Wifi_Password(), false);
        WiFi.setAutoReconnect(true);
        ret = _mesh->BeginBridge(Channel(), Mesh_Ssid(), Mesh_Password(), Wifi_Ssid(), Wifi_Password());

        debugln(WIFI, "Setup Bridge");
    }

    return ret;
}

bool WypWiFi_c::_SetupNode()
{
    debugln(WIFI, "setuping Node");

    bool ret = _mesh;

    if (ret)
    {
        WiFi.mode(WIFI_AP_STA);
        ret = _mesh->BeginNode(Channel(), Mesh_Ssid(), Mesh_Password());
        debugln(WIFI, "Setup Node");
    }

    return ret;
}

//wifi
const char *WypWiFi_c::Wifi_Password() const
{
    const char *ret = NULL;
    Preferences.Get(preferences_key_t::wifi_password, &ret);
    return ret;
}

void WypWiFi_c::Wifi_Password(const char *password)
{
    if (password)
    {
        Preferences.Set(preferences_key_t::wifi_password, password);
        Preferences.Save();
    }
}

const char *WypWiFi_c::Wifi_Ssid() const
{
    const char *ret = NULL;
    Preferences.Get(preferences_key_t::wifi_ssid, &ret);
    return ret;
}

void WypWiFi_c::Wifi_Ssid(const char *ssid)
{
    if (ssid)
    {
        Preferences.Set(preferences_key_t::wifi_ssid, ssid);
        Preferences.Save();
    }
}

//mesh
const char *WypWiFi_c::Mesh_Password() const
{
    const char *ret = NULL;
    Preferences.Get(preferences_key_t::mesh_password, &ret);
    return ret;
}

void WypWiFi_c::Mesh_Password(const char *password)
{
    if (password)
    {
        Preferences.Set(preferences_key_t::mesh_password, password);
        Preferences.Save();
    }
}

const char *WypWiFi_c::Mesh_Ssid() const
{
    const char *ret = NULL;
    Preferences.Get(preferences_key_t::mesh_ssid, &ret);
    return ret;
}

void WypWiFi_c::Mesh_Ssid(const char *ssid)
{
    if (ssid)
    {
        Preferences.Set(preferences_key_t::mesh_ssid, ssid);
        Preferences.Save();
    }
}

uint8_t WypWiFi_c::Channel() const
{
    uint8_t ret = 0;
    Preferences.Get(preferences_key_t::channel, &ret);
    return ret;
}

void WypWiFi_c::Channel(const uint8_t channel)
{
    if (channel > 0 && channel < 14)
    {
        Preferences.Set(preferences_key_t::channel, channel);
        Preferences.Save();
    }
}

uint32_t WypWiFi_c::Mode()
{
    // uint32_t mode = 0;
    // Preferences.Get<uint32_t>(preferences_key_t::mode, &mode);
    return _mode;
}

void WypWiFi_c::SetMode(wyp_wifi_modes_t mode)
{
    _mode = mode;
    Preferences.Set<uint32_t>(preferences_key_t::mode, (uint32_t)mode);
    Preferences.Save();
    debugln(WIFI, "mode set");
    _setupDone = false;
}

// #if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYPWIFI)
// WypWiFi_c WypWiFi;
// #endif

} // namespace wyp