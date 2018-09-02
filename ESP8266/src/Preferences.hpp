#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <Arduino.h>
#include <string>
#include "JsonBundle.hpp"
#include "Debug.hpp"

namespace wyp
{

// typedef enum : char
// {
//   password = 0,
//   password_salt,
// } preferences_key_t;

enum preferences_key_t
{
#define ITEM(X, Y) X = Y,
#include "PreferencesKeyList"
};

const char *PreferencesKeyToString(preferences_key_t key);

class Preferences_c
{
  //methods
public:
  Preferences_c(const char *filepath);

  void Begin();

  template <class TValue>
  bool Get(preferences_key_t key, TValue *value)
  {
    bool ret = false;
    const char *key_str = PreferencesKeyToString(key);
    if (key_str)
    {
      if (_bundle.Root().containsKey(key_str))
      {
        ret = true;
        *value = _bundle.Root()[key_str].as<TValue>();
      }
      else
      {
        debug(PREFERENCES, "Preferences doesn't contain %s", key_str);
      }
    }
    return ret;
  }

  template <class TValue>
  void Set(preferences_key_t key, TValue value)
  {
    const char *key_str = PreferencesKeyToString(key);
    if (key_str)
    {
      _bundle.Root()[key_str] = value;
    }
  }

  void Save();

protected:
  static bool GetFromFile(const char *filepath, std::string *str);

  //variables
private:
  JsonBundle_s _bundle;
  const char *_filepath;
  bool _began = false;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PREFERENCES)

extern Preferences_c Preferences;

#endif

} // namespace wyp
#endif