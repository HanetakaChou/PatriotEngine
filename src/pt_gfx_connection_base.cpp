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

#include <stddef.h>
#include "pt_gfx_connection_base.h"

void gfx_connection_base::frame_node_destroy_list_push(class gfx_node_base *node)
{
    mcrt_rwlock_rdlock(&this->m_rwlock_frame_throttling_index);
    uint32_t frame_throttling_index = mcrt_atomic_load(&this->m_frame_throttling_index);
    this->m_frame_node_destory_list[frame_throttling_index].produce(node);
    mcrt_rwlock_rdunlock(&this->m_rwlock_frame_throttling_index);
}

void gfx_connection_base::frame_object_destroy_list_push(class gfx_frame_object_base *frame_object)
{
    mcrt_rwlock_rdlock(&this->m_rwlock_frame_throttling_index);
    uint32_t frame_throttling_index = mcrt_atomic_load(&this->m_frame_throttling_index);
    this->m_frame_object_destory_list[frame_throttling_index].produce(frame_object);
    mcrt_rwlock_rdunlock(&this->m_rwlock_frame_throttling_index);
}

// API
#if defined(PT_WIN32)
extern class gfx_connection_base *gfx_connection_d3d12_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
#endif

#if 0 && defined(PT_POSIX_MACH)
extern class gfx_connection_base *gfx_connection_mtl_init(wsi_window_ref wsi_window);
#endif

#if defined(PT_POSIX_LINUX) || defined(PT_POSIX_MACH) || defined(PT_WIN32)
    extern class gfx_connection_base *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, char const *gfx_cache_dirname);
#endif

// API
class gfx_connection_base *gfx_connection_common_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, char const *gfx_cache_dirname)
{
    class gfx_connection_base *gfx_connection = NULL;

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
        gfx_connection = gfx_connection_vk_init(wsi_connection, wsi_visual, gfx_cache_dirname);
    }
#endif

    return gfx_connection;
}

inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline gfx_node_ref wrap(class gfx_node_base *node) { return reinterpret_cast<gfx_node_ref>(node); }
inline class gfx_node_base *unwrap(gfx_node_ref node) { return reinterpret_cast<class gfx_node_base *>(node); }

inline gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<gfx_mesh_ref>(mesh); }
inline class gfx_mesh_base *unwrap(gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

inline gfx_material_ref wrap(class gfx_material_base *gfx_material) { return reinterpret_cast<gfx_material_ref>(gfx_material); }
inline class gfx_material_base *unwrap(gfx_material_ref gfx_material) { return reinterpret_cast<class gfx_material_base *>(gfx_material); }

inline gfx_texture_ref wrap(class gfx_texture_base *texture) { return reinterpret_cast<gfx_texture_ref>(texture); }
inline class gfx_texture_base *unwrap(gfx_texture_ref texture) { return reinterpret_cast<class gfx_texture_base *>(texture); }

PT_ATTR_GFX pt_gfx_connection_ref PT_CALL gfx_connection_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, char const* gfx_cache_dirname)
{
    return wrap(gfx_connection_common_init(wsi_connection, wsi_visual, gfx_cache_dirname));
}

PT_ATTR_GFX bool PT_CALL gfx_connection_on_wsi_window_created(pt_gfx_connection_ref gfx_connection, wsi_connection_ref wsi_connection, wsi_window_ref wsi_window, float width, float height)
{
    return unwrap(gfx_connection)->on_wsi_window_created(wsi_connection, wsi_window, width, height);
}

PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_window_destroyed(pt_gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->on_wsi_window_destroyed();
}

PT_ATTR_GFX void PT_CALL gfx_connection_on_wsi_window_resized(pt_gfx_connection_ref gfx_connection, float width, float height)
{
    return unwrap(gfx_connection)->on_wsi_resized(width, height);
}

PT_ATTR_GFX void PT_CALL gfx_connection_draw_acquire(pt_gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->on_wsi_redraw_needed_acquire();
}

PT_ATTR_GFX void PT_CALL gfx_connection_draw_release(pt_gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->on_wsi_redraw_needed_release();
}

PT_ATTR_GFX gfx_node_ref PT_CALL gfx_connection_create_node(pt_gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_node());
}

PT_ATTR_GFX gfx_mesh_ref PT_CALL gfx_connection_create_mesh(pt_gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_mesh());
}

PT_ATTR_GFX gfx_material_ref PT_CALL gfx_connection_create_material(pt_gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_material());
}

PT_ATTR_GFX gfx_texture_ref PT_CALL gfx_connection_create_texture(pt_gfx_connection_ref gfx_connection)
{
    return wrap(unwrap(gfx_connection)->create_texture());
}

PT_ATTR_GFX void PT_CALL gfx_connection_destroy(pt_gfx_connection_ref gfx_connection)
{
    return unwrap(gfx_connection)->destroy();
}
