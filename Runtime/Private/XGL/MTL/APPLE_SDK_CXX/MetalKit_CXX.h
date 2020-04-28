#ifndef _METALKIT_CXX_H_
#define _METALKIT_CXX_H_ 1

#include "Metal_CXX.h"

#if __is_target_os(ios)
#include "UIKit_CXX.h"
#elif __is_target_os(macos)
#include "AppKit_CXX.h"
#else
#error Unknown Target
#endif

struct MTKView *MTKView_alloc();

struct MTKView *MTKView_initWithFrame(struct MTKView *self, CGRect frameRect, struct MTLDevice *device);

struct MTKViewDelegate_Class *MTKViewDelegate_allocClass(
    char const *classname,
    void (*_I_MTKViewDelegate_drawableSizeWillChange_)(struct MTKViewDelegate *, struct MTKViewDelegate_drawableSizeWillChange_ *, struct MTKView *view, CGSize size),
    void (*_I_MTKViewDelegate_drawInMTKView_)(struct MTKViewDelegate *, struct MTKViewDelegate_drawInMTKView_ *, struct MTKView *view));

bool MTKViewDelegate_Class_addIvarVoidPointer(struct MTKViewDelegate_Class *self, char const *ivarname);

struct MTKViewDelegate *MTKViewDelegate_alloc(struct MTKViewDelegate_Class *);

struct MTKViewDelegate *MTKViewDelegate_init(struct MTKViewDelegate *self);

void MTKViewDelegate_setIvarVoidPointer(struct MTKViewDelegate *self, char const *ivarname, void *pVoid);

void *MTKViewDelegate_getIvarVoidPointer(struct MTKViewDelegate *self, char const *ivarname);

void MTKView_setDelegate(struct MTKView *self, struct MTKViewDelegate *delegate);

void MTKView_setColorPixelFormat(struct MTKView *self, MTLPixelFormat colorPixelFormat);

void MTKView_setDepthStencilPixelFormat(struct MTKView *self, MTLPixelFormat depthStencilPixelFormat);

void MTKView_setSampleCount(struct MTKView *self, NSUInteger sampleCount);

struct MTLRenderPassDescriptor *MTKView_currentRenderPassDescriptor(struct MTKView *self);

struct CAMetalDrawable *MTKView_currentDrawable(struct MTKView *self);

#if __is_target_os(ios)
void UIViewController_setView(struct UIViewController *self, struct MTKView *view);
#elif __is_target_os(macos)
void NSViewController_setView(struct NSViewController *self, struct MTKView *view);
#else
#error Unknown Target
#endif

void MTLCommandBuffer_presentDrawable(struct MTLCommandBuffer *self, struct CAMetalDrawable *drawable);

#endif
