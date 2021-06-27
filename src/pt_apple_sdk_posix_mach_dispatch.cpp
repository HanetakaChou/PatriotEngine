/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <pt_apple_sdk_posix_mach_objc.h>
#include <pt_apple_sdk_posix_mach_dispatch.h>

#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include <objc/message.h>
#include <objc/runtime.h>
#elif TARGET_OS_OSX
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

extern "C" __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) struct _DISPATCHQUEUE_T_ _dispatch_main_q;

PT_ATTR_APPLE_SDK dispatch_queue_t dispatch_get_main_queue(void)
{
    return &_dispatch_main_q;
}

extern "C" __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) struct _DISPATCHSOURCETYPE_T_ _dispatch_source_type_data_add;

PT_ATTR_APPLE_SDK dispatch_source_type_t dispatch_get_source_type_data_add()
{
    return &_dispatch_source_type_data_add;
}

extern "C" dispatch_source_t __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_source_create(dispatch_source_type_t type, uintptr_t handle, uintptr_t mask, dispatch_queue_t queue);

PT_ATTR_APPLE_SDK dispatch_source_t dispatch_create_source(dispatch_source_type_t type, uintptr_t handle, uintptr_t mask, dispatch_queue_t queue)
{
    return dispatch_source_create(type, handle, mask, queue);
}

// Block Implementation Specification
// https://clang.llvm.org/docs/Block-ABI-Apple.html

// This document describes the Apple ABI implementation specification of Blocks.
// The first shipping version of this ABI is found in Mac OS X 10.6, and shall be referred to as 10.6.ABI. As of 2010/3/16, the following describes the ABI contract with
// the runtime and the compiler, and, as necessary, will be referred to as ABI.2010.3.16.
// Since the Apple ABI references symbols from other elements of the system, any attempt to use this ABI on systems prior to SnowLeopard is undefined.

// High Level

// The ABI of Blocks consist of their layout and the runtime functions required by the compiler. A Block consists of a structure of the following form:
// struct Block_literal_1
// {
//     void *isa; // initialized to &_NSConcreteStackBlock or &_NSConcreteGlobalBlock
//     int flags;
//     int reserved;
//     void (*invoke)(void *, ...);
//     struct Block_descriptor_1
//     {
//         unsigned long int reserved; // NULL
//         unsigned long int size;     // sizeof(struct Block_literal_1)
//         // optional helper functions
//         void (*copy_helper)(void *dst, void *src); // IFF (1<<25)
//         void (*dispose_helper)(void *src);         // IFF (1<<25)
//         // required ABI.2010.3.16
//         const char *signature; // IFF (1<<30)
//     } * descriptor;
//     // imported variables
// };

// extern "C" void *_NSConcreteGlobalBlock[32];
extern "C" void *_NSConcreteStackBlock[32];

// The following flags bits are in use thusly for a possible ABI.2010.3.16:
enum
{
    // Set to true on blocks that have captures (and thus are not true
    // global blocks) but are known not to escape for various other
    // reasons. For backward compatibility with old runtimes, whenever
    // BLOCK_IS_NOESCAPE is set, BLOCK_IS_GLOBAL is set too. Copying a
    // non-escaping block returns the original block and releasing such a
    // block is a no-op, which is exactly how global blocks are handled.
    BLOCK_IS_NOESCAPE = (1 << 23),

    BLOCK_HAS_COPY_DISPOSE = (1 << 25),
    BLOCK_HAS_CTOR = (1 << 26), // helpers have C++ code
    BLOCK_IS_GLOBAL = (1 << 28),
    BLOCK_HAS_STRET = (1 << 29), // IFF BLOCK_HAS_SIGNATURE
    BLOCK_HAS_SIGNATURE = (1 << 30),
};

// In 10.6.ABI the (BLOCK_HAS_STRET) was usually set and was always ignored by the runtime - it had been a transitional marker that did not get deleted after the transition. This
// bit is now paired with (BLOCK_HAS_SIGNATURE), and represented as the pair (BLOCK_HAS_SIGNATURE|BLOCK_HAS_STRET), for the following combinations of valid bit settings, and their meanings:
// switch (flags & (3<<29)) {
//  case (0):                                   10.6.ABI, no signature field available
//  case (BLOCK_HAS_STRET):                     10.6.ABI, no signature field available
//  case (BLOCK_HAS_SIGNATURE):                 ABI.2010.3.16, regular calling convention, presence of signature field
//  case (BLOCK_HAS_SIGNATURE|BLOCK_HAS_STRET): ABI.2010.3.16, stret calling convention, presence of signature field,
// }

// The signature field is not always populated.

// The following discussions are presented as 10.6.ABI otherwise.

// Block literals may occur within functions where the structure is created in stack local memory. They may also appear as initialization expressions for Block variables
// of global or static local variables.

// When a Block literal expression is evaluated the stack based structure is initialized as follows:

// 1. A static descriptor structure is declared and initialized as follows:
//      a. The invoke function pointer is set to a function that takes the Block structure as its first argument and the rest of the arguments (if any) to the Block
//      and executes the Block compound statement.
//      b. The size field is set to the size of the following Block literal structure.
//      c. The copy_helper and dispose_helper function pointers are set to respective helper functions if they are required by the Block literal.

