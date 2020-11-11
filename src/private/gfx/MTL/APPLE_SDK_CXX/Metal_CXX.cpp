#include "Metal_CXX.h"
#include "Dispatch_CXX_IMPL.h"
#include "Metal_CXX_IMPL.h"

#if __is_target_os(ios)
#include <objc/message.h>
#include <objc/runtime.h>
#elif __is_target_os(macos)
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

bool MTLDevice_supportsFeatureSet(struct MTLDevice *self, MTLFeatureSet featureSet)
{
    BOOL supportFeatureSet = reinterpret_cast<BOOL (*)(struct objc_object *, struct objc_selector *, MTLFeatureSet)>(objc_msgSend)(
        self,
        sel_registerName("supportsFeatureSet:"),
        featureSet);
    return (supportFeatureSet != NO) ? true : false;
}

struct MTLVertexDescriptor *MTLVertexDescriptor_alloc()
{
    struct objc_object *vertexdescriptor = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTLVertexDescriptor"),
        sel_registerName("alloc"));
    return static_cast<struct MTLVertexDescriptor *>(vertexdescriptor);
}

struct MTLVertexDescriptor *MTLVertexDescriptor_init(struct MTLVertexDescriptor *self)
{
    struct NSObject *pipelinedescriptor = NSObject_init(self);
    return static_cast<struct MTLVertexDescriptor *>(pipelinedescriptor);
}

struct MTLVertexBufferLayoutDescriptorArray *MTLVertexDescriptor_layouts(struct MTLVertexDescriptor *self)
{
    struct objc_object *vertexdescriptor_layouts = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("layouts"));
    return static_cast<struct MTLVertexBufferLayoutDescriptorArray *>(vertexdescriptor_layouts);
}

struct MTLVertexBufferLayoutDescriptor *MTLVertexBufferLayoutDescriptorArray_objectAtIndexedSubscript(struct MTLVertexBufferLayoutDescriptorArray *self, NSUInteger index)
{
    struct objc_object *vertexdescriptor_layout = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("objectAtIndexedSubscript:"),
        index);
    return static_cast<struct MTLVertexBufferLayoutDescriptor *>(vertexdescriptor_layout);
}

void MTLVertexBufferLayoutDescriptor_setStride(struct MTLVertexBufferLayoutDescriptor *self, NSUInteger stride)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setStride:"),
        stride);
}

void MTLVertexBufferLayoutDescriptor_setStepFunction(struct MTLVertexBufferLayoutDescriptor *self, MTLVertexStepFunction stepFunction)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setStepFunction:"),
        stepFunction);
}

void MTLVertexBufferLayoutDescriptor_setStepRate(struct MTLVertexBufferLayoutDescriptor *self, NSUInteger stepRate)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setStepRate:"),
        stepRate);
}

struct MTLVertexAttributeDescriptorArray *MTLVertexDescriptor_attributes(struct MTLVertexDescriptor *self)
{
    struct objc_object *vertexdescriptor_attributes = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("attributes"));
    return static_cast<struct MTLVertexAttributeDescriptorArray *>(vertexdescriptor_attributes);
}

struct MTLVertexAttributeDescriptor *MTLVertexAttributeDescriptorArray_objectAtIndexedSubscript(struct MTLVertexAttributeDescriptorArray *self, NSUInteger index)
{
    struct objc_object *vertexdescriptor_attribute = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("objectAtIndexedSubscript:"),
        index);
    return static_cast<struct MTLVertexAttributeDescriptor *>(vertexdescriptor_attribute);
}

void MTLVertexAttributeDescriptor_setFormat(struct MTLVertexAttributeDescriptor *self, MTLVertexFormat format)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setFormat:"),
        format);
}

void MTLVertexAttributeDescriptor_setOffset(struct MTLVertexAttributeDescriptor *self, NSUInteger offset)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setOffset:"),
        offset);
}

void MTLVertexAttributeDescriptor_setBufferIndex(struct MTLVertexAttributeDescriptor *self, NSUInteger bufferIndex)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setBufferIndex:"),
        bufferIndex);
}

void MTLVertexDescriptor_release(struct MTLVertexDescriptor *self)
{
    return NSObject_release(self);
}

NSUInteger MTLVertexDescriptor_retainCount(struct MTLVertexDescriptor *self)
{
    return NSObject_retainCount(self);
}

