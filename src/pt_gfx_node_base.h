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

#ifndef _PT_GFX_NODE_BASE_H_
#define _PT_GFX_NODE_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <pt_gfx_connection.h>
#include <pt_math.h>
#include <pt_mcrt_allocator.h>
#include "imaging/mesh.h"
#include "pt_gfx_material_base.h"

class gfx_node_base
{
    // [Hudson 2006] 
    // Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
    // 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List 
    template <typename T>
    struct mpsc_list
    {
        static_assert(std::is_pod<T>::value, "");

        struct link_list
        {
            struct link_list *m_next;
            T m_value;
        };
        struct link_list *m_link_list_head;

        inline void init();
        inline void produce(T value);
        inline void consume_and_clear(void (*consume_callback)(T value, void *user_defined), void *user_defined);
    };

    pt_math_alignas16_mat4x4 m_mat_m;

    class gfx_mesh_base *m_mesh;

    uint32_t m_material_count;
    class gfx_material_base **m_materials;

    // concurrent_vector
    // https://web.archive.org/web/20090301235240/http://software.intel.com/en-us/blogs/2008/07/24/tbbconcurrent_vector-secrets-of-memory-organization/


    // concurrent queue
    // https://web.archive.org/web/20090923193040/http://software.intel.com/zh-cn/blogs/2009/08/13/tbb-concurrent_queue

    mcrt_vector<class gfx_node_vk *> m_child_node_list;

    mcrt_vector<size_t> m_child_node_free_index_list;


    size_t m_index_in_parent_node_list; //= -1


    // TODO
    // padding avoid false sharing


    // "to add" and "to remove" may not sync
    // may repatriate to remove first ?

    //mpsc_list<class gfx_node_vk *> m_child_node_to_add_list;

    //mpsc_list<class gfx_node_vk *> m_child_node_to_remove_list;

protected:
    inline gfx_node_base(uint32_t material_count) : m_mesh(NULL), m_material_count(material_count)
    {
        pt_math_store_alignas16_mat4x4(&this->m_mat_m, pt_math_mat_identity());

        this->m_materials = static_cast<class gfx_material_base **>(mcrt_aligned_malloc(sizeof(class gfx_material_base*) * this->m_material_count, alignof(class gfx_material_base*)));
        for (uint32_t material_index = 0U; material_index < material_count; ++material_index)
        {
            this->m_materials[material_index] = NULL;
        }
    }

public:
    inline pt_math_alignas16_mat4x4 get_transform() const { return this->m_mat_m; }

    void set_mesh(class gfx_connection_base *gfx_connection, class gfx_mesh_base *future_mesh);

    inline class gfx_mesh_base *get_mesh() const { return this->m_mesh; }

    void set_material(class gfx_connection_base *gfx_connection, uint32_t material_index, class gfx_material_base *future_material);

    class gfx_material_base* get_material(uint32_t material_index) const;

    void destroy(class gfx_connection_base *gfx_connection);
};

template <typename T>
inline void gfx_node_base::mpsc_list<T>::init()
{
    this->m_link_list_head = NULL;
}

template <typename T>
inline void gfx_node_base::mpsc_list<T>::produce(T value)
{
    // Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush

    struct link_list *link_list_node = new (mcrt_aligned_malloc(sizeof(struct link_list), alignof(struct link_list))) link_list{};
    link_list_node->m_value = value;

    struct link_list *link_list_head;
    do
    {
        link_list_head = mcrt_atomic_load(&this->m_link_list_head);
        link_list_node->m_next = link_list_head;
    } while (link_list_head != mcrt_atomic_cas_ptr(&this->m_link_list_head, link_list_node, link_list_head));
}

template <typename T>
inline void gfx_node_base::mpsc_list<T>::consume_and_clear(void (*consume_callback)(T value, void *user_defined), void *user_defined)
{
    // Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / repatriatePublicFreeList

    struct link_list *link_list_node = mcrt_atomic_xchg_ptr(this->m_link_list_head, static_cast<struct link_list *>(NULL));
    while (link_list_node != NULL)
    {
        consume_callback(link_list_node->m_value, user_defined);
        struct link_list *link_list_next = link_list_node->m_next;
        mcrt_aligned_free(link_list_node);

        link_list_node = link_list_next;
    }
    this->m_link_list_head = NULL;
}

#endif
