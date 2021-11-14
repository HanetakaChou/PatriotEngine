

## backend
The "DriverAPI.inc" lists the abstract common interfaces, which is called the "Driver APIs", for all backends (OpenGL, Vulkan).

Logically, the Renderer calls the Driver APIs of the "DriverApi" which is the synonym for "CommandStream" as defined inside the "DriverApiForward.h".

Currently, the "#include "DriverAPI.inc"" is used by both the "CommandStream" and the backend //and the "ConcreteDispatcher"  

The macros "DECL_DRIVER_API", "DECL_DRIVER_API_RETURN" and "DECL_DRIVER_API_SYNCHRONOUS" should be defined before "#include "DriverAPI.inc"".  

The "#include "DriverAPI.inc"" will add all these interfaces as the member functions of the related class "CommandStream" or backend.

The "CommandStream.h" defines DECL_DRIVER_API

"allocateCommand" //circlebuffer

I don't praise this design //performance //DriverApi-CommandStream-backend  
// not able to tune of the API in a fine-grained way


// difficult to debug
// modify "CommandStream.h"  
```c++

// since we are invoking the destructor inside the "Command::execute", we use "std::aligned_storage" and "new" to prevent CXX invoking the destructor implicitly when we exit the function


#define DECL_DRIVER_API(methodName, paramsDecl, params)                                         \
    inline void methodName(paramsDecl) {                                                        \
        DEBUG_COMMAND_BEGIN(methodName, false, params);                                         \
        using Cmd = COMMAND_TYPE(methodName);                                                   \
        std::aligned_storage<sizeof(Cmd), alignof(Cmd)>::type p;                                \
        Cmd *base = new(&p) Cmd(mDispatcher->methodName##_, APPLY(std::move, params));          \
        DEBUG_COMMAND_END(methodName, false);                                                   \
        intptr_t next;                                                                          \
        Cmd::execute(&Driver::methodName, *mDriver, base, &next);                               \
    }

#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params)                                 \
    inline RetType methodName(paramsDecl) {                                                             \
        DEBUG_COMMAND_BEGIN(methodName, false, params);                                                 \
        RetType result = mDriver->methodName##S();                                                      \
        using Cmd = COMMAND_TYPE(methodName##R);                                                        \
        std::aligned_storage<sizeof(Cmd), alignof(Cmd)>::type p;                                        \
        Cmd *base = new(&p) Cmd(mDispatcher->methodName##_, RetType(result), APPLY(std::move, params)); \
        DEBUG_COMMAND_END(methodName, false);                                                           \
        intptr_t next;                                                                                  \
        Cmd::execute(&Driver::methodName##R, *mDriver, base, &next);                                    \
        return result;                                                                                  \
    }
```

// #define DECL_DRIVER_API(methodName, paramsDecl, params) DECL_DRIVER_API_SYNCHRONOUS(void, methodName, paramsDecl, params)  
// #define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params) DECL_DRIVER_API_SYNCHRONOUS(RetType, methodName, paramsDecl, params)
