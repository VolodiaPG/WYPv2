#include "MeshClientManager.hpp"
namespace wyp
{

MeshClientManager_c::MeshClientManager_c(WypMesh_c *mesh)
    : _mesh(mesh)
{
}

MeshClientManager_c::~MeshClientManager_c()
{
    for (auto pair : clients)
    {
        if (pair.second)
        {
            uint32_t id = pair.first;
            Erase(id);
        }
    }
}

bool MeshClientManager_c::Exists(uint32_t &nodeId)
{
    return clients.count(nodeId) != 0;
}

MeshClient_c *MeshClientManager_c::Get(uint32_t &nodeId)
{
    MeshClient_c *ret = NULL;
    if (Exists(nodeId))
    {
        ret = clients[nodeId];
    }

    return ret;
}

void MeshClientManager_c::Add(uint32_t nodeId)
{
    std::pair<uint32_t, MeshClient_c *> pair = std::make_pair(nodeId, new MeshClient_c(this, nodeId));
    clients.insert(pair);
}

void MeshClientManager_c::Erase(uint32_t &nodeId)
{
    if (Exists(nodeId) && clients[nodeId])
    {
        free(clients[nodeId]);
    }
    clients.erase(nodeId);
}

} // namespace wyp