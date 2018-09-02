#include "CommandManager.hpp"
#include <map>
#include <list>
#include <algorithm>
#include "Debug.hpp"
#include "WebSocketClientManager.hpp"
#include "WebSocketClient.hpp"
#include "JsonMessages.hpp"
#include "WypWebSocket.hpp"

namespace wyp
{

CommandManager_c::CommandManager_c(WypManager_c *manager, JsonReceived_s *messageReceived)
    : received(messageReceived),
      wypManager(manager)
{
}

CommandManager_c::CommandManager_c(WypManager_c *manager)
    : CommandManager_c(manager, NULL)
{
}

CommandManager_c::~CommandManager_c()
{
    if (response)
        delete response;
}

JsonResponse_s *CommandManager_c::Handle(const char *password, const int &pass_salt, const int &salt)
{
    if (received)
    {
        this->salt = salt;
        bool keepGoing = true;

        if (keepGoing)
            keepGoing = password;

        if (keepGoing)
        {
            if (received->To() == wypManager->Id())
            {
                response = new JsonResponse_s(wypManager->Id(), received->From());
                if (received->IsConform())
                {
                    String arguments = "";
                    received->Arguments().printTo(arguments);
                    debugln(COMMANDMGR, "Checking message cs: pass: %s, pass_salt, %i, salt: %i, cotent: %s", password, pass_salt, salt, arguments.c_str());
                    if (received->IsChecksumCorrect(password, pass_salt, salt))
                    {
                        debugln(COMMANDMGR, "passed");
                        debugln(COMMANDMGR, "arg length: %i", received->Arguments().measureLength());
                        if (received->Arguments().measureLength() != 0)
                        {
                            _CommandLogic();
                        }
                    }
                    else
                    {
                        debugln(COMMANDMGR, "Incorrect checksum");
                        response->Root()["error"] = "Incorrect Checksum";
                    }
                }
                else
                {
                    debugln(COMMANDMGR, "Not conform");
                    response->Root()["error"] = "Not conform";
                }
            }
            else
            {
                debugln(COMMANDMGR, "Trying to re-route");
                _HandleNotDestination();
            }
        }
    }

    return response;
}

void CommandManager_c::_HandleNotDestination()
{
    if (received && wypManager)
    {
        WypMesh_c *mesh = wypManager->GetMesh();
        if (mesh)
        {
            bool foundDest = false;

            uint32_t to = received->To();
            if (to == DESTINATION_WS_CLIENTS)
            {
                WypWebServer_c *server = wypManager->GetWebServer();
                if (server)
                {
                    WypWebSocket_c *ws = server->GetWebSocket();
                    if (ws)
                    {
                        for (auto pair : *ws->GetClientManager())
                        {
                            pair.second->Send(received);
                        }
                        foundDest = true;
                    }
                }
            }
            else
            {
                MeshClientManager_c *manager = mesh->GetMeshClientManager();
                if (manager)
                {
                    if (manager->Exists(to))
                    {
                        foundDest = manager->Get(to)->Send(received);
                    }
                }
            }

            if (!foundDest)
            {
                debug(COMMANDMGR, "Can't redirect message, the id %u is not present in the mesh", to);
            }
        }
    }
}

JsonResponse_s *CommandManager_c::Handle(commands_keys_t key, const int &pass_salt, const uint32_t &to)
{
    response = new JsonResponse_s(wypManager->Id(), to);
    _HandleCommand(key);

    return response;
}

void CommandManager_c::_CommandLogic()
{
    if (response && received)
    {
        debugln(COMMANDMGR, "Starting command logic");
        JsonObject &arguments = received->Arguments();
        for (const auto &kv : arguments)
        {
            auto it = converter.find(kv.key);
            if (it != converter.end())
            {
                _HandleCommand(it->second);
            }
        }
    }
}

void CommandManager_c::_HandleCommand(commands_keys_t key)
{

#define COMMAND(X, Y)                                                    \
    if (key == Y)                                                        \
    {                                                                    \
        debugln(COMMANDMGR, "Command is %s", CommandsKeyToString(key)); \
        this->X(key);                                                    \
    }
#include "Commands"
}

const char *CommandsKeyToString(commands_keys_t key)
{
    switch (key)
    {
#define COMMAND(X, Y) \
    case X:           \
        return #X;
#include "Commands"
    }
    return NULL;
}
} // namespace wyp