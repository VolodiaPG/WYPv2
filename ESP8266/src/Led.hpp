#include "Arduino.h"
#include <Ticker_latest.h>
#include "Output.hpp"

namespace wyp
{

#ifndef LED_H
#define LED_H

class Led_c : Output_c
{
  //Variables
public:
  static const int MAX = 255;

private:
  static const int LED_MAX = 1023;

  /**
 * @brief represents the status of the led (from 0 - 255)
 * 
 */
  volatile int _dimming = 0;

  /**
 * @brief The incremental value for fading
 * 
 */
  uint _increment = 10;

  /**
 * @brief The dimming objective
 * 
 */
  volatile int _dimming_objective = 0;

  /**
 * @brief The refresh rate in ms
 * 
 */
  uint _refreshRate = 10;

  volatile bool _continuous = false;

protected:
  Ticker_latest ticker;

  //Methods
private:
  /**
 * @brief Set the @see _dimming variable
 * 
 * @param state the status
 */
  void _SetDimming(int dimming);

  int _GetDimmingToWrite(int dim);

  void _Write(int dim);

  void _SetDimmingObjective(int dimming);

  static void _OnTick(Led_c *const instance);

public:
  /**
 * @brief Construct a new Led object
 * 
 * @param pin The led pin
 */
  Led_c(uint8_t pin);

  /**
 * @brief Destroy the Led object
 * 
 */
  ~Led_c();

  /**
 * @brief Write the status of the led
 * 
 * @param state the status wanted
 */
  void Write(int dim);

  /**
   * @brief Turn the Led on
   */
  void SetHigh();

  /**
   * @brief Turn the Led off
   * 
   */
  void SetLow();

  /**
   * @brief Get the Status of the Led (on / off)
   * 
   * @return true if on
   * @return false false if not
   */
  int GetDimming();

  /**
 * @brief Get the Dimming objective
 * 
 * @return uint8_t the dimming ojective
 */
  int GetDimmingWanted();

  /**
 * @brief Set the Refresh Rate of the fading
 * 
 * @param rate the refresh rate in ms
 */
  void SetRefreshRate(uint rate);

  /**
 * @brief Get the Increment
 * 
 * @return uint8_t the increment
 */
  uint GetIncrement();

  /**
 * @brief Set the Increment
 * 
 * @param increment the increment wanted
 */
  void SetIncrement(uint increment);

  /**
 * @brief Fade to the dimming wanted
 * 
 * @param dim the dimming wanted
 */
  void FadeWrite(int dim, bool continuous = false);

  /**
 * @brief Fade to completely on
 * 
 */
  void FadeOn();

  /**
 * @brief Fade to completely off
 * 
 */
  void FadeOff();

  void FadeContinuously();

  void StopContinuousFading();
};

#endif
}