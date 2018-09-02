#include "WebSocketClientManager.hpp"

#include <map>
#include <AsyncWebSocket.h>
#include "WebSocketClient.hpp"
#include "Debug.hpp"

namespace wyp
{

WebSocketClientManager_c::WebSocketClientManager_c()
{
}

WebSocketClientManager_c::~WebSocketClientManager_c()
{
    for (auto pair : clients)
    {
        if (pair.second)
        {
            Erase((uint32_t)pair.first);
        }
    }
}

bool WebSocketClientManager_c::Exists(uint32_t id)
{
    debugln(CLIENTMGR, "Exists?");
    return clients.count(id) != 0;
}

WebSocketClient_c *WebSocketClientManager_c::Get(uint32_t id)
{
    debugln(CLIENTMGR, "Get");
    WebSocketClient_c *ret = NULL;
    if (Exists(id))
    {
        debugln(CLIENTMGR, "Client detected");
        ret = clients[id];
    }
    else
    {
        debugln(CLIENTMGR, "Client not in database");
    }

    return ret;
}

clients_map_t *WebSocketClientManager_c::GetMap()
{
    return &clients;
}

void WebSocketClientManager_c::Add(AsyncWebSocketClient *client, WebSocketClientManager_c *manager)
{
    debugln(CLIENTMGR, "Add");
    if (client)
    {
        std::pair<uint32_t, WebSocketClient_c *> pair = std::make_pair(client->id(), new WebSocketClient_c(client, manager));
        clients.insert(pair);
    }
}

void WebSocketClientManager_c::Erase(uint32_t id)
{
    debugln(CLIENTMGR, "Erase");
    if (Exists(id) && clients[id])
    {
        free(clients[id]);
    }
    clients.erase(id);
}

} // namespace wyp