#ifndef WYPWEBSERVER_H
#define WYPWEBSERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "WypWebSocket.hpp"
#include <painlessMesh.h>
#include "WypManager.hpp"

namespace wyp
{

class WypWebSocket_c;
class WypManager_c;

enum WypServerEvents
{
  debug,
};

class WypWebServer_c : public AsyncWebServer
{
  //methods

public:
  //in WypWebServer.cpp
  WypWebServer_c(WypManager_c *manager);
  ~WypWebServer_c();
  void begin();
  bool HasBegan();
  WypWebSocket_c *GetWebSocket() { return _websocket; }

private:
  //in WypWebServer.cpp
  void _Init();
  void _OnTest(AsyncWebServerRequest *request);
  void _OnNotFound(AsyncWebServerRequest *request);
  void _OnSetup(AsyncWebServerRequest *request);

  //variables
private:
  //in WypWebServer.cpp
  static const int _port = 80;
  bool _hasBegan = false;

  WypWebSocket_c *_websocket = NULL;
  WypManager_c *_wypManager = NULL;
};

} // namespace wyp
#endif