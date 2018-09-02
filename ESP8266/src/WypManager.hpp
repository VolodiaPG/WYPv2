#ifndef WYPMANAGER_H
#define WYPMANAGER_H

#include "WypMesh.hpp"
#include "WypWebServer.hpp"
#include "Output.hpp"
#include "Button.hpp"
#include "WypWiFi.hpp"

namespace wyp
{

class WypWebServer_c;
class WypWiFi_c;

class WypManager_c
{
  //methods
public:
  WypManager_c();
  ~WypManager_c();

  void Begin();

  void Update();

  uint32_t Id();

  Output_c *GetRelay() { return &_relay; }
  WypMesh_c *GetMesh() { return _mesh; }
  WypWiFi_c *GetWiFi() { return _wifiManager; }
  WypWebServer_c *GetWebServer() { return _webserver; }

private:
  void _OnButton(const bool status);
  void _OnRelayChange();

  //variables
private:
  WypWebServer_c *_webserver = NULL;
  Output_c _relay;
  Button_c *_button = NULL;
  WypWiFi_c *_wifiManager = NULL;
  WypMesh_c *_mesh = NULL;

  //button
  bool _switch = false;
  long _pressedTime;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYPMANAGER)
extern WypManager_c WypManager;
#endif

} // namespace wyp

#endif