#include "CommandManager.hpp"
#include <list>
#include "Preferences.hpp"
#include "Debug.hpp"
#include "Output.hpp"
#include "Time.hpp"

#define OUTPUT_HIGH "true"
#define OUTPUT_LOW "false"

namespace wyp
{

void CommandManager_c::wsclts(const commands_keys_t &key)
{
    if (wypManager)
    {
        WypWebServer_c *server = wypManager->GetWebServer();
        if (server)
        {
            WypWebSocket_c *socket = server->GetWebSocket();
            if (socket)
            {
                WebSocketClientManager_c *manager = socket->GetClientManager();
                if (manager && response)
                {
                    clients_map_t *clients = manager->GetMap();
                    debugln(CLIENTMGR, "imma ok");
                    clients_map_t::iterator it;
                    debugln(CLIENTMGR, "array");
                    JsonArray &array = response->Arguments().createNestedArray("clients");
                    debugln(CLIENTMGR, "array done");
                    for (it = clients->begin(); it != clients->end(); ++it)
                    {
                        array.add(it->second->GetClient()->remoteIP().toString());
                    }
                }
            }
        }
    }
}

void CommandManager_c::restart(const commands_keys_t &key)
{
    ESP.restart();
}

void CommandManager_c::heap(const commands_keys_t &key)
{
    if (response)
    {
        response->Arguments()[CommandsKeyToString(key)] = ESP.getFreeHeap();
    }
}

void CommandManager_c::relay(const commands_keys_t &key)
{
    if (wypManager)
    {
        if (received)
        {
            // wypManager->GetRelay().Lock(true);
            const char *payload = received->Arguments()[CommandsKeyToString(key)];
            if (strcmp(payload, OUTPUT_HIGH) == 0)
            {
                wypManager->GetRelay()->setHigh();
            }
            else if (strcmp(payload, OUTPUT_LOW) == 0)
            {
                wypManager->GetRelay()->setLow();
            }
            // wypManager->GetRelay().Lock(false);
        }
        response->Arguments()[CommandsKeyToString(key)] = wypManager->GetRelay()->IsHigh();
    }
    else
    {
        debugln(COMMANDMGR, "wyp manager is null");
    }
}

void CommandManager_c::meshclts(const commands_keys_t &key)
{
    if (wypManager && response)
    {
        painlessMesh *mesh = wypManager->GetMesh()->GetMesh();
        if (mesh)
        {
            std::list<uint32_t> list = mesh->getNodeList();
            list.push_back(wypManager->Id());
            JsonArray &array = response->Arguments().createNestedArray(CommandsKeyToString(key));
            debugln(CLIENTMGR, "nodes: %i", list.size());
            for (uint32_t id : list)
            {
                array.add(id);
                debug(CLIENTMGR, "mesh node @%u", id);
            }
        }
    }
}

void CommandManager_c::time(const commands_keys_t &key)
{
    if (response)
    {
        response->Arguments()[CommandsKeyToString(key)] = Time.GetFormattedTime();
    }
}

void CommandManager_c::uptime(const commands_keys_t &key)
{
    if (response)
    {
        response->Arguments()[CommandsKeyToString(key)] = millis();
    }
}

void CommandManager_c::timezone(const commands_keys_t &key)
{
    if (response)
    {
        if (received)
        {
            if (strcmp(received->Arguments()[CommandsKeyToString(key)].as<const char *>(), ""))
            {
                Time.SetTimezone(received->Arguments()[CommandsKeyToString(key)].as<int>());
            }
        }
        response->Arguments()[CommandsKeyToString(key)] = Time.GetTimezone();
    }
}

void CommandManager_c::epochtime(const commands_keys_t &key)
{
    if (response)
    {
        bool internalMessage = received;

        WypMesh_c *mesh;
        MeshClientManager_c *manager;
        const char *password;

        if (internalMessage)
            internalMessage = wypManager;

        if (internalMessage)
            internalMessage = wypManager->GetWiFi();

        if (internalMessage)
            internalMessage = Time.GetNodeTime();

        if (internalMessage)
        {
            mesh = wypManager->GetMesh();
            internalMessage = mesh;
        }

        if (internalMessage)
        {
            manager = mesh->GetMeshClientManager();
            internalMessage = manager;
        }

        if (internalMessage)
        {
            uint32_t id = received->From();
            internalMessage = manager->Exists(id);
        }

        if (internalMessage)
            internalMessage = Preferences.Get(preferences_key_t::mesh_password, &password) && password;

        if (internalMessage)
            internalMessage = received->IsChecksumCorrect(password, received->PassSalt(), salt);

        // debugln(CLIENTMGR, "response");
        // if (received)
        // {
        //     debugln(CLIENTMGR, "received");
        //     if (wypManager)
        //     {
        //         debugln(CLIENTMGR, "manager");
        //         WypMesh_c *mesh = wypManager->GetMesh();
        //         if (mesh)
        //         {
        //             MeshClientManager_c *manager = mesh->GetMeshClientManager();
        //             if (manager)
        //             {
        //                 debugln(CLIENTMGR, "mmanger 1");
        //                 uint32_t id = received->From();
        //                 if (manager->Exists(id))
        //                 {
        //                     debugln(CLIENTMGR, "exist");
        //                     MeshClient_c *clt = manager->Get(id);
        //                     if (clt)
        //                     {
        //                         debugln(CLIENTMGR, "clt");
        //                         const char *password = NULL;
        //                         int pass_salt = 0;
        //                         Preferences.Get(preferences_key_t::mesh_password, &password);

        //                         if (password && received->IsChecksumCorrect(password, received->PassSalt(), salt))
        //                         {
        //                             internalMessage = true;
        //                         }
        //                     }
        //                     else
        //                     {
        //                         debugln(CLIENTMGR, "clt null");
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }

        if (internalMessage)
        {
            debugln(COMMANDMGR, "internal true");
            WypWiFi_c *wifi = wypManager->GetWiFi();
            if (wifi)
            {
                debugln(COMMANDMGR, "wifi true");
                if (wifi->Mode() & wyp_wifi_modes_t::node)
                {
                    debugln(COMMANDMGR, "mode true");
                    NodeTime_c *time = Time.GetNodeTime();
                    if (time)
                    {
                        debugln(COMMANDMGR, "time true");
                        time->SetRawEpochTime(received->Arguments()[CommandsKeyToString(key)]);
                    }
                }
            }
        }

        if (received && strcmp(received->Arguments()[CommandsKeyToString(key)].as<const char *>(), "") == 0)
        {
            debugln(COMMANDMGR, "sending an epoch time response");
            response->Arguments()[CommandsKeyToString(key)] = Time.GetRawEpochTime();
        }
        else if (!internalMessage)
        {
            response->Arguments()[CommandsKeyToString(key)] = "";
        }
    }
}

