#ifndef _METAL_CXX_IMPL_H_
#define _METAL_CXX_IMPL_H_ 1

#include "NSRuntime_CXX_IMPL.h"

struct MTLDevice : public NSObject
{
    MTLDevice() = delete;
};

struct MTLVertexDescriptor : public NSObject
{
    MTLVertexDescriptor() = delete;
};

struct MTLVertexBufferLayoutDescriptorArray : public NSObject
{
    MTLVertexBufferLayoutDescriptorArray() = delete;
};

struct MTLVertexBufferLayoutDescriptor : public NSObject
{
    MTLVertexBufferLayoutDescriptor() = delete;
};

struct MTLVertexAttributeDescriptorArray : public NSObject
{
    MTLVertexAttributeDescriptorArray() = delete;
};

struct MTLVertexAttributeDescriptor : public NSObject
{
    MTLVertexAttributeDescriptor() = delete;
};

struct MTLRenderPipelineColorAttachmentDescriptorArray : public NSObject
{
    MTLRenderPipelineColorAttachmentDescriptorArray() = delete;
};

struct MTLRenderPipelineColorAttachmentDescriptor : public NSObject
{
    MTLRenderPipelineColorAttachmentDescriptor() = delete;
};

struct MTLRenderPipelineDescriptor : public NSObject
{
    MTLRenderPipelineDescriptor() = delete;
};

struct MTLRenderPipelineState : public NSObject
{
    MTLRenderPipelineState() = delete;
};

struct MTLDepthStencilDescriptor : public NSObject
{
    MTLDepthStencilDescriptor() = delete;
};

struct MTLDepthStencilState : public NSObject
{
    MTLDepthStencilState() = delete;
};

struct MTLResource : public NSObject
{
    MTLResource() = delete;
};

struct MTLBuffer : public MTLResource
{
    MTLBuffer() = delete;
};

struct MTLTextureDescriptor : public NSObject
{
    MTLTextureDescriptor() = delete;
};

struct MTLTexture : public MTLResource
{
    MTLTexture() = delete;
};

struct MTLCommandQueue : public MTLResource
{
    MTLCommandQueue() = delete;
};

struct MTLCommandBuffer : public MTLResource
{
    MTLCommandBuffer() = delete;
};

struct MTLDrawable : public MTLResource
{
    MTLDrawable() = delete;
};

struct MTLRenderPassDescriptor : public NSObject
{
    MTLRenderPassDescriptor() = delete;
};

struct MTLCommandEncoder : public NSObject
{
    MTLCommandEncoder() = delete;
};

struct MTLRenderCommandEncoder : public MTLCommandEncoder
{
    MTLRenderCommandEncoder() = delete;
};

struct MTLParallelRenderCommandEncoder : public MTLCommandEncoder
{
    MTLParallelRenderCommandEncoder() = delete;
};

struct MTLBlitCommandEncoder : public MTLCommandEncoder
{
    MTLBlitCommandEncoder() = delete;
};

struct MTLLibrary : public NSObject
{
    MTLLibrary() = delete;
};

struct MTLFunction : public NSObject
{
    MTLFunction() = delete;
};

#endif
