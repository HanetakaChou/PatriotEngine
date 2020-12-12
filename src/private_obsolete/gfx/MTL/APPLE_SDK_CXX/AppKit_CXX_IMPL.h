#ifndef _APPKIT_CXX_IMPL_H_
#define _APPKIT_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"

struct NSScreen : public NSObject
{
    NSScreen() = delete;
};

struct NSWindow : public NSObject
{
    NSWindow() = delete;
};

struct NSView : public NSObject
{
    NSView() = delete;
};

struct NSViewController_Class : OBJC_CLASS
{
    NSViewController_Class() = delete;
};

struct NSViewController : public NSObject
{
    NSViewController() = delete;
};

struct NSApplicationDelegate : public NSObject
{
    NSApplicationDelegate() = delete;
};

struct NSApplication : public NSObject
{
    NSApplication() = delete;
};

#endif