struct MTLRenderPipelineDescriptor *MTLRenderPipelineDescriptor_alloc()
{
    struct objc_object *pipelinedescriptor = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTLRenderPipelineDescriptor"),
        sel_registerName("alloc"));
    return reinterpret_cast<struct MTLRenderPipelineDescriptor *>(pipelinedescriptor);
}

struct MTLRenderPipelineDescriptor *MTLRenderPipelineDescriptor_init(struct MTLRenderPipelineDescriptor *self)
{
    struct NSObject *pipelinedescriptor = NSObject_init(self);
    return static_cast<struct MTLRenderPipelineDescriptor *>(pipelinedescriptor);
}

void MTLRenderPipelineDescriptor_setLabel(struct MTLRenderPipelineDescriptor *self, struct NSString *label)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setLabel:"),
        label);
}

void MTLRenderPipelineDescriptor_setVertexFunction(struct MTLRenderPipelineDescriptor *self, struct MTLFunction *vertexFunction)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setVertexFunction:"),
        vertexFunction);
}

void MTLRenderPipelineDescriptor_setFragmentFunction(struct MTLRenderPipelineDescriptor *self, struct MTLFunction *fragmentFunction)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setFragmentFunction:"),
        fragmentFunction);
}

void MTLRenderPipelineDescriptor_setVertexDescriptor(struct MTLRenderPipelineDescriptor *self, struct MTLVertexDescriptor *vertexDescriptor)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setVertexDescriptor:"),
        vertexDescriptor);
}

void MTLRenderPipelineDescriptor_setSampleCount(struct MTLRenderPipelineDescriptor *self, NSUInteger sampleCount)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setSampleCount:"),
        sampleCount);
}

struct MTLRenderPipelineColorAttachmentDescriptorArray *MTLRenderPipelineDescriptor_colorAttachments(struct MTLRenderPipelineDescriptor *self)
{
    struct objc_object *renderpipelinedescriptor_colorattachments = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("colorAttachments"));
    return static_cast<struct MTLRenderPipelineColorAttachmentDescriptorArray *>(renderpipelinedescriptor_colorattachments);
}

struct MTLRenderPipelineColorAttachmentDescriptor *MTLRenderPipelineColorAttachmentDescriptorArray_objectAtIndexedSubscript(struct MTLRenderPipelineColorAttachmentDescriptorArray *self, NSUInteger attachmentIndex)
{
    struct objc_object *renderpipelinedescriptor_colorattachment = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("objectAtIndexedSubscript:"),
        attachmentIndex);
    return static_cast<struct MTLRenderPipelineColorAttachmentDescriptor *>(renderpipelinedescriptor_colorattachment);
}

void MTLRenderPipelineColorAttachmentDescriptor_setPixelFormat(struct MTLRenderPipelineColorAttachmentDescriptor *self, MTLPixelFormat pixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setPixelFormat:"),
        pixelFormat);
}

void MTLRenderPipelineDescriptor_setDepthAttachmentPixelFormat(struct MTLRenderPipelineDescriptor *self, MTLPixelFormat pixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setDepthAttachmentPixelFormat:"),
        pixelFormat);
}

void MTLRenderPipelineDescriptor_setStencilAttachmentPixelFormat(struct MTLRenderPipelineDescriptor *self, MTLPixelFormat pixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setStencilAttachmentPixelFormat:"),
        pixelFormat);
}

void MTLRenderPipelineDescriptor_release(struct MTLRenderPipelineDescriptor *self)
{
    return NSObject_release(self);
}

NSUInteger MTLRenderPipelineDescriptor_retainCount(struct MTLRenderPipelineDescriptor *self)
{
    return NSObject_retainCount(self);
}

struct MTLRenderPipelineState *MTLDevice_newRenderPipelineStateWithDescriptor(struct MTLDevice *self, struct MTLRenderPipelineDescriptor *descriptor, struct NSError **error)
{
    struct objc_object *pipelinestate = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *, struct objc_object **)>(objc_msgSend)(
        self,
        sel_registerName("newRenderPipelineStateWithDescriptor:error:"),
        descriptor,
        reinterpret_cast<struct objc_object **>(error));
    return static_cast<MTLRenderPipelineState *>(pipelinestate);
}

struct MTLDepthStencilDescriptor *MTLDepthStencilDescriptor_alloc()
{
    struct objc_object *pipelinedescriptor = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTLDepthStencilDescriptor"),
        sel_registerName("alloc"));
    return static_cast<struct MTLDepthStencilDescriptor *>(pipelinedescriptor);
}

