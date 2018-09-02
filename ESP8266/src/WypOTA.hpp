#ifndef WYP_OTA_H
#define WYP_OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Led.hpp"

namespace wyp
{

/**
 * @brief Encapsulate Arduino OTA for using with wyp device
 * 
 */
class WypOTA_c
{

    //Methods

  public:
    /**
   * @brief Construct a new WypOTA_c object
   * 
   * @param password the password
   * @param hostname the hostname
   * @param port the port
   */
    WypOTA_c(String password, String hostname, int port);

    /**
 * @brief Construct a new WypOTA_c object
 * 
 */
    WypOTA_c();

    /**
 * @brief Readies the object for the handle command
 * 
 */
    void Begin();

    /**
 * @brief Handle the loop
 * 
 */
    void Handle();

    /**
 * @brief Set the Password
 * 
 * @param password 
 */
    void SetPassword(String password);

    /**
 * @brief Get the Password
 * 
 * @return String 
 */
    String GetPassword();

    /**
 * @brief Set the Hostname
 * 
 * @param hostname 
 */
    void SetHostname(String hostname);

    /**
 * @brief Get the Hostname
 * 
 * @return String 
 */
    String GetHostname();

    /**
 * @brief Set the Port
 * 
 * @param port 
 */
    void SetPort(int port);

    /**
 * @brief Get the Port
 * 
 * @return int 
 */
    int GetPort();

    /**
 * @brief Set the Led object (if using one)
 * 
 * @param led 
 */
    void SetLed(Led_c *led);

  private:
    /**
   * @brief Init the assignation of callback methods
   * 
   */
    void _Init();

    /**
 * @brief OnStart callback
 * 
 */
    void _OnStart();

    /**
 * @brief OnEnd callback
 * 
 */
    void _OnEnd();

    /**
 * @brief OnError callback
 * 
 * @param error 
 */
    void _OnError(ota_error_t error);

    /**
 * @brief OnProgress callback
 * 
 * @param progress 
 * @param total 
 */
    void _OnProgress(unsigned int progress, unsigned int total);

    String _ProgressIndicator(int percent);

    //Variables

  private:
    /**
   * @brief The port
   * 
   */
    int _port;

    /**
     * @brief The password
     * 
     */
    String _password;

    /**
     * @brief The hostname
     * 
     */
    String _hostname;

    /**
     * @brief The led
     * 
     */
    Led_c *_led = NULL;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WYP_OTA)
extern WypOTA_c WypOTA;
#endif

}
#endif