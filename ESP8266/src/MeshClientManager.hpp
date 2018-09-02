#ifndef MESHCLIENTMANAGER_H
#define MESHCLIENTMANAGER_H

#include "WypMesh.hpp"
#include <map>
#include "MeshClient.hpp"

namespace wyp
{

class MeshClient_c;
class WypMesh_c;

typedef std::map<uint32_t, MeshClient_c *> mesh_clients_map_t;

class MeshClientManager_c
{
  //methods
public:
  MeshClientManager_c(WypMesh_c *mesh);
  ~MeshClientManager_c();

  bool Exists(uint32_t &nodeId);
  MeshClient_c *Get(uint32_t &nodeId);
  void Add(uint32_t nodeId);
  void Erase(uint32_t &nodeId);
  mesh_clients_map_t *GetMap() { return &clients; }

  WypMesh_c *GetMesh() { return _mesh; }

  //iterator
  using pair = std::pair<uint32_t, MeshClient_c*>;
  using iterator = mesh_clients_map_t::iterator;
  iterator begin() { return clients.begin(); }
  iterator end() { return clients.end(); }

  //variables
private:
  mesh_clients_map_t clients;
  WypMesh_c *_mesh = NULL;
};

} // namespace wyp

#endif