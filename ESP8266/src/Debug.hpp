#include <Arduino.h>

namespace wyp
{
#ifndef DEBUG_H
#define DEBUG_H

enum debug_flags_t
{
#define ITEM(X, Y) X = Y,
#include "DebugKeyList"
};

const char* DebugFlagsToString(debug_flags_t key);

#ifdef WYP_DEBUG
class Debug_c
{
  //methods
public:
  Debug_c();

  void SetDebugFlags(uint16_t flags);

  // size_t Println(debug_flags_t flag, const char * format, ...);

  size_t Print(debug_flags_t flag, const char *endline, const char *format, ...);

private:
  // size_t _Print(debug_flags_t flags, const char* endline, const char* format, ...);
  void _Output(debug_flags_t flag, const char *str, const char *endline);
  //variables
  // private:
  uint16_t _flags = 0;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DEBUG)

extern Debug_c Debug;

#endif
#endif
#endif

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_DEBUG) && defined(WYP_DEBUG)

#define debugVar(var) Debug.Print(debug_flags_t::VAR, "\r\n", "%s: %s", #var, var)
#define debugln(flag, ...) Debug.Print(flag, "\r\n", __VA_ARGS__)
#define debug(flag, ...) Debug.Print(flag, "", __VA_ARGS__)

#else

#define debugVar(var)
#define debugln(flag, ...)
#define debug(flag, ...)

#endif

} // namespace wyp