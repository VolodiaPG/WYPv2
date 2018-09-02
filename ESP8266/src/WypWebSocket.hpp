#ifndef WYPWEBSOCKET_H
#define WYPWEBSOCKET_H

#include <Arduino.h>
#include <AsyncWebSocket.h>
#include <Ticker_latest.h>
#include "WebSocketClientManager.hpp"
#include "WypManager.hpp"

namespace wyp
{

class WebSocketClientManager_c;
class WypManager_c;

class WypWebSocket_c : public AsyncWebSocket
{
    //methods
  public:
    WypWebSocket_c(const char *uri, WypManager_c *manager);
    WebSocketClientManager_c *GetClientManager() { return &_clientManager; }

  private:
    void _Init();

    void _onWebsocketEvent(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);

    void _OnEventConnect(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);

    void _OnEventDisconnect(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);

    void _OnEventError(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);

    void _OnEventPong(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);

    void _OnEventData(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len);
    void _HandleEventData(WebSocketClient_c *client, String &message);

    void _onWebSocketHeartbeat(void);

    //variables
  private:
    Ticker_latest _ticker;
    WebSocketClientManager_c _clientManager;
    WypManager_c *_wypManager;
};

} // namespace wyp
#endif