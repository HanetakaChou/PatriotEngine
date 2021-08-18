/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
typedef struct _gfx_buffer_t_ *gfx_buffer_ref;
typedef struct _gfx_mesh_t_ *gfx_mesh_ref;
typedef struct _gfx_texture_t_ *gfx_texture_ref;
typedef struct _gfx_node_t_ *gfx_node_ref;

#ifdef __cplusplus
extern "C"
{
#endif

    PT_ATTR_GFX gfx_connection_ref PT_CALL gfx_connection_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

    // ANativeActivityCallbacks::onNativeWindowResized
    // MTKViewDelegate::drawableSizeWillChange
    PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_resized(gfx_connection_ref gfx_connection, float width, float height);

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
    PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_redraw_needed_release(gfx_connection_ref gfx_connection);

    PT_ATTR_GFX void PT_CALL gfx_connection_destroy(gfx_connection_ref gfx_connection);

    PT_ATTR_GFX gfx_buffer_ref PT_CALL gfx_connection_create_buffer(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL gfx_buffer_read_vertex_input_stream(gfx_connection_ref gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));
    PT_ATTR_GFX bool PT_CALL gfx_buffer_read_index_input_stream(gfx_connection_ref gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));
    PT_ATTR_GFX void PT_CALL gfx_buffer_destroy(gfx_connection_ref gfx_connection, gfx_buffer_ref buffer);

    // We may share the texture but we scarcely share the buffer
    // Thus we don't support buffer
    PT_ATTR_GFX gfx_mesh_ref PT_CALL gfx_connection_create_mesh(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX bool PT_CALL gfx_mesh_read_input_stream(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));
    PT_ATTR_GFX void PT_CALL gfx_mesh_destroy(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh);

    PT_ATTR_GFX gfx_texture_ref PT_CALL gfx_connection_create_texture(gfx_connection_ref gfx_connection);
    // the execution of "gfx_texture_read_input_stream" may be overlapped with "gfx_texture_destroy"
    // but must be after the return of the "gfx_connection_create_texture"
    PT_ATTR_GFX bool PT_CALL gfx_texture_read_input_stream(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));
    // the execution of "gfx_texture_destroy" may be overlapped with "gfx_texture_read_input_stream"
    // but must be after the return of the "gfx_connection_create_texture"
    PT_ATTR_GFX void PT_CALL gfx_texture_destroy(gfx_connection_ref gfx_connection, gfx_texture_ref texture);

    //TLS //node
    // gfx_connection_create_node
    // gfx_node_set_mesh
    //BLS //mesh

    // Top Level Structure - Node
    // Bottom Level Structure - Mesh
    PT_ATTR_GFX gfx_node_ref PT_CALL gfx_connection_create_node(gfx_connection_ref gfx_connection);
    PT_ATTR_GFX void PT_CALL gfx_node_set_mesh(gfx_connection_ref gfx_connection, gfx_node_ref node, gfx_mesh_ref gfx_mesh);
    PT_ATTR_GFX void PT_CALL gfx_node_destroy(gfx_connection_ref gfx_connection, gfx_node_ref node);

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
                                   gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                                   intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
                                   int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
                                   void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) = 0;

    virtual void destroy() = 0;
};
#endif

#endif
