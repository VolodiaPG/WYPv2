#include <Arduino.h>
#include <FS.h>
#include <string>
#include "Debug.hpp"

#include "Preferences.hpp"

using namespace std;

#define PREFERENCES_PATH "/private/preferences.json"

namespace wyp
{

Preferences_c::Preferences_c(const char *filepath)
    : _filepath(filepath)
{
}

void Preferences_c::Begin()
{
    if (!_began)
    {
        std::string str;
        if (GetFromFile(_filepath, &str))
        {
            _bundle.CreateBundle(str.c_str());
        }
        else
        {
            _bundle.CreateBundle();
        }
        _began = true;
    }
}

bool Preferences_c::GetFromFile(const char *filepath, std::string *str)
{
    File file = SPIFFS.open(filepath, "r");
    bool ret = false;

    if (!file)
    {
        File file = SPIFFS.open(filepath, "w");
        if (!file)
        {
            debugln(FILESYSTEM, "File creation failed (%s)", filepath);
        }

        file.close();
    }
    else
    {
        if (str)
        {
            while (file.available())
            {
                *str += char(file.read());
            }
            ret = true;
        }
    }

    return ret;
}

void Preferences_c::Save()
{
    File file = SPIFFS.open(_filepath, "w");

    if (!file)
    {
        debugln(FILESYSTEM, "File creation failed (%s)", _filepath);
    }
    else
    {
        _bundle.Root().printTo(file);
        // _bundle.Root().prettyPrintTo(Serial);
        debugln(FILESYSTEM, "Successfully saved (%s)", _filepath);
    }

    file.close();
}

const char *PreferencesKeyToString(preferences_key_t key)
{
    switch (key)
    {
#define ITEM(X, Y) \
    case X:        \
        return #X;
#include "PreferencesKeyList"
    }

    return NULL;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PREFERENCES)

Preferences_c Preferences(PREFERENCES_PATH);

#endif

} // namespace wyp