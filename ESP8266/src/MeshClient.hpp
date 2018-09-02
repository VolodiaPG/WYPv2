#ifndef MESHCLIENT_H
#define MESHCLIENT_H

#include "JsonMessages.hpp"
#include "MeshClientManager.hpp"
#include "WypMesh.hpp"

namespace wyp
{

class MeshClientManager_c;

class MeshClient_c
{
  //methods
public:
  MeshClient_c(MeshClientManager_c *parent, uint32_t nodeId);
  bool Send(JsonResponse_s *emited, const char *password, const int pass_salt, const int salt);
  bool Send(JsonReceived_s *emited);
  MeshClientManager_c *Parent() const { return _parent; }
  int Salt(int salt_received);
  int Salt(bool newSalt = false);
  const uint32_t GetNodeId() const { return _nodeId; }

protected:
  bool _Send(JsonMessage_s *emited);

  //variables
private:
  MeshClientManager_c *_parent;
  int _salt;
  uint32_t _nodeId;
};

} // namespace wyp
#endif