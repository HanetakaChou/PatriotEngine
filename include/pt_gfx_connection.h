/*
 * Copyright (C) YuqiaoZhang
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
#include "pt_wsi_window.h"

//Some platforms (e.g. X11) may wait on the first size_change_callback //to retrieve the info required by the platform_physical_device_presentation_support   
//We need to invoke size_change_callback on these plaforms in the listen_size_change to avoid dead-lock
extern "C" PT_GFX_ATTR struct gfx_iconnection *PT_CALL gfx_connection_init(struct wsi_iwindow *window);

//struct gfx_imesh;
//struct gfx_iterrain;
struct gfx_itexture;

//We can just treat the gfx server as the 3D version X11 server.
struct gfx_iconnection
{
    //virtual struct gfx_imesh *create_mesh() = 0;

    virtual struct gfx_itexture *create_texture() = 0;

    virtual void destroy() = 0;
};

typedef struct _gfx_input_stream_t_ *gfx_input_stream;

enum
{
    PT_GFX_INPUT_STREAM_SEEK_SET = 0,
    PT_GFX_INPUT_STREAM_SEEK_CUR = 1,
    PT_GFX_INPUT_STREAM_SEEK_END = 2
};

//gfx_imesh -> X Pixmap
//mesh_file -> X Bitmap
//https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/#pixmapst

struct gfx_imesh
{
    virtual bool put_vertex(/*inputstream*/) = 0; //xcb_put_image_checked

    virtual bool put_material() = 0;

    virtual bool put_texture() = 0; //MDL Library

    virtual void destroy() = 0;
};

struct gfx_imaterial
{
    //MDL //OSL
    virtual bool read_input_stream() = 0; //XReadBitmapFile //XCreateBitmapFromData

    virtual void destroy() = 0;
};

struct gfx_itexture
{
    //DDS //PVR
    virtual bool read_input_stream(char const *initial_filename,
                                   gfx_input_stream(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                                   intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream input_stream, void *buf, size_t count),
                                   int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream input_stream, int64_t offset, int whence),
                                   void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream input_stream)) = 0;

    virtual void destroy() = 0;
};

#endif
