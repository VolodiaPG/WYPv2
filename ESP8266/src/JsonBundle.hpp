#ifndef JSONBUNDLE_H
#define JSONBUNDLE_H
#include <Arduino.h>
#include <ArduinoJson.h>

namespace wyp
{


struct JsonBundle_s
{
  //methods
public:
//   JsonBundle_c();
//   JsonBundle_c(const char *input);

  void CreateBundle();
  void CreateBundle(const char *input);

  JsonObject &Root() const;

  //variables
protected:
  DynamicJsonBuffer jsonBuffer;
  JsonVariant root;
};


} // namespace wyp
#endif