void CommandManager_c::masternode(const commands_keys_t &key)
{
    bool keepGoing = wypManager;

    WypMesh_c *mesh = NULL;
    WypWiFi_c *wifi;

    MeshClientManager_c *manager = NULL;
    const char *password = NULL;
    int pass_salt;
    uint32_t id = 0;

    if (keepGoing)
    {
        mesh = wypManager->GetMesh();
        keepGoing = mesh;
    }

    if (keepGoing)
    {
        wifi = wypManager->GetWiFi();
        keepGoing = wifi;
    }

    if (keepGoing)
    {
        manager = mesh->GetMeshClientManager();
        keepGoing = manager;
    }

    if (keepGoing && received)
        id = received->From();

    if (keepGoing)
        keepGoing = Preferences.Get(preferences_key_t::mesh_password, &password);

    if (keepGoing)
        keepGoing = Preferences.Get(preferences_key_t::mesh_password_salt, &pass_salt);

    if (keepGoing && received)
        keepGoing = received->IsChecksumCorrect(password, received->PassSalt(), salt);

    if (keepGoing)
    {
        if (wifi->Mode() == wyp_wifi_modes_t::node && received)
        {
            mesh->SetMasterId(received->Arguments()[CommandsKeyToString(key)]);
        }
        else if (wifi->Mode() == wyp_wifi_modes_t::bridge)
        {
            if (response)
            {
                response->Arguments()[CommandsKeyToString(key)] = mesh->GetMesh()->getNodeId();
            }
        }
    }
    else
    {
        debugln(COMMANDMGR, "master node command failed");
    }
}
} // namespace wyp