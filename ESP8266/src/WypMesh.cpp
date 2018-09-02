#include "WypMesh.hpp"
#include "WypWiFi.hpp"
#include <functional>
#include "Commandmanager.hpp"
#include "Preferences.hpp"
#include "Debug.hpp"
#include "Salt.h"

namespace wyp
{

#define MESH_PORT 5555

WypMesh_c::WypMesh_c(WypManager_c *wypmgr)
    : wypManager(wypmgr)
{
    manager = new MeshClientManager_c(this);

    mesh.onReceive(std::bind(&WypMesh_c::_OnMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
    mesh.onNewConnection(std::bind(&WypMesh_c::_OnNewConnection, this, placeholders::_1));
    mesh.onDroppedConnection(std::bind(&WypMesh_c::_OnDroppedConnection, this, placeholders::_1));
}

bool WypMesh_c::BeginBridge(const uint8_t &channel, const char *mesh_ssid, const char *mesh_pass, const char *wifi_ssid, const char *wifi_pass)
{
    bool keepGoing = WypWiFi_c::IsChannelConform(channel);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsSsidConform(mesh_ssid);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsPasswordConform(mesh_pass);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsSsidConform(wifi_ssid);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsPasswordConform(wifi_pass);

    if (keepGoing)
    {
        mesh.setRoot(true);
        mesh.init(mesh_ssid, mesh_pass, MESH_PORT, WIFI_AP_STA, channel);
        mesh.stationManual(wifi_ssid, wifi_pass);
    }

    return keepGoing;
}

bool WypMesh_c::BeginNode(const uint8_t &channel, const char *mesh_ssid, const char *mesh_pass)
{
    bool keepGoing = WypWiFi_c::IsChannelConform(channel);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsSsidConform(mesh_ssid);

    if (keepGoing)
        keepGoing = WypWiFi_c::IsPasswordConform(mesh_pass);

    if (keepGoing)
    {
        mesh.setRoot(false);
        mesh.init(mesh_ssid, mesh_pass, MESH_PORT, WIFI_AP_STA, channel);
    }

    return keepGoing;
}

void WypMesh_c::Stop()
{
    mesh.stop();
}

void WypMesh_c::Update()
{
    mesh.update();
}

void WypMesh_c::_OnMessageReceived(uint32_t from, String &msg)
{
    if (manager)
    {
        if (manager->Exists(from))
        {
            debugln(MESH, "Treating message from %u", from);

            const char *mesh_password;
            int mesh_pass_salt = 0;
            const char *ws_password;
            int ws_pass_salt = 0;
            Preferences.Get(preferences_key_t::mesh_password, &mesh_password);
            Preferences.Get(preferences_key_t::mesh_password_salt, &mesh_pass_salt);
            Preferences.Get(preferences_key_t::ws_password_salt, &ws_pass_salt);
            Preferences.Get(preferences_key_t::ws_password, &ws_password);

            MeshClient_c *client = manager->Get(from);

            JsonReceived_s received(msg.c_str());

            //does not matter if conform or not

            CommandManager_c commands(wypManager, &received);

            JsonResponse_s *response;

            bool sent = false;
            if (received.From() == DESTINATION_WS_CLIENTS)
            {
                debugln(MESH, "message coming from WS");
                response = commands.Handle(ws_password, ws_pass_salt, client->Salt());
                sent = client->Send(response, ws_password, ws_pass_salt, client->Salt(true));
            }
            else
            {
                debugln(MESH, "message coming from MESH");
                //modify from's salt
                // client->Salt(received.Salt());
                response = commands.Handle(mesh_password, received.PassSalt(), received.Salt());
                sent = client->Send(response, mesh_password, mesh_pass_salt, trng());
            }

            if (response)
            {
                debugln(MESH, "Sending response... %s", sent ? "Done" : "Failed");
            }
        }
        else
        {
            debugln(MESH, "%u is not registered in the mesh client manager!", from);
        }
    }
}

void WypMesh_c::_OnNewConnection(uint32_t nodeId)
{
    if (manager)
    {
        manager->Add(nodeId);

        _NotifyWsClients();
        debugln(MESH, "Connection with %u has been established", nodeId);

        if (wypManager->GetWiFi() && wypManager->GetWiFi()->Mode() == wyp_wifi_modes_t::bridge)
        {
            _NotifyBridge();
        }
    }
}

void WypMesh_c::_OnDroppedConnection(uint32_t nodeId)
{
    if (manager)
    {
        manager->Erase(nodeId);
        _NotifyWsClients();
        debugln(MESH, "Connection with %u has been lost", nodeId);
    }
}

void WypMesh_c::_NotifyWsClients()
{
    if (wypManager)
    {
        WypWebServer_c *server = wypManager->GetWebServer();
        if (server)
        {
            WypWebSocket_c *socket = server->GetWebSocket();
            if (socket)
            {
                WebSocketClientManager_c *wsmanager = socket->GetClientManager();
                if (wsmanager)
                {
                    int pass_salt;
                    if (Preferences.Get(preferences_key_t::ws_password_salt, &pass_salt))
                    {
                        CommandManager_c cmd(wypManager);
                        JsonResponse_s *response = cmd.Handle(commands_keys_t::meshclts, pass_salt, DESTINATION_WS_CLIENTS);
                        if (response)
                        {
                            for (WebSocketClientManager_c::pair pair : *wsmanager)
                            {
                                pair.second->Send(response);
                            }
                        }
                    }
                }
            }
        }
    }
}

void WypMesh_c::_NotifyBridge()
{
    debugln(MESH, "Notify Bridge");
    bool keepGoing = wypManager;

    WypMesh_c *mesh = NULL;
    WypWiFi_c *wifi;
    MeshClientManager_c *manager = NULL;
    const char *password = NULL;
    int pass_salt;

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

    if (keepGoing)
        keepGoing = wifi->Mode() == wyp_wifi_modes_t::bridge;

    if (keepGoing)
        keepGoing = Preferences.Get(preferences_key_t::mesh_password, &password);

    if (keepGoing)
        keepGoing = Preferences.Get(preferences_key_t::mesh_password_salt, &pass_salt);

    if (keepGoing)
    {
        for (MeshClientManager_c::pair pair : *manager)
        {
            if (pair.second)
            {
                CommandManager_c cmd(wypManager);
                JsonResponse_s *response = cmd.Handle(commands_keys_t::masternode, pass_salt, pair.second->GetNodeId());
                pair.second->Send(response, password, pass_salt, pair.second->Salt(true));
            }
        }
    }
    else
    {
        debugln(MESH, "unable to notify bridge");
    }
}

} // namespace wyp