struct MTLDepthStencilDescriptor *MTLDepthStencilDescriptor_init(struct MTLDepthStencilDescriptor *self)
{
    struct NSObject *depthstencildescriptor = NSObject_init(self);
    return static_cast<struct MTLDepthStencilDescriptor *>(depthstencildescriptor);
}

void MTLDepthStencilDescriptor_setDepthCompareFunction(struct MTLDepthStencilDescriptor *self, MTLCompareFunction compareFunction)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setDepthCompareFunction:"),
        compareFunction);
}

void MTLDepthStencilDescriptor_setDepthWriteEnabled(struct MTLDepthStencilDescriptor *self, bool depthWriteEnabled)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, BOOL)>(objc_msgSend)(
        self,
        sel_registerName("setDepthWriteEnabled:"),
        (depthWriteEnabled != false) ? YES : NO);
}

void MTLDepthStencilDescriptor_release(struct MTLDepthStencilDescriptor *self)
{
    return NSObject_release(self);
}

NSUInteger MTLDepthStencilDescriptor_retainCount(struct MTLDepthStencilDescriptor *self)
{
    return NSObject_retainCount(self);
}

struct MTLDepthStencilState *MTLDevice_newDepthStencilStateWithDescriptor(struct MTLDevice *self, struct MTLDepthStencilDescriptor *descriptor)
{
    struct objc_object *depthstencilstate = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("newDepthStencilStateWithDescriptor:"),
        descriptor);
    return static_cast<struct MTLDepthStencilState *>(depthstencilstate);
}

void MTLResource_setLabel(struct MTLResource *self, struct NSString *label)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setLabel:"),
        label);
}

struct MTLBuffer *MTLDevice_newBufferWithLength(struct MTLDevice *self, NSUInteger length, MTLResourceOptions options)
{
    struct objc_object *buffer = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger, MTLResourceOptions)>(objc_msgSend)(
        self,
        sel_registerName("newBufferWithLength:options:"),
        length,
        options);
    return static_cast<struct MTLBuffer *>(buffer);
}

void MTLBuffer_setLabel(struct MTLBuffer *self, struct NSString *label)
{
    return MTLResource_setLabel(self, label);
}

void *MTLBuffer_contents(struct MTLBuffer *self)
{
    void *contents = reinterpret_cast<void *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("contents"));
    return contents;
}

void MTLBuffer_release(struct MTLBuffer *self)
{
    return NSObject_release(self);
}

NSUInteger MTLBuffer_retainCount(struct MTLBuffer *self)
{
    return NSObject_retainCount(self);
}

struct MTLTextureDescriptor *MTLTextureDescriptor_alloc()
{
    struct objc_object *texturedescriptor = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTLTextureDescriptor"),
        sel_registerName("alloc"));
    return static_cast<struct MTLTextureDescriptor *>(texturedescriptor);
}

struct MTLTextureDescriptor *MTLTextureDescriptor_init(struct MTLTextureDescriptor *self)
{
    struct objc_object *texturedescriptor = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("init"));
    return static_cast<struct MTLTextureDescriptor *>(texturedescriptor);
}

void MTLTextureDescriptor_setTextureType(struct MTLTextureDescriptor *self, MTLTextureType textureType)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLTextureType)>(objc_msgSend)(
        self,
        sel_registerName("setTextureType:"),
        textureType);
}

void MTLTextureDescriptor_setPixelFormat(struct MTLTextureDescriptor *self, MTLPixelFormat pixelFormat)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPixelFormat)>(objc_msgSend)(
        self,
        sel_registerName("setPixelFormat:"),
        pixelFormat);
}

void MTLTextureDescriptor_setWidth(struct MTLTextureDescriptor *self, NSUInteger width)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setWidth:"),
        width);
}

void MTLTextureDescriptor_setHeight(struct MTLTextureDescriptor *self, NSUInteger height)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setHeight:"),
        height);
}

void MTLTextureDescriptor_setDepth(struct MTLTextureDescriptor *self, NSUInteger depth)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setDepth:"),
        depth);
}

void MTLTextureDescriptor_setMipmapLevelCount(struct MTLTextureDescriptor *self, NSUInteger mipmapLevelCount)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setMipmapLevelCount:"),
        mipmapLevelCount);
}

void MTLTextureDescriptor_setSampleCount(struct MTLTextureDescriptor *self, NSUInteger sampleCount)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setSampleCount:"),
        sampleCount);
}

