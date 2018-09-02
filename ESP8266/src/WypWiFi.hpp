#ifndef WYPWIFI_H
#define WYPWIFI_H

#include "WypMesh.hpp"

namespace wyp
{

class WypMesh_c;

enum wyp_wifi_modes_t
{
  setup = 1 << 0,
  node = 1 << 1,
  bridge = 1 << 2,
};

class WypWiFi_c
{
  //methods
public:
  WypWiFi_c();

  void Begin(WypMesh_c *mesh);
  void Update();

  const char *Wifi_Password() const;
  void Wifi_Password(const char *password);

  const char *Wifi_Ssid() const;
  void Wifi_Ssid(const char *ssid);

  const char *Mesh_Password() const;
  void Mesh_Password(const char *password);

  const char *Mesh_Ssid() const;
  void Mesh_Ssid(const char *ssid);

  uint8_t Channel() const;
  void Channel(const uint8_t channel);

  uint32_t Mode();
  void SetMode(wyp_wifi_modes_t mode);

  static bool IsChannelConform(const uint8_t &channel) { return channel > 0 && channel < 14; }
  static bool IsSsidConform(const char *ssid) { return ssid && strlen(ssid) < 32; }
  static bool IsPasswordConform(const char *pass) { return pass && strlen(pass) < 32 && strlen(pass) > 8; }

private:
  void _Setup();
  bool _SetupBridge();
  bool _SetupNode();
  void _SetupAp();

  //variables
private:
  const char *_ssid;
  const char *_password;
  bool _doOnce = false;

  uint32_t _currentMode;
  volatile bool _setupDone = false;
  WypMesh_c *_mesh = NULL;

  uint32_t _mode;
};

} // namespace wyp

#endif