// 2. A stack (or global) Block literal data structure is created and initialized as follows:
//      a. The isa field is set to the address of the external _NSConcreteStackBlock, which is a block of uninitialized memory supplied in libSystem, or
//      _NSConcreteGlobalBlock if this is a static or file level Block literal.
//      b. The flags field is set to zero unless there are variables imported into the Block that need helper functions for program level Block_copy() and
//      Block_release() operations, in which case the (1<<25) flags bit is set.

// As an example, the Block literal expression:
//
// ^ { printf("hello world\n"); }
//
// would cause the following to be created on a 32-bit system:
//
// struct __block_literal_1 {
//     void *isa;
//     int flags;
//     int reserved;
//     void (*invoke)(struct __block_literal_1 *);
//     struct __block_descriptor_1 *descriptor;
// };
//
// void __block_invoke_1(struct __block_literal_1 *_block) {
//     printf("hello world\n");
// }
//
// static struct __block_descriptor_1 {
//     unsigned long int reserved;
//     unsigned long int Block_size;
// } __block_descriptor_1 = { 0, sizeof(struct __block_literal_1) };

// and where the Block literal itself appears:
//
// struct __block_literal_1 _block_literal = {
//      &_NSConcreteStackBlock,
//      BLOCK_HAS_STRET,
//      <uninitialized>,
//      __block_invoke_1,
//      &__block_descriptor_1
// };

// A Block imports other Block references, const copies of other variables, and variables marked __block. In Objective-C, variables may additionally be objects.
// When a Block literal expression is used as the initial value of a global or static local variable, it is initialized as follows:
//
// struct __block_literal_1 __block_literal_1 = {
//       &_NSConcreteGlobalBlock,
//       (1<<28)|(1<<29),
//       BLOCK_IS_GLOBAL|BLOCK_HAS_STRET,
//       __block_invoke_1,
//      &__block_descriptor_1
// };
//
// that is, a different address is provided as the first value and a particular (BLOCK_IS_GLOBAL) bit is set in the flags field, and otherwise it is the same as for stack based Block literals. This is an optimization that can be used for any Block
// literal that imports no const or __block storage variables.

// Imported Variables

// Variables of auto storage class are imported as const copies. Variables of __block storage class are imported as a pointer to an enclosing data structure. Global
// variables are simply referenced and not considered as imported.

// Imported const copy variables
// Automatic storage variables not marked with __block are imported as const copies.

// The simplest example is that of importing a variable of type int:
//
// int x = 10;
// void (^vv)(void) = ^{ printf("x is %d\n", x); }
// x = 11;

// which would be compiled to:
//
// struct __block_literal_2 {
//     void *isa;
//     int flags;
//     int reserved;
//     void (*invoke)(struct __block_literal_2 *);
//     struct __block_descriptor_2 *descriptor;
//     const int x;
// };
//
// void __block_invoke_2(struct __block_literal_2 *_block) {
//     printf("x is %d\n", _block->x);
// }
//
// static struct __block_descriptor_2 {
//     unsigned long int reserved;
//     unsigned long int Block_size;
// } __block_descriptor_2 = { 0, sizeof(struct __block_literal_2) };

// and:
// struct __block_literal_2 __block_literal_2 = {
//       &_NSConcreteStackBlock,
//       BLOCK_HAS_STRET,
//       <uninitialized>,
//       __block_invoke_2,
//       &__block_descriptor_2,
//       x
// };

// In summary, scalars, structures, unions, and function pointers are generally imported as const copies with no need for helper functions.

// Memory Management Policy
// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmRules.html

// Objective-C Automatic Reference Counting (ARC)
// https://clang.llvm.org/docs/AutomaticReferenceCounting.html

// Using Autorelease Pool Blocks
// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html

extern "C" void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_source_set_event_handler(dispatch_source_t source, struct objc_object *handler);

PT_ATTR_APPLE_SDK void dispatch_set_source_event_handler(dispatch_source_t source, void (*pfnCallback)(void *pUserData), void *pUserData)
{
    struct __block_literal_t
    {
        void *isa;
        int flags;
        int reserved;
        void (*invoke)(struct __block_literal_t *);
        void *descriptor;
        void (*pfnCallback)(void *pUserData);
        void *pUserData;
    };

    static void (*const __block_invoke)(struct __block_literal_t *) = [](struct __block_literal_t *_block) -> void
    {
        _block->pfnCallback(_block->pUserData);
    };

    static struct
    {
        unsigned long int reserved;
        unsigned long int Block_size;
    } __block_descriptor = {0, sizeof(struct __block_literal_t)};

    struct __block_literal_t _block = {
        &_NSConcreteStackBlock,
        BLOCK_HAS_STRET,
        0, // uninitialized
        __block_invoke,
        &__block_descriptor,
        pfnCallback,
        pUserData};
    return dispatch_source_set_event_handler(source, reinterpret_cast<struct objc_object *>(&_block));
}

extern "C" void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_resume(dispatch_source_t source);

PT_ATTR_APPLE_SDK void dispatch_resume_source(dispatch_source_t source)
{
    return dispatch_resume(source);
}

extern "C" void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_source_merge_data(dispatch_source_t source, uintptr_t value);

PT_ATTR_APPLE_SDK void dispatch_merge_source_data(dispatch_source_t source, uintptr_t value)
{
    return dispatch_source_merge_data(source, value);
}
