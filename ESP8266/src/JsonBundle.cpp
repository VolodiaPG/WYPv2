#include "JsonBundle.hpp"
#include "Debug.hpp"

namespace wyp
{

// JsonBundle_c::JsonBundle_c()
//     // : root(jsonBuffer.createObject())
// {
//     root  = jsonBuffer.createObject();
//     Root()["toto"] = "toto";
//     debugVar(Root()["toto"]);
//     debugln(DEBUG, "created a bundle");
// }

// JsonBundle_c::JsonBundle_c(const char *input)
// {
//     if (input)
//     {
//         jsonBuffer.parseObject(input);
//     }
//     else
//     {
//         jsonBuffer.createObject();
//     }
//     debugln(DEBUG, "created a bundle");
// }

void JsonBundle_s::CreateBundle()
{
    root = jsonBuffer.createObject();
}

void JsonBundle_s::CreateBundle(const char *input)
{

    root = jsonBuffer.parseObject(input);
}

JsonObject &JsonBundle_s::Root() const
{
    return root;
}

} // namespace wyp