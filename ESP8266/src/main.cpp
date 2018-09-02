// error squiggles deactivated

#include <Arduino.h>
#include <Hash.h> //fix for platformio and the Websocket async
#include <painlessMesh.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <map>

// #define NO_GLOBAL_PREFERENCES

#include "Led.hpp"
#include "Debug.hpp"
#include "WypOTA.hpp"
#include "WypManager.hpp" 
#include "Preferences.hpp"
#include "Time.hpp"

using namespace wyp;

Led_c EmbeddedLed = Led_c(2);

const char *ssid = "Livebox-1EC6";
const char *wifi_password = "2Xi6Xrf7chNZPV9Jxe";
// const char *ssid = "GIT_GUD";
// const char *wifi_password = "=Volodia333";

const String ota_password = "123";
const String ota_hostname = "ESP8266 OTA - " + ESP.getChipId();
const int otaPort = 8266;

bool doOnce = false;

void setup()
{
    delay(5000);

#ifdef WYP_DEBUG
    Debug.SetDebugFlags(debug_flags_t::DEBUG | debug_flags_t::OTA | WIFI | SERVER | VAR |
                        SETUP | WEBSOCKET | WSMESSAGE | FILESYSTEM | CLIENTMGR |
                        COMMANDMGR | MESH | TIME);

    Serial.begin(115200);
    Serial.println("\r\n");
#endif

    // Mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages

    debugln(debug_flags_t::SETUP, "Starting Sketchy sketch");

    EmbeddedLed.SetRefreshRate(16);
    EmbeddedLed.SetIncrement(1);
    EmbeddedLed.FadeContinuously();

    //spiffs
    SPIFFS.begin();
    debugln(SETUP, "Began SPIFFS");
    Preferences.Begin();
    debugln(SETUP, "Began Preferences");

    WypManager.Begin();
    debugln(SETUP, "Began WypManager");
    
    Time.Begin();
    debugln(SETUP, "Began Time");

    WypOTA.SetHostname(ota_hostname);
    WypOTA.SetPassword(ota_password);
    WypOTA.SetPort(otaPort);
    WypOTA.SetLed(&EmbeddedLed);
    WypOTA.Begin();
    debugln(debug_flags_t::OTA, "Ready");

    debugln(debug_flags_t::SETUP, "Done.");
    EmbeddedLed.FadeOff();
}

void loop()
{
    WypManager.Update();
    WypOTA.Handle();
    Time.Update();
}
