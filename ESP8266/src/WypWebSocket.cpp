#include <Ticker_latest.h>
#include <painlessMesh.h>
#include "WypWebSocket.hpp"
#include "Debug.hpp"
#include "Preferences.hpp"
#include "CommandManager.hpp"

#define WS_HEARTBEAT 1000 //ms

namespace wyp
{

WypWebSocket_c::WypWebSocket_c(const char *uri, WypManager_c *manager)
    : AsyncWebSocket(uri)
{
    _wypManager = manager;
    _Init();
}

void WypWebSocket_c::_Init()
{
    using namespace std::placeholders;
    this->onEvent(std::bind(&WypWebSocket_c::_onWebsocketEvent, this, _1, _2, _3, _4, _5, _6));

    _ticker.attach_ms(WS_HEARTBEAT, std::bind(&WypWebSocket_c::_onWebSocketHeartbeat, this));
}

void WypWebSocket_c::_onWebsocketEvent(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        _clientManager.Add(client, &_clientManager);
        _OnEventConnect(server, client, type, arg, data, len);
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        _clientManager.Erase(client->id());
        _OnEventDisconnect(server, client, type, arg, data, len);
    }
    else if (type == WS_EVT_ERROR)
    {
        _clientManager.Erase(client->id());
        _OnEventError(server, client, type, arg, data, len);
    }
    else if (type == WS_EVT_PONG)
    {
        _OnEventPong(server, client, type, arg, data, len);
    }
    else if (type == WS_EVT_DATA)
    {
        _OnEventData(server, client, type, arg, data, len);
    }
}

void WypWebSocket_c::_OnEventConnect(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    debug(debug_flags_t::WEBSOCKET, "ws[%s][%u] connect\r\n", server->url(), client->id());

    int pass_salt;
    const char *password;
    if (Preferences.Get(preferences_key_t::ws_password_salt, &pass_salt) &&
        Preferences.Get(preferences_key_t::ws_password, &password) &&
        password)
    {
        WebSocketClient_c *clt = _clientManager.Get(client->id());

        CommandManager_c cmd(_wypManager);
        JsonResponse_s *response = cmd.Handle(commands_keys_t::meshclts, pass_salt, DESTINATION_WS_CLIENTS);
        
        if (response)
        {
            debugln(WEBSOCKET, "%s", clt->Send(response) ? "sent" : "not sent");
        }
        else
        {
            debugln(WEBSOCKET, "response cannot be sent (null)");
        }
    }
    else
    {
        debugln(WEBSOCKET, "can't access the preferences (not existing)");
    }

    client->ping();
}

void WypWebSocket_c::_OnEventDisconnect(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    debugln(debug_flags_t::WEBSOCKET, "ws[%s][%u] disconnect: %u\r\n", server->url(), client->id(), client->id());
}

void WypWebSocket_c::_OnEventError(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    debugln(debug_flags_t::WEBSOCKET, "ws[%s][%u] error(%u): %s\r\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
}

void WypWebSocket_c::_OnEventPong(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    // debug(debug_flags_t::WEBSOCKET,"ws[%s][%u] pong[%u]: %s\r\n", server->url(), client->id(), len, (len) ? (char *)data : "");
}

void WypWebSocket_c::_OnEventData(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
        //the whole message is in a single frame and we got all of it's data
        debugln(WEBSOCKET, "ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

        if (info->opcode == WS_TEXT)
        {
            for (size_t i = 0; i < info->len; i++)
            {
                msg += (char)data[i];
            }
        }
        else
        {
            char buff[3];
            for (size_t i = 0; i < info->len; i++)
            {
                sprintf(buff, "%02x ", (uint8_t)data[i]);
                msg += buff;
            }
        }

        debug(WEBSOCKET, "%s\r\n", msg.c_str());

        if (info->opcode == WS_TEXT)
        {
            // client->text("I got your text message");

            WebSocketClient_c *clt = _clientManager.Get(client->id());
            _HandleEventData(clt, msg);
        }
    }
    else
    {
        debugln(WEBSOCKET, "Message is too long to be understood (not coded yet ^^')");
    }
}

void WypWebSocket_c::_HandleEventData(WebSocketClient_c *client, String &message)
{
    if (client)
    {
        const char *password;
        int pass_salt;
        Preferences.Get(preferences_key_t::ws_password_salt, &pass_salt);
        Preferences.Get(preferences_key_t::ws_password, &password);

        JsonReceived_s received(message.c_str());

        CommandManager_c CommandManager(_wypManager, &received);

        JsonResponse_s *response = CommandManager.Handle(password, pass_salt, client->Salt());

        if (response)
        {
            client->Send(response);
            debugln(WEBSOCKET, "Sending response... Done");
        } 
        else
        {
            debugln(WEBSOCKET, "Sending response... Failed (null)");
        }
    }
    else
    {
        debug(WEBSOCKET, "client does not exist (null)");
    }
}

void WypWebSocket_c::_onWebSocketHeartbeat(void)
{
    if (this->count() > 0)
    {
        this->pingAll();
    }
}
} // namespace wyp