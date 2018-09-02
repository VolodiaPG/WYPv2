#ifndef WEBSOCKETCLIENTMANAGER_H
#define WEBSOCKETCLIENTMANAGER_H

#include <Arduino.h>
#include <map>
#include <AsyncWebSocket.h>
#include "WebSocketClient.hpp"

namespace wyp
{

class WebSocketClient_c;
// class WypWebSocket_c;
typedef std::map<uint32_t, WebSocketClient_c *> clients_map_t;

class WebSocketClientManager_c
{
  //methods
public:
  WebSocketClientManager_c();
  ~WebSocketClientManager_c();

  bool Exists(uint32_t id);
  WebSocketClient_c *Get(uint32_t id);
  void Add(AsyncWebSocketClient *client, WebSocketClientManager_c *manager);
  void Erase(uint32_t id);
  clients_map_t *GetMap();

  //iterator
  using pair = std::pair<uint32_t, WebSocketClient_c*>;
  using iterator = clients_map_t::const_iterator;
  iterator begin() const { return clients.begin(); }
  iterator end() const { return clients.end(); }

  //variables
protected:
  clients_map_t clients;
};

} // namespace wyp
#endif