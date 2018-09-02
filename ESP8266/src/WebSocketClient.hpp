#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <Arduino.h>
#include <AsyncWebSocket.h>
#include "JsonMessages.hpp"
#include "WebSocketClientManager.hpp"

namespace wyp
{

class WebSocketClientManager_c;

class WebSocketClient_c
{

public:
  //methods
  WebSocketClient_c();
  // WebSocketClient_c(const WebSocketClient_c &ws, WebSocketClientManager_c *parent);
  WebSocketClient_c(AsyncWebSocketClient *client, WebSocketClientManager_c *parent);
  bool Send(JsonResponse_s *response);
  bool Send(JsonReceived_s *message);
  AsyncWebSocketClient *GetClient();
  WebSocketClientManager_c *Parent() const { return _parent; };
  int Salt(bool newSalt = false);

private:
  bool _Send(JsonMessage_s *message);

  //variables
private:
  // WypWebSocket_c *_websocket;
  AsyncWebSocketClient *_client;
  WebSocketClientManager_c *_parent;
  int _salt;
};

} // namespace wyp
#endif