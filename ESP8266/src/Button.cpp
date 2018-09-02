#include "Button.hpp"
#include <functional>

namespace wyp
{

#define DEBOUNCE_COUNT 10

Button_c::Button_c(uint8_t pin, callback_function_t func)
    : _pin(pin)
{
    pinMode(_pin, INPUT);
    if (func)
    {
        _callback = func;
    }
    _current_state = digitalRead(_pin);
}

Button_c::~Button_c()
{
    _callback = nullptr;
}

void Button_c::Update()
{
    if (_callback)
    {
        //next ms
        if (millis() != _time)
        {
            bool state = digitalRead(_pin);

            if (state == _current_state && _counter > 0)
            {
                --_counter;
            }
            else if (state != _current_state)
            {
                ++_counter;
            }

            if (_counter >= DEBOUNCE_COUNT)
            {
                _counter = 0;
                _current_state = state;
                //state is inverted
                _callback(!_current_state);
            }
        }
    }
}

} // namespace wyp