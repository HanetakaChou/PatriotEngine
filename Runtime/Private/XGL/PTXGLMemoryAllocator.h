#ifndef PT_XGL_MEMORYALLOCATOR_H
#define PT_XGL_MEMORYALLOCATOR_H 1

#include <stdint.h>

//https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_resource_flags

enum
{
    //https://chromium.googlesource.com/angle/angle
    //src/libANGLE/renderer/vulkan/vk_helpers.cpp
    //ImageHelper::stageSubresourceUpdateImpl
    PT_XGL_MEM_T_BUF_STAGING,

    PT_XGL_MEM_T_BUF_CONSTANT,

    //For static objects in the scene
    //----------------------------------
    //PT_XGL_MEM_T_BUF_IMMUTABLE,

    //-------------------------------------------------
    PT_XGL_MEM_T_BUF_VERTEX,

    PT_XGL_MEM_T_BUF_INDEX,

    //Transient
    //D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
    PT_XGL_MEM_T_TEX_RT_SR,

    //To be consistent with Metal, we write Depth to RenderTarget.
    //Hardware use Hiz to implement DepthTest, we may always deny ShaderResouce for max efficiency.
    //------
    //Transient
    //D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
    PT_XGL_MEM_T_TEX_DS,

    //For texture assets
    //-----
    PT_XGL_MEM_T_TEX_SR,

    //-------------------------------------------------
    PT_XGL_MEM_T_BUF_UA_SR,

    PT_XGL_MEM_T_BUF_SR,

    //Since we can't use ComputeShader in RenderPass, we don't need Transient.
    //D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    PT_XGL_MEM_T_TEX_UA_SR,

    PT_XGL_MEM_T_COUNT
};

#endif