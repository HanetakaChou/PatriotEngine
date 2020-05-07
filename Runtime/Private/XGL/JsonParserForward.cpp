#include "JsonParser.h"
#include "JsonFrontend.h"

static inline JsonStringRef wrap(std::string *x) { return reinterpret_cast<JsonStringRef>(x); }
static inline std::string *unwrap(JsonStringRef x) { return reinterpret_cast<std::string *>(x); }

extern "C" JsonStringRef JsonFrontend_CreateString(JsonFrontend *pUserData, char const *s)
{
    return wrap(pUserData->Callback_CreateString(s));
}

extern "C" JsonStringRef JsonFrontend_StringAppend2(JsonFrontend *pUserData, JsonStringRef l, JsonStringRef r)
{
    return wrap(pUserData->Callback_StringAppend(unwrap(l), unwrap(r)));
}

extern "C" JsonStringRef JsonFrontend_StringAppend3(JsonFrontend *pUserData, JsonStringRef l, char const *m, JsonStringRef r)
{
    return wrap(pUserData->Callback_StringAppend(unwrap(l), m, unwrap(r)));
}

extern "C" void MDLFrontend_DisposeString(JsonFrontend *pUserData, JsonStringRef s)
{
    return pUserData->Callback_DisposeString(unwrap(s));
}
