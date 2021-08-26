//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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
#include "pt_gfx_common.h"

typedef struct _wsi_connection_t_ *wsi_connection_ref;
typedef struct _wsi_visual_t_ *wsi_visual_ref;
typedef struct _wsi_window_t_ *wsi_window_ref;

typedef struct _gfx_input_stream_t_ *gfx_input_stream_ref;
enum
{
    PT_GFX_INPUT_STREAM_SEEK_SET = 0,
    PT_GFX_INPUT_STREAM_SEEK_CUR = 1,
    PT_GFX_INPUT_STREAM_SEEK_END = 2
};

typedef struct _gfx_connection_t *gfx_connection_ref;
typedef struct _gfx_node_t_ *gfx_node_ref;
typedef struct _gfx_mesh_t_ *gfx_mesh_ref;
typedef struct _gfx_material_t_ *gfx_material_ref;
typedef struct _gfx_texture_t_ *gfx_texture_ref;

#ifdef __cplusplus
extern "C"
{
#endif

    PT_ATTR_GFX gfx_connection_ref PT_CALL gfx_connection_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);

    PT_ATTR_GFX bool PT_CALL gfx_connection_on_wsi_window_created(gfx_connection_ref gfx_connection, wsi_connection_ref wsi_connection, wsi_window_ref wsi_window, float width, float height);
    PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_window_destroyed(gfx_connection_ref gfx_connection);

    // ANativeActivityCallbacks::onNativeWindowResized
    // MTKViewDelegate::drawableSizeWillChange
    PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_resized(gfx_connection_ref gfx_connection, float width, float height);

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
    PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_redraw_needed_acquire(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_redraw_needed_release(gfx_connection_ref gfx_connection);

    PT_ATTR_GFX void PT_CALL gfx_connection_destroy(gfx_connection_ref gfx_connection);

    // Top Level Structure - Node
    // Bottom Level Structure - Mesh
    PT_ATTR_GFX gfx_node_ref PT_CALL gfx_connection_create_node(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX void PT_CALL gfx_node_set_mesh(gfx_connection_ref gfx_connection, gfx_node_ref gfx_node, gfx_mesh_ref gfx_mesh);
    PT_ATTR_GFX void PT_CALL gfx_node_set_material(gfx_connection_ref gfx_connection, gfx_node_ref gfx_node, gfx_material_ref gfx_material);
    PT_ATTR_GFX void PT_CALL gfx_node_destroy(gfx_connection_ref gfx_connection, gfx_node_ref gfx_node);

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

    PT_ATTR_GFX gfx_mesh_ref PT_CALL gfx_connection_create_mesh(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL gfx_mesh_read_input_stream(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *), intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), void(PT_PTR *gfx_input_stream_destroy_callback)(gfx_input_stream_ref));
    PT_ATTR_GFX void PT_CALL gfx_mesh_destroy(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh);

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

    PT_ATTR_GFX gfx_material_ref PT_CALL gfx_connection_create_material(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL gfx_material_init_with_texture(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material, uint32_t material_model, uint32_t texture_count, gfx_texture_ref *gfx_textures);
    PT_ATTR_GFX void PT_CALL gfx_material_destroy(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material);

    PT_ATTR_GFX gfx_texture_ref PT_CALL gfx_connection_create_texture(gfx_connection_ref gfx_connection);
    // the execution of "gfx_texture_read_input_stream" may be overlapped with "gfx_texture_destroy"
    // but must be after the return of the "gfx_connection_create_texture"
    PT_ATTR_GFX bool PT_CALL gfx_texture_read_input_stream(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename, gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *), intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), void(PT_PTR *gfx_input_stream_destroy_callback)(gfx_input_stream_ref));
    // the execution of "gfx_texture_destroy" may be overlapped with "gfx_texture_read_input_stream"
    // but must be after the return of the "gfx_connection_create_texture"
    PT_ATTR_GFX void PT_CALL gfx_texture_destroy(gfx_connection_ref gfx_connection, gfx_texture_ref texture);

#ifdef __cplusplus
}
#endif

#if 0

//struct gfx_imesh;
//struct gfx_iterrain;
struct gfx_itexture;

// We may treat the gfx server as the 3D version X11 server.
struct gfx_iconnection
{
    //init_input_stream_callback

    //virtual struct gfx_imesh *create_mesh() = 0;

    virtual struct gfx_itexture *create_texture() = 0;

    // app can use these count to tweak the "create/destroy" strategy
    // size_t request_count() = 0;
    // size_t fail_count() =0; // since the loading is async, we prefer fail_count than resident_count
    // ~~size_t resident_count() = 0~~

    virtual void destroy() = 0;

    // ANativeActivityCallbacks::onNativeWindowResized
    // MTKViewDelegate::drawableSizeWillChange
    virtual void on_wsi_resized(wsi_window_ref wsi_window, float width, float height) = 0;

    // ANativeActivityCallbacks::onNativeWindowRedrawNeeded
    // MTKViewDelegate::drawInMTKView
    // the gfx module may use the given window to recreate the swapchain
    virtual void on_wsi_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) = 0; //frame throttling

    // update animation etc

    virtual void wsi_on_redraw_needed_draw_and_release() = 0;
};

// gfx_imesh -> X Pixmap
// mesh_file -> X Bitmap
// https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/#pixmapst

struct gfx_imesh
{
    //virtual bool put_vertex(/*inputstream*/) = 0; //xcb_put_image_checked

    // PMD GLTF
    // vertex/index info //ignore others
    //virual bool attach_input_stream() //associate with file //can change

    virtual bool put_material() = 0;

    virtual bool put_texture() = 0; //MDL Library

    virtual void destroy() = 0;
};

struct gfx_imaterial
{
    // MDL //OSL
    //virual bool attach_input_stream() //associate with file

    virtual bool read_input_stream() = 0; //XReadBitmapFile //XCreateBitmapFromData

    //is_resident

    virtual void destroy() = 0;
};

struct gfx_itexture
{
    // DDS //PVR
    //virual bool attach_input_stream(asset_url) //associate with file //gfx automatic evict and resident

    // for debug purpose
    // make_resident
    virtual bool read_input_stream(char const *initial_filename,
                                   gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *),
                                   intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t),
                                   int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int),
                                   void(PT_PTR *gfx_input_stream_destroy_callback)(gfx_input_stream_ref )) = 0;

    virtual void destroy() = 0;
};
#endif

#endif
