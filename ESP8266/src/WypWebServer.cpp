#include "WypWebServer.hpp"
#include <ESPAsyncWebServer.h>
#include "Debug.hpp"
#include "WypWebSocket.hpp"
#include "Preferences.hpp"
#include "Salt.h"
#include "Debug.hpp"
#include "WypWiFi.hpp"

namespace wyp
{

#define PUBLIC_PATH_EXTERNAL "/www/external/"
#define PUBLIC_PATH_INTERNAL "/www/internal/"
#define ROOT_URI "/"
#define TEST_URI "/test"
#define SETUP_URI "/setup"

//particular
#define PARAM_WIFI_SSID "wifi_ssid"
#define PARAM_WIFI_PASS "wifi_pass"
#define PARAM_MESH_SSID "mesh_ssid"
#define PARAM_MESH_PASS "mesh_pass"

//common to all
#define PARAM_CHANNEL "channel"
#define PARAM_WEBSOCKET_PASS "ws_pass"
#define PARAM_MODE "mode"

//param values
#define PARAM_MODE_NODE "node"
#define PARAM_MODE_BRIDGE "bridge"

WypWebServer_c::WypWebServer_c(WypManager_c *manager)
    : AsyncWebServer(_port)
{
    _wypManager = manager;

    _websocket = new WypWebSocket_c("/ws", _wypManager);

    this->addHandler(_websocket);

    _Init();
}

WypWebServer_c::~WypWebServer_c()
{
    if (_websocket)
    {
        delete _websocket;
    }
}

void WypWebServer_c::_Init()
{
    serveStatic(ROOT_URI, SPIFFS, PUBLIC_PATH_EXTERNAL).setDefaultFile("index.html").setFilter(ON_STA_FILTER);
    serveStatic(ROOT_URI, SPIFFS, PUBLIC_PATH_INTERNAL).setDefaultFile("index.html").setFilter(ON_AP_FILTER);

    on(TEST_URI, HTTP_GET, std::bind(&WypWebServer_c::_OnTest, this, std::placeholders::_1));
    on(SETUP_URI, HTTP_POST, std::bind(&WypWebServer_c::_OnSetup, this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
    onNotFound(std::bind(&WypWebServer_c::_OnNotFound, this, std::placeholders::_1));
}

void WypWebServer_c::_OnTest(AsyncWebServerRequest *request)
{
    debugln(debug_flags_t::SERVER, "test");
    request->send(200, "text/plain", "Test");
}

void WypWebServer_c::_OnNotFound(AsyncWebServerRequest *request)
{
    debugln(debug_flags_t::SERVER, "Not found: http://%s%s", request->host().c_str(), request->url().c_str());
    request->send(404, "text/plain", "Not found");
}

void WypWebServer_c::_OnSetup(AsyncWebServerRequest *request)
{
    bool keepGoing = true;
    bool success = false;

    debugln(SERVER, "Start post");

    keepGoing = _wypManager;

    if (keepGoing)
        keepGoing = _wypManager->GetWiFi();

    if (keepGoing)
        keepGoing = request->hasParam(PARAM_CHANNEL, true);
    if (keepGoing)
        keepGoing = WypWiFi_c::IsChannelConform(request->getParam(PARAM_CHANNEL, true)->value().toInt());

    if (keepGoing)
        keepGoing = request->hasParam(PARAM_MODE, true);

    if (keepGoing)
        keepGoing = request->hasParam(PARAM_WEBSOCKET_PASS, true);
    if (keepGoing)
        keepGoing = WypWiFi_c::IsPasswordConform(request->getParam(PARAM_WEBSOCKET_PASS, true)->value().c_str());

    if (keepGoing)
    {
        bool bridge = request->getParam(PARAM_MODE, true)->value().equals(PARAM_MODE_BRIDGE);
        if (bridge)
        {
            if (keepGoing)
                keepGoing = request->hasParam(PARAM_WIFI_SSID, true);
            if (keepGoing)
                keepGoing = WypWiFi_c::IsSsidConform(request->getParam(PARAM_WIFI_SSID, true)->value().c_str());

            if (keepGoing)
                keepGoing = request->hasParam(PARAM_WIFI_PASS, true);
            if (keepGoing)
                keepGoing = WypWiFi_c::IsPasswordConform(request->getParam(PARAM_WIFI_PASS, true)->value().c_str());

            if (keepGoing)
            {
                _wypManager->GetWiFi()->Wifi_Ssid(request->getParam(PARAM_WIFI_SSID, true)->value().c_str());
                _wypManager->GetWiFi()->Wifi_Password(request->getParam(PARAM_WIFI_PASS, true)->value().c_str());
            }
            //can't have succes yet
        }

        if (request->getParam(PARAM_MODE, true)->value().equals(PARAM_MODE_NODE) || bridge)
        {
            if (keepGoing)
                keepGoing = request->hasParam(PARAM_MESH_SSID, true);
            if (keepGoing)
                keepGoing = WypWiFi_c::IsSsidConform(request->getParam(PARAM_MESH_SSID, true)->value().c_str());
            
            if (keepGoing)
                keepGoing = request->hasParam(PARAM_MESH_PASS, true);
            if (keepGoing)
                keepGoing = WypWiFi_c::IsSsidConform(request->getParam(PARAM_MESH_PASS, true)->value().c_str());

            if (keepGoing)
            {
                _wypManager->GetWiFi()->Mesh_Ssid(request->getParam(PARAM_MESH_SSID, true)->value().c_str());
                _wypManager->GetWiFi()->Mesh_Password(request->getParam(PARAM_MESH_PASS, true)->value().c_str());
                if (!bridge)
                {
                    _wypManager->GetWiFi()->SetMode(wyp_wifi_modes_t::node);
                }
            }

            success = true;
        }
        if (bridge && success)
        {
            _wypManager->GetWiFi()->SetMode(wyp_wifi_modes_t::bridge);
        }
    }

    if (success)
    {
        //weird bug if c_str() is used
        Preferences.Set(preferences_key_t::ws_password, request->getParam(PARAM_WEBSOCKET_PASS, true)->value());
        Preferences.Set(preferences_key_t::ws_password_salt, trng());
        Preferences.Set(preferences_key_t::mesh_password_salt, trng());
        Preferences.Set(preferences_key_t::channel, request->getParam(PARAM_CHANNEL, true)->value().toInt());
        Preferences.Save();

        debugln(SERVER, "the post request to configure the module has been registrated");

        request->redirect(ROOT_URI);
    }
    else
    {
        debugln(SERVER, "the post request to configure the module has been rejected");
    }
}

void WypWebServer_c::begin()
{
    AsyncWebServer::begin();
    _hasBegan = true;
}

bool WypWebServer_c::HasBegan()
{
    return _hasBegan;
}
} // namespace wyp