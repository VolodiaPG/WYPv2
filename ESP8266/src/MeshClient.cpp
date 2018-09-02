#include "MeshClient.hpp"
#include "Preferences.hpp"
#include "Salt.h"

namespace wyp
{

MeshClient_c::MeshClient_c(MeshClientManager_c *parent, uint32_t nodeId)
    : _parent(parent),
      _nodeId(nodeId)
{
}

bool MeshClient_c::Send(JsonResponse_s *emited, const char *password, const int pass_salt, const int salt)
{
    bool ret = false;

    if (emited)
    {
        // if (emited->To() == DESTINATION_WS_CLIENTS)
        // {
        //     emited->PrepareToSend(password, pass_salt, Salt(true));
        // }
        // else
        // {
        //     emited->PrepareToSend(password, pass_salt, Salt());
        // }
        emited->PrepareToSend(password, pass_salt, salt);
        ret = _Send(emited);
    }

    return ret;
}

bool MeshClient_c::Send(JsonReceived_s *emited)
{
    bool ret = false;

    if (emited)
    {
        ret = _Send(emited);
    }

    return ret;
}

bool MeshClient_c::_Send(JsonMessage_s *emited)
{
    bool ret = false;
    if (emited && _parent && _parent->GetMesh())
    {
        String msg = "";
        emited->Root().printTo(msg);
        ret = _parent->GetMesh()->GetMesh()->sendSingle(_nodeId, msg);
    }

    return ret;
}

int MeshClient_c::Salt(int salt_received)
{
    _salt = salt_received;

    return _salt;
}

int MeshClient_c::Salt(bool newSalt)
{
    if (newSalt)
    {
        _salt = trng();
    }
    return _salt;
}

} // namespace wyp