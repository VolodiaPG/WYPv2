#include "Output.hpp"
#include <Arduino.h>

namespace wyp
{
Output_c::Output_c(const uint8_t pin)
    : pin(pin)
{
    pinMode(pin, OUTPUT);
}

void Output_c::SetCallback(callback_function_t callback)
{
    _callback_function = callback;
}

void Output_c::DigitaWrite(bool state)
{
    bool current_status = status;
    status = state;
    digitalWrite(pin, !state);

    if (_callback_function && !lock && current_status != state)
    {
        _callback_function();
    }
}

void Output_c::setHigh()
{
    DigitaWrite(true);
}

void Output_c::setLow()
{
    DigitaWrite(false);
}

uint8_t Output_c::GetPin()
{
    return pin;
}

bool Output_c::IsHigh()
{
    return status;
}
} // namespace wyp