#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <functional>

namespace wyp
{

typedef std::function<void(const bool)> callback_function_t;

class Button_c
{
    //methods
  public:
    Button_c(uint8_t pin, callback_function_t func);
    ~Button_c();

    void Update();

    //variables
  private:
    callback_function_t _callback;
    uint8_t _pin;

    bool _current_state = false;
    int8_t _counter = 0;
    unsigned long _time;
};
} // namespace wyp

#endif