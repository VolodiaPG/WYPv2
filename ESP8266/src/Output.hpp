#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include <functional>

namespace wyp
{

class Output_c
{
  typedef std::function<void(void)> callback_function_t;
  //methods
public:
  Output_c(const uint8_t pin);

  /**
 * @brief Set the Callback function triggered when the state of the output changes
 * 
 * @param callback the function
 */
  void SetCallback(callback_function_t callback);

  void DigitaWrite(bool status);
  void setHigh();
  void setLow();
  uint8_t GetPin();
  bool IsHigh();
  void Lock(bool state) { lock = state; }

  //variables
protected:
  const uint8_t pin;
  bool status = false;
  bool lock = false;

private:
  callback_function_t _callback_function = NULL;
};
} // namespace wyp

#endif