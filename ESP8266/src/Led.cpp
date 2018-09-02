#include "Led.hpp"

namespace wyp
{

Led_c::Led_c(uint8_t pin):
Output_c(pin)
{
}

Led_c::~Led_c()
{
    ticker.detach();
}

int Led_c::_GetDimmingToWrite(int dim)
{
    return LED_MAX - dim;
}

void Led_c::_Write(int dim)
{
    _SetDimming(dim);
    analogWrite(GetPin(), _GetDimmingToWrite(dim));
}

void Led_c::Write(int dim)
{
    ticker.detach();
    _Write(dim);
}

void Led_c::SetHigh()
{
    Write(true);
}

void Led_c::SetLow()
{
    Write(false);
}

void Led_c::_SetDimming(int dimming)
{
    _dimming = dimming;
    status = dimming > MAX / 2;
}

int Led_c::GetDimming()
{
    return _dimming;
}

int Led_c::GetDimmingWanted()
{
    return _dimming_objective;
}

void Led_c::SetRefreshRate(uint rate)
{
    _refreshRate = rate;
}

void Led_c::FadeWrite(int dim, bool continuous)
{
    _continuous = continuous;

    _SetDimmingObjective(dim);
    ticker.detach();
    ticker.attach_ms(10, _OnTick, this);
}

void Led_c::FadeOn()
{
    FadeWrite(MAX);
}

void Led_c::FadeOff()
{
    FadeWrite(0);
}

void Led_c::FadeContinuously()
{
    int center = MAX / 2;
    FadeWrite(GetDimming() - center < 0 ? 0 : MAX, true);
}

void Led_c::StopContinuousFading()
{
    _continuous = false;
}

void Led_c::_OnTick(Led_c *const instance)
{
    int currentDimming = instance->GetDimming();
    int diff = currentDimming - instance->GetDimmingWanted();

    if (diff == 0)
    {
        if (instance->_continuous)
        {
            instance->_SetDimmingObjective(instance->_GetDimmingToWrite(currentDimming));
        }
        else
        {
            instance->ticker.detach();
        }
    }
    else if (abs(diff) <= abs(instance->_increment))
    {
        instance->_Write(instance->GetDimmingWanted());
    }
    else
    {
        int sign = diff < 0 ? 1 : -1;
        int toWrite = currentDimming + sign * instance->_increment;
        instance->_Write(toWrite);
    }
}

void Led_c::_SetDimmingObjective(int dimming)
{
    _dimming_objective = dimming;
}

uint Led_c::GetIncrement()
{
    return _increment;
}

void Led_c::SetIncrement(uint increment)
{
    _increment = increment;
}

}