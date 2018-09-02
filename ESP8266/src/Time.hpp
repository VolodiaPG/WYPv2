#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "WypManager.hpp"

namespace wyp
{

#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_UPDATE_INTERVAL 60 * 60 * 1000

class WypTime_i;
class NodeTime_c;
class NtpTime_c;

class Time_c
{
  //methods
public:
  Time_c();
  ~Time_c();
  void Begin();
  void Update();
  String GetFormattedTime();
  void SetTimezone(int timezone);
  int GetTimezone();
  unsigned long GetEpochTime();
  unsigned long GetRawEpochTime();

  NodeTime_c *GetNodeTime() { return _nodeTime; }

private:
  void _SwitchToNode();
  void _SwitchToNtp();
  void _OnNtpUpdate();

  //variables
private:
  uint32_t _lastMode = 0;

  NtpTime_c *_ntpTime;
  NodeTime_c *_nodeTime;
  WypTime_i *_time;
};

/**
 * @brief interface to implement gestion of time wether the node is in brdge or node mode
 * 
 */
class WypTime_i
{
public:
  virtual String GetFormattedTime() = 0;
  virtual unsigned long GetEpochTime() = 0;
  virtual unsigned long GetRawEpochTime() = 0;
  virtual void SetTimeOffset(int offset) = 0;
  virtual void Update() = 0;
  virtual ~WypTime_i() = default;
};

class NtpTime_c : WypTime_i
{
  //methods
public:
  NtpTime_c()
      : _ntpClient(_ntpUDP, NTP_SERVER, NTP_OFFSET, NTP_UPDATE_INTERVAL) {}
  ~NtpTime_c();

  String GetFormattedTime() { return _ntpClient.getFormattedTime(); }
  unsigned long GetEpochTime() { return _ntpClient.getEpochTime(); }
  unsigned long GetRawEpochTime() { return _ntpClient.getRawEpochTime(); }
  void SetTimeOffset(int offset) { _ntpClient.setTimeOffset(offset); }
  void SetCallbackOnUpdate(std::function<void(void)> func) { _ntpClient.setCallbackOnUpdate(func); }
  void Update() { _ntpClient.update(); }
  void Begin() { _ntpClient.begin(); }
  void End() { _ntpClient.end(); }
  //variables
private:
  WiFiUDP _ntpUDP;
  NTPClient _ntpClient;
};

class NodeTime_c : WypTime_i
{
  //methods
public:
  ~NodeTime_c() {}
  String GetFormattedTime();
  void SetTimeOffset(int offset) { _offset = offset; }
  void Update();
  void SetRawEpochTime(unsigned long epoc);
  unsigned long GetRawEpochTime() { return _epoc + (millis() - _lastUpdate) / 1000; }
  unsigned long GetEpochTime() { return _epoc + _offset + (millis() - _lastUpdate) / 1000; }

  //variables
private:
  unsigned long _epoc = 0;        //in s
  unsigned long _lastUpdate = 0;  //in ms
  int _offset = 0;                //in s
  unsigned long _lastAttempt = 0; //in ms
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIME)
extern Time_c Time;
#endif

} // namespace wyp

#endif