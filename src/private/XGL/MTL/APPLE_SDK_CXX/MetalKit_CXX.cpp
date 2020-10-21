#include "MetalKit_CXX.h"
#include "MetalKit_CXX_IMPL.h"

#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

#include <assert.h>

struct MTKView *MTKView_alloc()
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTKView"),
        sel_registerName("alloc"));
    return static_cast<struct MTKView *>(view);
}

struct MTKView *MTKView_initWithFrame(MTKView *self, CGRect frameRect, struct MTLDevice *device)
{
    struct objc_object *view = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, CGRect, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("initWithFrame:device:"),
        frameRect,
        device);
    return static_cast<struct MTKView *>(view);
}

struct MTKViewDelegate_Class *MTKViewDelegate_allocClass(
    char const *classname,
    void (*_I_MTKViewDelegate_drawableSizeWillChange_)(struct MTKViewDelegate *, struct MTKViewDelegate_drawableSizeWillChange_ *, struct MTKView *view, CGSize size),
    void (*_I_MTKViewDelegate_drawInMTKView_)(struct MTKViewDelegate *, struct MTKViewDelegate_drawInMTKView_ *, struct MTKView *view))
{
    Class class_MTKViewDelegate_CXX;
    {
        class_MTKViewDelegate_CXX = objc_allocateClassPair(
            objc_getClass("NSObject"),
            classname,
            0);
        assert(class_MTKViewDelegate_CXX != NULL);

        BOOL result_1 = class_addMethod(
            class_MTKViewDelegate_CXX,
            sel_registerName("mtkView:drawableSizeWillChange:"),
            reinterpret_cast<IMP>(_I_MTKViewDelegate_drawableSizeWillChange_),
            "v@:@{CGSize=dd}");
        assert(result_1 != NO);

        BOOL result_2 = class_addMethod(
            class_MTKViewDelegate_CXX,
            sel_registerName("drawInMTKView:"),
            reinterpret_cast<IMP>(_I_MTKViewDelegate_drawInMTKView_),
            "v@:@");
        assert(result_2 != NO);
    }

    return reinterpret_cast<struct MTKViewDelegate_Class *>(class_MTKViewDelegate_CXX);
}

bool MTKViewDelegate_Class_addIvarVoidPointer(struct MTKViewDelegate_Class *self, char const *ivarname)
{
    return OBJC_CLASS_addIvarVoidPointer(self, ivarname);
}

struct MTKViewDelegate *MTKViewDelegate_alloc(struct MTKViewDelegate_Class *class_MTKViewDelegate_CXX)
{
    struct objc_object *delegate = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        reinterpret_cast<Class>(class_MTKViewDelegate_CXX),
        sel_registerName("alloc"));

    return static_cast<struct MTKViewDelegate *>(delegate);
}

struct MTKViewDelegate *MTKViewDelegate_init(struct MTKViewDelegate *self)
{
    struct objc_object *delegate = NSObject_init(self);
    return static_cast<struct MTKViewDelegate *>(delegate);
}

void MTKViewDelegate_setIvarVoidPointer(struct MTKViewDelegate *self, char const *ivarname, void *pVoid)
{
    return OBJC_OBJECT_setIvarVoidPointer(self, ivarname, pVoid);
}

void *MTKViewDelegate_getIvarVoidPointer(struct MTKViewDelegate *self, char const *ivarname)
{
    return OBJC_OBJECT_getIvarVoidPointer(self, ivarname);
}

void MTKView_setDelegate(struct MTKView *self, struct MTKViewDelegate *delegate)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setDelegate:"),
        delegate);
}

void MTKView_setColorPixelFormat(struct MTKView *self, MTLPixelFormat colorPixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setColorPixelFormat:"),
        colorPixelFormat);
}

void MTKView_setDepthStencilPixelFormat(struct MTKView *self, MTLPixelFormat depthStencilPixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setDepthStencilPixelFormat:"),
        depthStencilPixelFormat);
}

void MTKView_setSampleCount(struct MTKView *self, NSUInteger sampleCount)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setSampleCount:"),
        sampleCount);
}

struct MTLRenderPassDescriptor *MTKView_currentRenderPassDescriptor(struct MTKView *self)
{
    struct objc_object *renderpassdescriptor = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("currentRenderPassDescriptor"));
    return static_cast<struct MTLRenderPassDescriptor *>(renderpassdescriptor);
}

struct CAMetalDrawable *MTKView_currentDrawable(struct MTKView *self)
{
    struct objc_object *drawable = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("currentDrawable"));
    return static_cast<struct CAMetalDrawable *>(drawable);
}

#if __is_target_os(ios)
void UIViewController_setView(struct UIViewController *self, struct MTKView *view)
{
    return UIViewController_setView(self, static_cast<struct UIView *>(view));
}
#elif __is_target_os(macos)
void NSViewController_setView(struct NSViewController *self, struct MTKView *view)
{
    return NSViewController_setView(self, static_cast<struct NSView *>(view));
}
#else
#error Unknown Target
#endif

void MTLCommandBuffer_presentDrawable(struct MTLCommandBuffer *self, struct CAMetalDrawable *drawable)
{
    return MTLCommandBuffer_presentDrawable(self, static_cast<MTLDrawable *>(drawable));
}
