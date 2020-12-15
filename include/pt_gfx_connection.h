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

#ifdef __cplusplus
extern "C"
{
#endif

#include "pt_gfx_common.h"
#include "pt_wsi_window.h"

//struct gfx_imesh;
//struct gfx_iterrain;

//We can just treat the gfx server as the 3D version X11 server.
struct gfx_iconnection
{
    //virtual gfx_imesh create_mesh() = 0;

    virtual void destroy() = 0;
};

PT_GFX_ATTR gfx_iconnection *PT_CALL gfx_connection_init(struct wsi_iwindow *window);

//gfx_imesh -> X Pixmap   
//mesh_file -> X Bitmap  
//https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/#pixmapst  

struct gfx_imesh
{
    virtual bool put_mesh(/*inputstream*/) = 0; //xcb_put_image_checked

    virtual void destroy() = 0;
};

#ifdef __cplusplus
}
#endif

#endif
