#include "WypManager.hpp"
#include <ESP8266mDNS.h>
#include "WebSocketClientManager.hpp"
#include "CommandManager.hpp"
#include "Debug.hpp"
#include "Preferences.hpp"

#define PIN_RELAY 2
#define PIN_BUTTON 0

#define PRESSED_TIME_RESET_WIFI 5000 //ms

namespace wyp
{

WypManager_c::WypManager_c()
    : _relay(PIN_RELAY)

{
    _webserver = new WypWebServer_c(this);
    _mesh = new WypMesh_c(this);
    _button = new Button_c(PIN_BUTTON, std::bind(&WypManager_c::_OnButton, this, std::placeholders::_1));
    _wifiManager = new WypWiFi_c();

    _relay.SetCallback(std::bind(&WypManager_c::_OnRelayChange, this));
}

WypManager_c::~WypManager_c()
{
    if (_webserver)
        delete _webserver;
    if (_button)
        delete _button;
}

void WypManager_c::Begin()
{
    if (_wifiManager)
    {
        _wifiManager->Begin(_mesh);
    }

    //webServer
    if (_webserver)
    {
        _webserver->begin();
        MDNS.addService("http", "tcp", 80);
    }
} // namespace wyp

void WypManager_c::Update()
{
    if (_mesh)
    {
        _mesh->Update();
    }

    if (_wifiManager)
    {
        _wifiManager->Update();
    }

    if (_button)
    {
        _button->Update();
    }
}

void WypManager_c::_OnButton(const bool status)
{
    if (status)
    {
        _pressedTime = millis();
    }
    else
    {
        if (millis() - _pressedTime >= PRESSED_TIME_RESET_WIFI)
        {
            debug(WIFI, "Button has been pressed to go in setup mode");
            if (_wifiManager)
            {
                _wifiManager->SetMode(wyp_wifi_modes_t::setup);
            }
        }
        else
        {
            _relay.DigitaWrite(!_relay.IsHigh());
        }
    }
}

void WypManager_c::_OnRelayChange()
{
    if (_webserver)
    {
        WypWebSocket_c *ws = _webserver->GetWebSocket();
        if (ws)
        {
            WebSocketClientManager_c *manager = ws->GetClientManager();
            if (manager)
            {
                int pass_salt;
                if (Preferences.Get(preferences_key_t::ws_password_salt, &pass_salt))
                {

                    CommandManager_c cmd(this);
                    JsonResponse_s *response = cmd.Handle(commands_keys_t::relay, pass_salt, DESTINATION_WS_CLIENTS);
                    if (response)
                    {
                        for (auto p : *manager)
                        {
                            p.second->Send(response);
                        }
                    }
                }
            }
        }
    }
}

uint32_t WypManager_c::Id()
{
    uint32_t ret = 0;
    if (_mesh)
    {
        ret = _mesh->GetMesh()->getNodeId();
    }
    return ret;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYPMANAGER)
WypManager_c WypManager;
#endif

} // namespace wyp