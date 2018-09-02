#include "WebSocketClient.hpp"
#include <ArduinoJson.h>
#include <AsyncWebSocket.h>
#include "JsonMessages.hpp"
#include "Debug.hpp"
#include "Salt.h"
#include "Preferences.hpp"

namespace wyp
{

WebSocketClient_c::WebSocketClient_c()
{
}

// WebSocketClient_c::WebSocketClient_c(const WebSocketClient_c &ws, WebSocketClientManager_c *parent)
//     : _client(ws.GetClient()),
//       _parent(parent)
// {
// }

WebSocketClient_c::WebSocketClient_c(AsyncWebSocketClient *client, WebSocketClientManager_c *parent)
    : _client(client),
      _parent(parent)
{
}

bool WebSocketClient_c::Send(JsonResponse_s *response)
{
    bool ret = false;

    if (response)
    {
        bool keepGoing = true;

        const char *password;
        int pass_salt;

        if (keepGoing)
            keepGoing = Preferences.Get(preferences_key_t::ws_password, &password) && password;
        
        if (keepGoing)
            keepGoing = Preferences.Get(preferences_key_t::ws_password_salt, &pass_salt);

        if (keepGoing)
        {
            response->PrepareToSend(password, pass_salt, Salt(true));
            debugln(WEBSOCKET, "Client sending a new message");
            ret = _Send(response);
        }
    }

    return ret;
}

bool WebSocketClient_c::Send(JsonReceived_s *message)
{
    bool ret = false;

    if (message)
    {
        debugln(WEBSOCKET, "Client sending a received message");
        ret = _Send(message);
    }

    return ret;
}

bool WebSocketClient_c::_Send(JsonMessage_s *message)
{
    bool ret = false;

    if (message && _client)
    {
        size_t len = message->Root().measureLength();
        AsyncWebSocketMessageBuffer *buffer = _client->server()->makeBuffer(len);
        if (buffer)
        {
            message->Root().printTo((char *)buffer->get(), len + 1);
            message->Root().prettyPrintTo(Serial);
            _client->text(buffer);
            ret = true;
        }
    }

    return ret;
}

AsyncWebSocketClient *WebSocketClient_c::GetClient()
{
    return _client;
}

int WebSocketClient_c::Salt(bool newSalt)
{
    if (newSalt)
    {
        _salt = trng();
    }
    return _salt;
}

} // namespace wyp