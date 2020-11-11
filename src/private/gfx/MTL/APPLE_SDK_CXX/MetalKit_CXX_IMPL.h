#ifndef _METALKIT_CXX_IMPL_H_
#define _METALKIT_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"
#include "Metal_CXX_IMPL.h"

#if __is_target_os(ios)
#include "UIKit_CXX_IMPL.h"
#elif __is_target_os(macos)
#include "AppKit_CXX_IMPL.h"
#else
#error Unknown Target
#endif

#if __is_target_os(ios)
struct MTKView : public UIView
{
    MTKView() = delete;
};
#elif __is_target_os(macos)
struct MTKView : public NSView
{
    MTKView() = delete;
};
#else
#error Unknown Target
#endif

struct CAMetalDrawable : public MTLDrawable
{
    CAMetalDrawable() = delete;
};

struct MTKViewDelegate_Class : public OBJC_CLASS
{
    MTKViewDelegate_Class() = delete;
};

struct MTKViewDelegate : public MTLDrawable
{
    MTKViewDelegate() = delete;
};

#endif
