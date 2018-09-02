#include "Time.hpp"
#include "Preferences.hpp"
#include "CommandManager.hpp"
#include "MeshClient.hpp"
#include "Debug.hpp"
#include "Salt.h"

namespace wyp
{

#define WAIT_TILL_ATTEMPT 10000

/***
 * 
 * TIME_C
 * 
 ***/

Time_c::Time_c()
{
    _ntpTime = new NtpTime_c();
    _nodeTime = new NodeTime_c();
    _ntpTime->SetCallbackOnUpdate(std::bind(&Time_c::_OnNtpUpdate, this));
    _time = NULL;
}

Time_c::~Time_c()
{
    if (_ntpTime)
        delete _ntpTime;
    if (_nodeTime)
        delete _nodeTime;
}

void Time_c::Begin()
{
    SetTimezone(GetTimezone());

    // WypWiFi_c *wifi = WypManager.GetWiFi();
    // if (wifi)
    // {
    //     _lastMode = wifi->Mode();
    // }
}

void Time_c::Update()
{
    WypWiFi_c *wifi = WypManager.GetWiFi();
    if (wifi)
    {
        if (wifi->Mode() != _lastMode)
        {
            if (wifi->Mode() == wyp_wifi_modes_t::bridge)
            {
                _SwitchToNtp();
            }
            else if (wifi->Mode() == wyp_wifi_modes_t::node)
            {
                _SwitchToNode();
            }

            if (_time)
            {
                _time->SetTimeOffset(GetTimezone() * 60 * 60);
            }
            else
            {
                debugln(debug_flags_t::TIME, "_time is null");
            }

            _lastMode = wifi->Mode();
        }
    }

    if (_time)
    {
        _time->Update();
    }
}

String Time_c::GetFormattedTime()
{
    String ret = "";
    if (_time)
    {
        ret = _time->GetFormattedTime();
    }
    return ret;
}

void Time_c::SetTimezone(int timezone)
{
    if (timezone >= -12 && timezone <= 14 && _time)
    {
        Preferences.Set(preferences_key_t::timezone_pref, timezone);
        Preferences.Save();
        if (_time)
        {
            _time->SetTimeOffset(timezone * 60 * 60);
        }
    }
}

int Time_c::GetTimezone()
{
    int ret = 0;
    Preferences.Get(preferences_key_t::timezone_pref, &ret);
    return ret;
}

unsigned long Time_c::GetEpochTime()
{
    unsigned long epoch = 0;
    if (_time)
    {
        epoch = _time->GetEpochTime();
    }
    return epoch;
}

unsigned long Time_c::GetRawEpochTime()
{
    unsigned long epoch = 0;
    if (_time)
    {
        epoch = _time->GetRawEpochTime();
    }
    return epoch;
}

void Time_c::_SwitchToNode()
{
    if (_ntpTime)
    {
        _ntpTime->End();
    }

    _time = (WypTime_i *)_nodeTime;
    debugln(debug_flags_t::TIME, "Switched to NODE");
}

void Time_c::_SwitchToNtp()
{
    if (_ntpTime)
    {
        _ntpTime->Begin();
    }

    _time = (WypTime_i *)_ntpTime;
    debugln(debug_flags_t::TIME, "Switched to NTP");
}

void Time_c::_OnNtpUpdate()
{
    WypMesh_c *mesh = WypManager.GetMesh();
    if (mesh)
    {
        MeshClientManager_c *manager = mesh->GetMeshClientManager();
        if (manager)
        {
            const char *password = NULL;
            int pass_salt;
            if (Preferences.Get(preferences_key_t::mesh_password, &password))
            {
                if (Preferences.Get(preferences_key_t::mesh_password_salt, &pass_salt))
                {
                    for (MeshClientManager_c::pair pair : *manager)
                    {
                        if (pair.second)
                        {
                            CommandManager_c cmd(&WypManager);
                            JsonResponse_s *response = cmd.Handle(commands_keys_t::epochtime, pass_salt, pair.second->GetNodeId());
                            pair.second->Send(response, password, pass_salt, pair.second->Salt());
                        }
                    }
                }
            }
        }
    }
}

/***
 * 
 * NTPTIME_C
 * 
 ***/

NtpTime_c::~NtpTime_c()
{
    _ntpClient.end();
}

/***
 * 
 * NODETIME_C
 * 
 ***/

String NodeTime_c::GetFormattedTime()
{
    unsigned long rawTime = GetEpochTime();
    unsigned long hours = (rawTime % 86400L) / 3600;
    String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

    unsigned long minutes = (rawTime % 3600) / 60;
    String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

    unsigned long seconds = rawTime % 60;
    String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

    return hoursStr + ":" + minuteStr + ":" + secondStr;
}

void NodeTime_c::SetRawEpochTime(unsigned long epoc)
{
    _lastUpdate = millis();
    _epoc = epoc;
}

void NodeTime_c::Update()
{
    if (_epoc == 0)
    {
        WypMesh_c *mesh = WypManager.GetMesh();
        if (mesh && millis() - _lastAttempt >= WAIT_TILL_ATTEMPT)
        {
            if (mesh->GetMasterId())
            {
                debugln(TIME, "master id: %i", mesh->GetMasterId());
                MeshClientManager_c *manager = mesh->GetMeshClientManager();
                if (manager)
                {
                    debugln(TIME, "manager");
                    uint32_t id = mesh->GetMasterId();
                    MeshClient_c *client = manager->Get(id);
                    if (client)
                    {
                        const char *password = NULL;
                        int pass_salt;
                        debugln(TIME, "mesh clt");
                        if (Preferences.Get(preferences_key_t::mesh_password, &password))
                        {
                            debugln(TIME, "mesh pass salt");
                            if (Preferences.Get(preferences_key_t::mesh_password_salt, &pass_salt))
                            {
                                debugln(TIME, "Epoch time is 0");
                                CommandManager_c cmd(&WypManager);
                                JsonResponse_s *response = cmd.Handle(commands_keys_t::epochtime, pass_salt, mesh->GetMasterId());
                                client->Send(response, password, pass_salt, trng());

                                _lastAttempt = millis();
                            }
                        }
                    }
                }
            }
        }
    }
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIME)
Time_c Time;
#endif

} // namespace wyp