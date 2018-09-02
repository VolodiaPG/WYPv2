#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include "JsonMessages.hpp"
#include <map>
#include "WebSocketClientManager.hpp"
#include "WypManager.hpp"

namespace wyp
{

enum commands_keys_t
{
#define COMMAND(X, Y) X = Y,
#include "Commands"
};

static std::map<std::string, commands_keys_t> converter{
#define COMMAND(X, Y) {#X, X},
#include "Commands"
};

const char *CommandsKeyToString(commands_keys_t key);
commands_keys_t StringToCommandsKey(const char *str);

class CommandManager_c
{
  //command methods
protected:
#define COMMAND(X, Y) void X(const commands_keys_t &key);
#include "Commands"
  // void wsclts(const commands_keys_t &key);
  // void args(const commands_keys_t &key);
  // void restart(const commands_keys_t &key);
  // void heap(const commands_keys_t &key);
  // void relay(const commands_keys_t &key);
  // void meshclts(const commands_keys_t &key);

  //methods
public:
  CommandManager_c(WypManager_c *manager, JsonReceived_s *messageReceived);
  CommandManager_c(WypManager_c *manager);
  ~CommandManager_c();

  /**
 * @brief handle the command received
 * 
 * @param password The password used to generate the checksum
 * @param pass_salt the salt used alongside the password
 * @return JsonResponse_s* the response to send back
 */
  JsonResponse_s *Handle(const char *password, const int &pass_salt, const int &salt);

  /**
   * @brief Handle an internal command, no need to protect it because it's generated as the module needs it
   * 
   * @param key the command key
   * @param pass_salt the password salt (for the reponse)
   * @return JsonResponse_s* the message to emit
   */
  JsonResponse_s *Handle(commands_keys_t key, const int &pass_salt, const uint32_t &to);

private:
  void _CommandLogic();
  void _HandleCommand(commands_keys_t key);
  void _HandleNotDestination();

  //variables
protected:
  //intern
  JsonResponse_s *response = NULL;

  //extern
  JsonReceived_s *received = NULL;
  WypManager_c *wypManager = NULL;

  //useful for commands
  int salt;
};

} // namespace wyp

#endif