void MTLTextureDescriptor_setArrayLength(struct MTLTextureDescriptor *self, NSUInteger arrayLength)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setArrayLength:"),
        arrayLength);
}

void MTLTextureDescriptor_setResourceOptions(struct MTLTextureDescriptor *self, MTLResourceOptions resourceOptions)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLResourceOptions)>(objc_msgSend)(
        self,
        sel_registerName("setResourceOptions:"),
        resourceOptions);
}

void MTLTextureDescriptor_setUsage(struct MTLTextureDescriptor *self, MTLTextureUsage usage)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLTextureUsage)>(objc_msgSend)(
        self,
        sel_registerName("setUsage:"),
        usage);
}

void MTLTextureDescriptor_release(struct MTLTextureDescriptor *self)
{
    return NSObject_release(self);
}

NSUInteger MTLTextureDescriptor_retainCount(struct MTLTextureDescriptor *self)
{
    return NSObject_retainCount(self);
}

struct MTLTexture *MTLDevice_newTextureWithDescriptor(struct MTLDevice *self, struct MTLTextureDescriptor *descriptor)
{
    struct objc_object *texture = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("newTextureWithDescriptor:"),
        descriptor);
    return static_cast<struct MTLTexture *>(texture);
}

void MTLTexture_setLabel(struct MTLTexture *self, struct NSString *label)
{
    return MTLResource_setLabel(self, label);
}

void MTLTexture_release(struct MTLTexture *self)
{
    return NSObject_release(self);
}

NSUInteger MTLTexture_retainCount(struct MTLTexture *self)
{
    return NSObject_retainCount(self);
}

struct MTLCommandQueue *MTLDevice_newCommandQueue(struct MTLDevice *self)
{
    struct objc_object *commandQueue = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("newCommandQueue"));
    return static_cast<struct MTLCommandQueue *>(commandQueue);
}

struct MTLCommandBuffer *MTLCommandQueue_commandBuffer(struct MTLCommandQueue *self)
{
    struct objc_object *commandBuffer = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("commandBuffer"));
    return static_cast<struct MTLCommandBuffer *>(commandBuffer);
}

void MTLCommandBuffer_setLabel(struct MTLCommandBuffer *self, struct NSString *label)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setLabel:"),
        label);
}

struct __block_literal_MTLCommandBufferHandler
{
    void *isa;
    int flags;
    int reserved;
    void (*invoke)(struct __block_literal_MTLCommandBufferHandler *, struct MTLCommandBuffer *buffer);
    struct __block_descriptor_MTLCommandBufferHandler *descriptor;
    void *pUserData;
    NSUInteger throttlingIndex;
    void (*pfnCallback)(void *, NSUInteger, struct MTLCommandBuffer *);
};

static void __block_invoke_MTLCommandBufferHandler(struct __block_literal_MTLCommandBufferHandler *_block, struct MTLCommandBuffer *buffer)
{
    _block->pfnCallback(_block->pUserData, _block->throttlingIndex, buffer);
}

static struct __block_descriptor_MTLCommandBufferHandler
{
    unsigned long int reserved;
    unsigned long int Block_size;
} __block_descriptor_MTLCommandBufferHandler = {0, sizeof(struct __block_literal_MTLCommandBufferHandler)};

void MTLCommandBuffer_addCompletedHandler(struct MTLCommandBuffer *self, void *pUserData, NSUInteger throttlingIndex, void (*pfnCallback)(void *, NSUInteger, struct MTLCommandBuffer *))
{
    struct __block_literal_MTLCommandBufferHandler __block_literal_MTLCommandBufferHandler = {
        &_NSConcreteStackBlock,
        BLOCK_HAS_STRET,
        0, // uninitialized
        __block_invoke_MTLCommandBufferHandler,
        &__block_descriptor_MTLCommandBufferHandler,
        pUserData,
        throttlingIndex,
        pfnCallback};

    reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("addCompletedHandler:"),
        reinterpret_cast<struct objc_object *>(&__block_literal_MTLCommandBufferHandler));
}

void MTLCommandBuffer_presentDrawable(struct MTLCommandBuffer *self, struct MTLDrawable *drawable)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("presentDrawable:"),
        drawable);
}

void MTLCommandBuffer_commit(struct MTLCommandBuffer *self)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("commit"));
}

NSUInteger MTLCommandBuffer_retainCount(struct MTLCommandBuffer *self)
{
    return NSObject_retainCount(self);
}

void MTLCommandEncoder_setLabel(struct MTLCommandEncoder *self, struct NSString *label)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setLabel:"),
        label);
}

