//
// Copyright (C) YuqiaoZhang(HanetakaChou)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_GFX_CONNECTION_H_
#define _PT_GFX_CONNECTION_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_common.h>
#include <pt_math.h>
#include <pt_io.h>

typedef struct pt_gfx_opaque_wsi_connection_t *pt_gfx_wsi_connection_ref;
typedef struct pt_gfx_opaque_wsi_visual_t *pt_gfx_wsi_visual_ref;
typedef struct pt_gfx_opaque_wsi_window_t *pt_gfx_wsi_window_ref;

typedef struct pt_gfx_opaque_connection_t *pt_gfx_connection_ref;
typedef struct pt_gfx_opaque_scene_t *pt_scene_ref;
typedef struct pt_gfx_opaque_node_t *pt_gfx_node_ref;
typedef struct pt_gfx_opaque_mesh_t *pt_gfx_mesh_ref;

typedef struct pt_gfx_opaque_material_t *pt_gfx_material_ref;
typedef struct pt_gfx_opaque_texture_t *pt_gfx_texture_ref;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_GFX pt_gfx_connection_ref PT_CALL pt_gfx_connection_init(
        pt_gfx_wsi_connection_ref wsi_connection,
        pt_gfx_wsi_visual_ref wsi_visual,
        pt_input_stream_init_callback cache_input_stream_init_callback,
        pt_input_stream_stat_size_callback cache_input_stream_stat_size_callback,
        pt_input_stream_read_callback cache_input_stream_read_callback,
        pt_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_output_stream_init_callback cache_output_stream_init_callback,
        pt_output_stream_write_callback cache_output_stream_write_callback,
        pt_output_stream_destroy_callback cache_output_stream_destroy_callback);

    // note that the "on_wsi_window_created"/"on_wsi_window_destroyed" can't be overlapped with the "draw_acquire"/"draw_acquire"
    PT_ATTR_GFX bool PT_CALL pt_gfx_connection_on_wsi_window_created(pt_gfx_connection_ref gfx_connection, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_window_ref wsi_window, float width, float height);
    PT_ATTR_GFX void PT_CALL pt_gfx_connection_on_wsi_window_destroyed(pt_gfx_connection_ref gfx_connection);

    // ANativeActivityCallbacks::onNativeWindowResized
    // MTKViewDelegate::drawableSizeWillChange
    PT_ATTR_GFX void PT_CALL pt_gfx_connection_on_wsi_window_resized(pt_gfx_connection_ref gfx_connection, float width, float height);

    // ANativeActivityCallbacks::onNativeWindowRedrawNeeded
    // MTKViewDelegate::drawInMTKView

    // usage
    // [current thread] app on_redraw_needed //drawInMTKView //onNativeWindowRedrawNeeded
    // [arbitrary thread] app update info which doesn't depend on accurate time ( scenetree etc ) //app may update in other threads
    // [current thread] app call gfx acquire //gfx sync ( from other threads ) and flatten scenetree //and then gfx frame throttling
    // [current thread] app update time-related info ( animation etc ) //frame throttling make the time here less latency //scenetree update here (include update from other threads) is ignored in current frame and to impact on the next frame
    // [current thread] app call gfx release //gfx draw and present //gfx not sync scenetree here

    // the gfx module may use the given window to recreate the swapchain
    // frame throttling

    // TODO the acquire may return a visible set by culling
    PT_ATTR_GFX void PT_CALL pt_gfx_connection_draw_acquire(pt_gfx_connection_ref gfx_connection); //add scene paramater

    PT_ATTR_GFX void PT_CALL pt_gfx_connection_draw_release(pt_gfx_connection_ref gfx_connection); //add scene paramater

    PT_ATTR_GFX void PT_CALL pt_gfx_connection_destroy(pt_gfx_connection_ref gfx_connection);

    // NVIDIA PRO PIPELINE
    // https://developer.nvidia.com/nvidia-pro-pipeline

    // 
    // Sync
    // Prepare
    // Commit
    // Execute


    // flatten_scene - HdRenderIndex
    // ??? - HdInstance
    // flatten_node - HdRprim
    // Hydra introduces "HdRprimSharedData" - HdBufferArrayRangeContainer - HdBufferArrayRange - HdBufferArray

    // Top Level Structure - Node
    // Bottom Level Structure - Mesh Material etc
    PT_ATTR_GFX pt_scene_ref PT_CALL pt_gfx_connection_create_flatten_scene(pt_gfx_connection_ref gfx_connection);
    PT_ATTR_GFX void PT_CALL pt_gfx_flatten_scene_set_root_node(pt_gfx_connection_ref gfx_connection, pt_scene_ref gfx_scene, pt_gfx_node_ref gfx_node);
    PT_ATTR_GFX void PT_CALL pt_gfx_flatten_scene_set_camera(pt_gfx_connection_ref gfx_connection, pt_scene_ref gfx_scene, pt_math_vec3 const *eye_position, pt_math_vec3 const *eye_direction, pt_math_vec3 const *up_direction);
    PT_ATTR_GFX void PT_CALL pt_gfx_flatten_scene_set_directional_light(pt_gfx_connection_ref gfx_connection, pt_scene_ref gfx_scene, pt_math_vec3 const *direction);
    PT_ATTR_GFX void PT_CALL pt_gfx_flatten_scene_destroy(pt_gfx_connection_ref gfx_connection, pt_scene_ref gfx_scene);

    PT_ATTR_GFX pt_gfx_node_ref PT_CALL pt_gfx_connection_create_node(pt_gfx_connection_ref gfx_connection, uint32_t material_count); // add callback paramter
    PT_ATTR_GFX void PT_CALL pt_gfx_node_set_transform(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_math_mat4x4 const *transform);
    PT_ATTR_GFX void PT_CALL pt_gfx_node_set_mesh(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_mesh_ref gfx_mesh);
    // UE4: one mesh - different elements - different materials
    PT_ATTR_GFX void PT_CALL pt_gfx_node_set_material(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, uint32_t material_index, pt_gfx_material_ref gfx_material);

    //PT_ATTR_GFX void PT_CALL pt_gfx_node_add_child(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_node_ref gfx_child_node);
    //PT_ATTR_GFX void PT_CALL pt_gfx_node_remove_child(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_node_ref gfx_child_node);
    

    
    PT_ATTR_GFX void PT_CALL pt_gfx_node_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node);

    // merge to "create_node"
    // callback should not be changed
    // since the destory is asynchrous (perhaps delay to next frame) // you need to use "ref_count" etc to make sure the "user_data" is available
    // PT_ATTR_GFX void PT_CALL pt_gfx_node_set_pre_draw_acquire_callback(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, void (*callback)()); // in acquire we will can this function before culling

    // In hydra, there exists "HdInstance"  

    // We don't support "HdStRenderBuffer" and we support "HdStMesh" instead
    // Because we scarcely share the "buffer" but we may share "mesh" between the node
    //
    // Although the concepts may be limited to the assets, this design allows us reading the data into the staging buffer directly without any temp buffer at all.
    //
    // -------------------------------------------------------------------------------------
    // Hydra Primitives
    //
    //  HdBprim //B(uffer) Prim(itive)
    //  [pxr/imaging/lib/hd/bprim.h\]
    //  [pxr/imaging/lib/hdSt/renderDelegate.cpp\]
    //  HdStRenderDelegate::CreateBprim
    //      HdStTexture HdStRenderBuffer HdStField
    //
    //  HdSprim //S(tate) Prim(itive)
    //  [pxr/imaging/lib/hd/sprim.h\]
    //  [pxr/imaging/lib/hdSt/renderDelegate.cpp\]
    //  HdStRenderDelegate::CreateSprim
    //      HdCamera HdStLight HdStMaterial HdStDrawTarget HdStExtComputation
    //
    //  HdRprim //R(enderable) Prim(itive)
    //  [pxr/imaging/lib/hd/rprim.h\]
    //  [pxr/imaging/lib/hdSt/renderDelegate.cpp\]
    //  HdStRenderDelegate::CreateRprim
    //      HdStMesh HdStBasisCurves HdStPoints HdStVolume

    PT_ATTR_GFX pt_gfx_mesh_ref PT_CALL pt_gfx_connection_create_mesh(pt_gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL pt_gfx_mesh_read_input_stream(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, char const *initial_filename, pt_input_stream_init_callback input_stream_init_callback, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback, pt_input_stream_destroy_callback input_stream_destroy_callback);
    PT_ATTR_GFX void PT_CALL pt_gfx_mesh_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh);

    enum
    {
        GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS = 1,
        GFX_MATERIAL_MODEL_COUNT = 2
    };

    // PBR Specular Glossiness

    // [glTF 2.0: PBR Materials](https://www.khronos.org/assets/uploads/developers/library/2017-gtc/glTF-2.0-and-PBR-GTC_May17.pdf) // https://github.com/KhronosGroup/glTF
    // [Conversion between metallic-roughness and specular-glossiness workflows](https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness/examples)

    // PBR Guide 2018 version.pdf - Substance 3D

    // diffusecolor
    // specularcolor = fresnel(0) = F0

    // convert "metallic-roughness" to "specular-glossiness"
    // specularcolor = lerp((0.4,0.4,0.4), basecolor, metallic) //(0.4,0.4,0.4) specularcolor for dielectric(/insulator/non-metal) // evidently, "metallic-roughness"workflow can't control the specularcolor of the dielectric
    // diffusecolor = 1 - specularcolor
    // glossiness = 1 - roughness

    // technically, "albedo" means "diffusecolor" // The constant reflectance value of a Lambertian BRDF is typically referred to as the diffuse color cdiff or the albedo ρ.
    // in Unity3D, "albedo" means "basecolor" in "metallic-roughness"workflow and means "diffusecolor" in "specular-glossiness" workflow


    // https://github.com/Autodesk/standard-surface

    enum
    {
        //PBR Specular Glossiness
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_DIFFUSE_COLOR_TEXTURE_INDEX = 0,
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_SPECULAR_COLOR_TEXTURE_INDEX = 1,
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_GLOSSINESS_TEXTURE_INDEX = 2,
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_AMBIENT_OCCLUSION_TEXTURE_INDEX = 3, // Another UV Channel // Why ??? // For lightmaps, multi-objects share the same map and the UV should not be overlapped.
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_HEIGHT_TEXTURE_INDEX = 4,            // Parallax
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_NORMAL_TEXTURE_INDEX = 5,            // Bump
        //GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS_TEXTURE_COUNT = 6,
        GFX_MATERIAL_MAX_TEXTURE_COUNT = 6
    };

    //enum
    //{
    //    GFX_MATERIAL_DIFFUSE_COLOR_TEXTURE_INDEX = 0,
    //};

    enum pt_gfx_material_closure_info_t
    {
        GFX_MATERIAL_CLOSURE_PBR_SPECULAR_GLOSSINESS = 0,
        GFX_MATERIAL_CLOSURE_MAX_COUNT = 1
    };

    struct pt_gfx_material_init_info_t
    {
        enum pt_gfx_material_closure_info_t closure;
        pt_gfx_texture_ref diffuse_color;
        pt_gfx_texture_ref specular_color;
        pt_gfx_texture_ref glossiness;
    };

    PT_ATTR_GFX pt_gfx_material_ref PT_CALL pt_gfx_connection_create_material(pt_gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL pt_gfx_material_init_with_texture(pt_gfx_connection_ref gfx_connection, pt_gfx_material_ref gfx_material, uint32_t material_model, uint32_t texture_count, pt_gfx_texture_ref *gfx_textures);
    PT_ATTR_GFX bool PT_CALL pt_gfx_material_init(pt_gfx_connection_ref gfx_connection, pt_gfx_material_ref gfx_material, struct pt_gfx_material_init_info_t const *gfx_material_init_info);
    PT_ATTR_GFX void PT_CALL pt_gfx_material_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_material_ref gfx_material);

    PT_ATTR_GFX pt_gfx_texture_ref PT_CALL pt_gfx_connection_create_texture(pt_gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL pt_gfx_texture_read_input_stream(
        pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture,
        char const *initial_filename,
        pt_input_stream_init_callback input_stream_init_callback, 
        pt_input_stream_read_callback input_stream_read_callback, 
        pt_input_stream_seek_callback input_stream_seek_callback, 
        pt_input_stream_destroy_callback input_stream_destroy_callback);
    PT_ATTR_GFX void PT_CALL pt_gfx_texture_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture);

#ifdef __cplusplus
}
#endif

#endif
