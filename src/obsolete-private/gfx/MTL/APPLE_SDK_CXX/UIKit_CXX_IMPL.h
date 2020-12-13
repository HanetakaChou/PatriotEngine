#ifndef _UIKIT_CXX_IMPL_H_
#define _UIKIT_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"

struct UIScreen : public NSObject
{
    UIScreen() = delete;
};

struct UIWindow : public NSObject
{
    UIWindow() = delete;
};

struct UIView : public NSObject
{
    UIView() = delete;
};

struct UIViewController_Class : OBJC_CLASS
{
    UIViewController_Class() = delete;
};

struct UIViewController : public NSObject
{
    UIViewController() = delete;
};

struct UIApplicationDelegate : public NSObject
{
    UIApplicationDelegate() = delete;
};

struct UIApplication : public NSObject
{
    UIApplication() = delete;
};


#endif