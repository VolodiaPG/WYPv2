#include "JsonMessages.hpp"

#include <Arduino.h>
#include <string>
#include "JsonBundle.hpp"
#include "Debug.hpp"
#include "sha256.h"

namespace wyp
{

#define KEY_TO "to"
#define KEY_FROM "from"
#define KEY_ARGUMENTS "arguments"
#define KEY_CHECKSUM "checksum"
#define KEY_PASS_SALT "passsalt"
#define KEY_SALT "salt"
#define KEY_ERROR "error"

/***
 * 
 * Json Message
 * 
 ***/

JsonMessage_s::JsonMessage_s(const uint32_t from, const uint32_t to)
    : _from(from),
      _to(to)
{

    isConform = true;

    JsonBundle_s::CreateBundle();

    arguments = Root().createNestedObject(KEY_ARGUMENTS);

    Root()[KEY_FROM] = _from;
    Root()[KEY_TO] = _to;
}

JsonMessage_s::JsonMessage_s(const char *message)
{
    JsonBundle_s::CreateBundle(message);

    debugln(WSMESSAGE, "message: %s", message);

    isConform = true;

    if (isConform)
        isConform = Root().containsKey(KEY_ARGUMENTS);

    if (isConform)
        arguments = Root()[KEY_ARGUMENTS];

    if (isConform)
        isConform = Root().containsKey(KEY_CHECKSUM);

    if (isConform)
        isConform = Root().containsKey(KEY_FROM);

    if (isConform)
        isConform = Root().containsKey(KEY_TO);

    if (isConform)
        isConform = Arguments().measureLength() > 2;

    if (isConform)
    {
        _from = Root().get<uint32_t>(KEY_FROM);
        _to = Root()[KEY_TO];
    }

    if (!isConform)
    {
        debugln(WSMESSAGE, "Source is not conform");
    }
}

JsonMessage_s::~JsonMessage_s()
{
    if (_hash)
        free(_hash);
}

char *JsonMessage_s::GenerateChecksum(const char *password, const int pass_salt, const int salt, const JsonObject &payload)
{
    size_t len = payload.measureLength();
    char jsonChar[len + 1];
    payload.printTo((char *)jsonChar, len + 1);

    uint8_t *hash;
    Sha256.init();
    Sha256.printf("%s%i%i%s", password, pass_salt, salt, jsonChar);
    hash = Sha256.result();

    char *ret = (char *)malloc(32 * sizeof(char *) + 1);
    if (ret)
    {
        for (int ii = 0; ii < 32; ++ii)
        {
            sprintf(ret + 2 * ii, "%02X", hash[ii]);
        }
    }

    return ret;
}

void JsonMessage_s::ComputeChecksum(const char *password, const int pass_salt, const int salt)
{
    if (_hash)
    {
        free(_hash);
    }
    _hash = GenerateChecksum(password, pass_salt, salt, Arguments());
    Root()[KEY_CHECKSUM] = _hash;
}

bool JsonMessage_s::IsChecksumCorrect(const char *password, const int pass_salt, const int salt)
{
    bool ret = false;
    char *hash = GenerateChecksum(password, pass_salt, salt, Arguments());
    const char *cs = Checksum();
    char *cs_up = (char *)malloc(strlen(cs) / 2 * sizeof(char *) + 1);
    if (cs_up)
    {
        char *cs_it;
        cs_it = cs_up;
        while (*cs)
        {
            *cs_it = toupper(*cs);
            cs++;
            cs_it++;
        }
        *cs_it = '\0';

        ret = strcmp(hash, cs_up) == 0;

        free(cs_up);
    }

    free(hash);
    return ret;
}

void JsonMessage_s::PrepareToSend(const char *password, const int pass_salt, const int salt)
{
    Root()[KEY_SALT] = salt;
    Root()[KEY_PASS_SALT] = pass_salt;

    debugVar(password);

    ComputeChecksum(password, pass_salt, salt);

    if (!Root().containsKey(KEY_ERROR) && Arguments().measureLength() <= 2)
    {
        Root()[KEY_ERROR] = "empty message";
    }
}

JsonObject &JsonMessage_s::Arguments() const
{
    return arguments;
}

bool JsonMessage_s::IsConform()
{
    return isConform;
}

const char *JsonMessage_s::Checksum() const
{
    return Root()[KEY_CHECKSUM];
}

/***
 * 
 * Json Response
 * 
 ***/

JsonResponse_s::JsonResponse_s(const uint32_t from, const uint32_t to)
    : JsonMessage_s(from, to)
{
}

int JsonResponse_s::Salt() const
{
    return Root()[KEY_SALT];
}

/***
 * 
 * Json Received
 * 
 ***/

JsonReceived_s::JsonReceived_s(const char *message)
    : JsonMessage_s(message)
{
}

int JsonReceived_s::Salt() const
{
    return Root()[KEY_SALT];
}

int JsonReceived_s::PassSalt() const
{
    return Root()[KEY_PASS_SALT];
}
} // namespace wyp