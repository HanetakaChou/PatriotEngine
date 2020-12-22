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
#include <stdint.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_texture_common.h"

inline gfx_connection_ref wrap(class gfx_connection_common *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_common *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_common *>(gfx_connection); }

inline gfx_texture_ref wrap(class gfx_texture_common *texture) { return reinterpret_cast<gfx_texture_ref>(texture); }
inline class gfx_texture_common *unwrap(gfx_texture_ref texture) { return reinterpret_cast<class gfx_texture_common *>(texture); }

PT_ATTR_GFX gfx_connection_ref PT_CALL gfx_connection_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    return wrap(gfx_connection_common::init(wsi_connection, wsi_visual));
}

PT_ATTR_GFX void PT_CALL gfx_connection_destroy(gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->destroy();
}

PT_ATTR_GFX gfx_texture_ref PT_CALL gfx_connection_create_texture(gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_texture());
}

PT_ATTR_GFX void PT_CALL gfx_connection_wsi_on_resized(gfx_connection_ref gfx_connection, wsi_window_ref wsi_window, float width, float height)
{
    return unwrap(gfx_connection)->wsi_on_resized(wsi_window, width, height);
}

PT_ATTR_GFX void gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection_ref gfx_connection, wsi_window_ref wsi_window, float width, float height)
{
    return unwrap(gfx_connection)->wsi_on_redraw_needed_acquire(wsi_window, width, height);
}

PT_ATTR_GFX void gfx_connection_wsi_on_redraw_needed_release(gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->wsi_on_redraw_needed_release();
}

PT_ATTR_GFX bool gfx_texture_read_input_stream(
    gfx_texture_ref texture,
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return unwrap(texture)->read_input_stream(initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

PT_ATTR_GFX void gfx_texture_destroy(gfx_texture_ref texture)
{
    return unwrap(texture)->destroy();
}