void MTLCommandEncoder_pushDebugGroup(struct MTLCommandEncoder *self, struct NSString *string)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("pushDebugGroup:"),
        string);
}

void MTLCommandEncoder_popDebugGroup(struct MTLCommandEncoder *self)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("popDebugGroup"));
}

void MTLCommandEncoder_endEncoding(struct MTLCommandEncoder *self)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("endEncoding"));
}

void MTLRenderCommandEncoder_setLabel(struct MTLRenderCommandEncoder *self, struct NSString *label)
{
    return MTLCommandEncoder_setLabel(self, label);
}

void MTLRenderCommandEncoder_pushDebugGroup(struct MTLRenderCommandEncoder *self, struct NSString *string)
{
    return MTLCommandEncoder_pushDebugGroup(self, string);
}

void MTLRenderCommandEncoder_popDebugGroup(struct MTLRenderCommandEncoder *self)
{
    return MTLCommandEncoder_popDebugGroup(self);
}

struct MTLRenderCommandEncoder *MTLCommandBuffer_renderCommandEncoderWithDescriptor(struct MTLCommandBuffer *self, struct MTLRenderPassDescriptor *renderPassDescriptor)
{
    struct objc_object *renderCommandEncoder = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("renderCommandEncoderWithDescriptor:"),
        renderPassDescriptor);
    return static_cast<struct MTLRenderCommandEncoder *>(renderCommandEncoder);
}

void MTLRenderCommandEncoder_setCullMode(struct MTLRenderCommandEncoder *self, MTLCullMode cullMode)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLCullMode)>(objc_msgSend)(
        self,
        sel_registerName("setCullMode:"),
        cullMode);
}

void MTLRenderCommandEncoder_setFrontFacingWinding(struct MTLRenderCommandEncoder *self, MTLWinding frontFacingWinding)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLWinding)>(objc_msgSend)(
        self,
        sel_registerName("setFrontFacingWinding:"),
        frontFacingWinding);
}

void MTLRenderCommandEncoder_setRenderPipelineState(struct MTLRenderCommandEncoder *self, struct MTLRenderPipelineState *pipelineState)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setRenderPipelineState:"),
        pipelineState);
}

void MTLRenderCommandEncoder_setDepthStencilState(struct MTLRenderCommandEncoder *self, struct MTLDepthStencilState *depthStencilState)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("setDepthStencilState:"),
        depthStencilState);
}

void MTLRenderCommandEncoder_setVertexBuffer(struct MTLRenderCommandEncoder *self, struct MTLBuffer *buffer, NSUInteger offset, NSUInteger index)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *, NSUInteger, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setVertexBuffer:offset:atIndex:"),
        buffer,
        offset,
        index);
}

void MTLRenderCommandEncoder_setFragmentBuffer(struct MTLRenderCommandEncoder *self, struct MTLBuffer *buffer, NSUInteger offset, NSUInteger index)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *, NSUInteger, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setFragmentBuffer:offset:atIndex:"),
        buffer,
        offset,
        index);
}

void MTLRenderCommandEncoder_setFragmentTexture(struct MTLRenderCommandEncoder *self, struct MTLTexture *texture, NSUInteger index)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("setFragmentTexture:atIndex:"),
        texture,
        index);
}

void MTLRenderCommandEncoder_drawPrimitives(struct MTLRenderCommandEncoder *self, MTLPrimitiveType primitiveType, NSUInteger vertexStart, NSUInteger vertexCount, NSUInteger instanceCount, NSUInteger baseInstance)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, MTLPrimitiveType, NSUInteger, NSUInteger, NSUInteger, NSUInteger)>(objc_msgSend)(
        self,
        sel_registerName("drawPrimitives:vertexStart:vertexCount:instanceCount:baseInstance:"),
        primitiveType,
        vertexStart,
        vertexCount,
        instanceCount,
        baseInstance);
}

void MTLRenderCommandEncoder_endEncoding(struct MTLRenderCommandEncoder *self)
{
    return MTLCommandEncoder_endEncoding(self);
}

struct MTLParallelRenderCommandEncoder *MTLCommandBuffer_parallelRenderCommandEncoderWithDescriptor(struct MTLCommandBuffer *self, struct MTLRenderPassDescriptor *renderPassDescriptor)
{
    struct objc_object *parallelRenderCommandEncoder = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("parallelRenderCommandEncoderWithDescriptor:"),
        renderPassDescriptor);
    return static_cast<struct MTLParallelRenderCommandEncoder *>(parallelRenderCommandEncoder);
}

