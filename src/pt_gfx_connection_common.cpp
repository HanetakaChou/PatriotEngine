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

#include <stddef.h>
#include "pt_gfx_connection_common.h"

#if defined(PT_WIN32)
extern class gfx_connection_common *gfx_connection_d3d12_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
#endif

#if 0 && defined(PT_POSIX_MACH)
extern class gfx_connection_common *gfx_connection_mtl_init(wsi_window_ref wsi_window);
#endif

#if defined(PT_POSIX_LINUX) || defined(PT_POSIX_MACH) || defined(PT_WIN32)
    extern class gfx_connection_common *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);
#endif

class gfx_connection_common *gfx_connection_common_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    class gfx_connection_common *gfx_connection = NULL;

#if defined(PT_WIN32)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_d3d12_init(wsi_connection, wsi_visual);
    }
#endif

#if 0 && defined(PT_POSIX_MACH)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_mtl_init(wsi_window);
    }
#endif

#if defined(PT_POSIX_LINUX) || defined(PT_POSIX_MACH) || defined(PT_WIN32)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_vk_init(wsi_connection, wsi_visual, wsi_window);
    }
#endif

    return gfx_connection;
}

//--- export ---

inline gfx_connection_ref wrap(class gfx_connection_common *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_common *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_common *>(gfx_connection); }

inline gfx_texture_ref wrap(class gfx_texture_common *texture) { return reinterpret_cast<gfx_texture_ref>(texture); }
inline class gfx_texture_common *unwrap(gfx_texture_ref texture) { return reinterpret_cast<class gfx_texture_common *>(texture); }

inline gfx_buffer_ref wrap(class gfx_buffer_base *buffer) { return reinterpret_cast<gfx_buffer_ref>(buffer); }
inline class gfx_buffer_base *unwrap(gfx_buffer_ref buffer) { return reinterpret_cast<class gfx_buffer_base *>(buffer); }

PT_ATTR_GFX gfx_connection_ref PT_CALL gfx_connection_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    return wrap(gfx_connection_common_init(wsi_connection, wsi_visual, wsi_window));
}

PT_ATTR_GFX void PT_CALL gfx_connection_destroy(gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->destroy();
}

PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_resized(gfx_connection_ref gfx_connection, float width, float height)
{
    return unwrap(gfx_connection)->on_wsi_resized(width, height);
}

PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->on_wsi_redraw_needed_acquire();
}

PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_redraw_needed_release(gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->on_wsi_redraw_needed_release();
}

PT_ATTR_GFX gfx_buffer_ref PT_CALL gfx_connection_create_buffer(gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_buffer());
}

PT_ATTR_GFX gfx_texture_ref PT_CALL gfx_connection_create_texture(gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_texture());
}