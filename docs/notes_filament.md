

## backend

Logically, the Renderer calls the Driver APIs of the "DriverApi".  
However, these Driver APIs are not executed immediately but encoded as "Command"s which will be decoded and executed in another thread "FEngine::loop".  
Personally, I don't praise this design since we benefit nothing from the encoding and decoding, but introduce extra latency.  
Besides, this design makes debugging more difficult since we can't use the callstacks collected by the RenderDoc to analyze the FrameGraph of the Renderer.  
We can use following code to modify the macros "DECL_DRIVER_API" and "DECL_DRIVER_API_RETURN" inside the "CommandStream.h" to remove the encoding and decoding, and make the Driver APIs be executed immediately.

```c++
#define DECL_DRIVER_API(methodName, paramsDecl, params)                                         \
    inline void methodName(paramsDecl) {                                                        \
        DEBUG_COMMAND_BEGIN(methodName, false, params);                                         \
        AutoExecute callOnExit([=](){                                                           \
            DEBUG_COMMAND_END(methodName, true);                                                \
        });                                                                                     \
        using Cmd = COMMAND_TYPE(methodName);                                                   \
        std::aligned_storage<sizeof(Cmd), alignof(Cmd)>::type p;                                \
        CommandBase *base = new(&p) Cmd(mDispatcher->methodName##_, APPLY(std::move, params));  \
        base->execute(*mDriver);                                                                \
    }

#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params)                                         \
    inline RetType methodName(paramsDecl) {                                                                     \
        DEBUG_COMMAND_BEGIN(methodName, false, params);                                                         \
        AutoExecute callOnExit([=](){                                                                           \
            DEBUG_COMMAND_END(methodName, true);                                                                \
        });                                                                                                     \
        RetType result = mDriver->methodName##S();                                                              \
        using Cmd = COMMAND_TYPE(methodName##R);                                                                \
        std::aligned_storage<sizeof(Cmd), alignof(Cmd)>::type p;                                                \
        CommandBase *base = new(&p) Cmd(mDispatcher->methodName##_, RetType(result), APPLY(std::move, params)); \
        base->execute(*mDriver);                                                                                \
        return result;                                                                                          \
    }
```