void MTLParallelRenderCommandEncoder_setLabel(struct MTLParallelRenderCommandEncoder *self, struct NSString *label)
{
    return MTLCommandEncoder_setLabel(self, label);
}

void MTLParallelRenderCommandEncoder_pushDebugGroup(struct MTLParallelRenderCommandEncoder *self, struct NSString *string)
{
    return MTLCommandEncoder_pushDebugGroup(self, string);
}

void MTLParallelRenderCommandEncoder_popDebugGroup(struct MTLParallelRenderCommandEncoder *self)
{
    return MTLCommandEncoder_popDebugGroup(self);
}

struct MTLRenderCommandEncoder *MTLParallelRenderCommandEncoder_renderCommandEncoder(struct MTLParallelRenderCommandEncoder *self)
{
    struct objc_object *renderCommandEncoder = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("renderCommandEncoder"));
    return static_cast<struct MTLRenderCommandEncoder *>(renderCommandEncoder);
}

void MTLParallelRenderCommandEncoder_endEncoding(struct MTLParallelRenderCommandEncoder *self)
{
    return MTLCommandEncoder_endEncoding(self);
}

struct MTLBlitCommandEncoder *MTLCommandBuffer_blitCommandEncoder(struct MTLCommandBuffer *self)
{
    struct objc_object *blitCommandEncoder = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        self,
        sel_registerName("blitCommandEncoder"));
    return static_cast<struct MTLBlitCommandEncoder *>(blitCommandEncoder);
}

void MTLBlitCommandEncoder_setLabel(struct MTLBlitCommandEncoder *self, struct NSString *label)
{
    return MTLCommandEncoder_setLabel(self, label);
}

void MTLBlitCommandEncoder_pushDebugGroup(struct MTLBlitCommandEncoder *self, struct NSString *string)
{
    return MTLCommandEncoder_pushDebugGroup(self, string);
}

void MTLBlitCommandEncoder_popDebugGroup(struct MTLBlitCommandEncoder *self)
{
    return MTLCommandEncoder_popDebugGroup(self);
}

void MTLBlitCommandEncoder_copyFromBuffer(struct MTLBlitCommandEncoder *self, struct MTLBuffer *sourceBuffer, NSUInteger sourceOffset, NSUInteger sourceBytesPerRow, NSUInteger sourceBytesPerImage, MTLSize sourceSize, struct MTLTexture *destinationTexture, NSUInteger destinationSlice, NSUInteger destinationLevel, MTLOrigin destinationOrigin)
{
    reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *, NSUInteger, NSUInteger, NSUInteger, MTLSize, struct objc_object *, NSUInteger, NSUInteger, MTLOrigin)>(objc_msgSend)(
        self,
        sel_registerName("copyFromBuffer:sourceOffset:sourceBytesPerRow:sourceBytesPerImage:sourceSize:toTexture:destinationSlice:destinationLevel:destinationOrigin:"),
        sourceBuffer,
        sourceOffset,
        sourceBytesPerRow,
        sourceBytesPerImage,
        sourceSize,
        destinationTexture,
        destinationSlice,
        destinationLevel,
        destinationOrigin);
}

void MTLBlitCommandEncoder_endEncoding(struct MTLBlitCommandEncoder *self)
{
    return MTLCommandEncoder_endEncoding(self);
}

struct MTLLibrary *MTLDevice_newLibraryWithData(struct MTLDevice *self, dispatch_data_t data, struct NSError **error)
{
    struct objc_object *library = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *, struct objc_object **)>(objc_msgSend)(
        self,
        sel_registerName("newLibraryWithData:error:"),
        data,
        reinterpret_cast<struct objc_object **>(error));
    return static_cast<struct MTLLibrary *>(library);
}

void MTLLibrary_release(struct MTLLibrary *self)
{
    return NSObject_release(self);
}

struct MTLFunction *MTLLibrary_newFunctionWithName(struct MTLLibrary *self, struct NSString *functionName)
{
    struct objc_object *function = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        self,
        sel_registerName("newFunctionWithName:"),
        functionName);
    return static_cast<struct MTLFunction *>(function);
}

void MTLFunction_release(struct MTLFunction *self)
{
    return NSObject_release(self);
}

NSUInteger MTLFunction_retainCount(struct MTLFunction *self)
{
    return NSObject_retainCount(self);
}
