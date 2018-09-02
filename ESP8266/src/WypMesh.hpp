#ifndef WYPMESH_H
#define WYPMESH_H

#include "painlessMesh.h"
#include "MeshClientManager.hpp"
#include "WypManager.hpp"

namespace wyp
{

class WypManager_c;
class MeshClientManager_c;

class WypMesh_c
{
  //methods
public:
  WypMesh_c(WypManager_c *manager);
  bool BeginBridge(const uint8_t &channel, const char *mesh_ssid, const char *mesh_pass, const char *wifi_ssid, const char *wifi_pass);
  bool BeginNode(const uint8_t &channel, const char *mesh_ssid, const char *mesh_pass);
  void Update();
  void Stop();
  painlessMesh *GetMesh() { return &mesh; };
  MeshClientManager_c *GetMeshClientManager() { return manager; }
  void SetMasterId(uint32_t id) { _masterId = id; }
  uint32_t GetMasterId() { return _masterId; }

private:
  void _OnMessageReceived(uint32_t from, String &msg);
  void _OnNewConnection(uint32_t nodeId);
  void _OnDroppedConnection(uint32_t nodeId);
  void _NotifyWsClients();
  void _NotifyBridge();

  //variables
protected:
  painlessMesh mesh;
  MeshClientManager_c *manager = NULL;
  WypManager_c *wypManager = NULL;
  uint32_t _masterId = 0;
};

} // namespace wyp

#endif