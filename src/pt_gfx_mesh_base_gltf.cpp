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
#include <pt_gfx_connection.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_log.h>
#include <pt_mcrt_scalable_allocator.h>
#include <vector>
#include "pt_gfx_mesh_base_gltf_lex_yacc.h"
#include <string.h>
#include <assert.h>

template <typename T>
using mcrt_vector = std::vector<T, mcrt::scalable_allocator<T>>;

struct gltf_scene
{
    mcrt_vector<int> m_nodes;
};

struct gltf_root
{
    int default_scene_index;
    mcrt_vector<gltf_scene> m_scenes;
};

struct gltf_yy_extra_type
{
    char const *m_initial_filename;
    intptr_t(PT_PTR *m_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
    struct gltf_root *m_gltf_root;
};

// Lex Yacc
extern "C" int gltf_yylex_init_extra(void *user_defined, void **yyscanner);
extern "C" void gltf_yyset_in(gfx_input_stream_ref input_stream, void *yyscanner);
extern "C" int gltf_yylex_destroy(void *yyscanner);
extern "C" int gltf_yyparse(void *user_defined, void *yyscanner);

bool gltf_parse_input_stream(char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    gfx_input_stream_ref input_stream;
    {
        class internal_input_stream_guard
        {
            gfx_input_stream_ref *const m_input_stream;
            void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);

        public:
            inline internal_input_stream_guard(
                gfx_input_stream_ref *input_stream,
                char const *initial_filename,
                gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
                : m_input_stream(input_stream),
                  m_input_stream_destroy_callback(input_stream_destroy_callback)
            {
                (*m_input_stream) = input_stream_init_callback(initial_filename);
            }
            inline ~internal_input_stream_guard()
            {
                m_input_stream_destroy_callback((*m_input_stream));
            }
        } instance_internal_input_stream_guard(&input_stream, initial_filename, input_stream_init_callback, input_stream_destroy_callback);

        struct gltf_root gltf_root;

        struct gltf_yy_extra_type user_defined;
        user_defined.m_initial_filename = initial_filename;
        user_defined.m_input_stream_read_callback = input_stream_read_callback;
        user_defined.m_gltf_root = &gltf_root;

        void *yyscanner;
        gltf_yylex_init_extra(&user_defined, &yyscanner);

        gltf_yyset_in(input_stream, yyscanner);

        int res_parse = gltf_yyparse(&user_defined, yyscanner);

        gltf_yylex_destroy(yyscanner);
    }

    //if ((error_out != NULL) && (!_jsonparser.m_msg_error.str().empty()))
    //{
    //	strncpy(error_out, _jsonparser.m_msg_error.str().c_str(), error_maxsize);
    //}

    //return (_res_parse == 0) ? _jsonparser.m_root_value : NULL;
    return true;
}

void *gltf_alloc_callback(size_t size, void *user_defined)
{
    return mcrt_aligned_malloc(size, alignof(uint8_t));
}

void *gltf_realloc_callback(void *ptr, size_t size, void *user_defined)
{
    return mcrt_aligned_realloc(ptr, size, alignof(uint8_t));
}

void gltf_free_callback(void *ptr, void *user_defined)
{
    return mcrt_aligned_free(ptr);
}

ptrdiff_t gltf_lex_input_callback(void *input_stream_void, void *buf, size_t size, void *user_defined_void)
{
    gfx_input_stream_ref input_stream = static_cast<gfx_input_stream_ref>(input_stream_void);
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    return user_defined->m_input_stream_read_callback(input_stream, buf, size);
}

void gltf_lex_fatal_error_callback(int line, int column, char const *msg, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    mcrt_log_print("%s:%i:%i: error: %s", user_defined->m_initial_filename, line, column, msg);
    return;
}

int gltf_lex_memcmp_callback(void const *ptr1, void const *ptr2, size_t num, void *user_defined)
{
    return memcmp(ptr1, ptr2, num);
}

void gltf_lex_memcpy_callback(char *dest, char const *src, int count, void *user_defined)
{
    mcrt_memcpy(dest, src, count);
    return;
}

void gltf_yacc_error_callback(int line, int column, char const *msg, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    mcrt_log_print("%s:%i:%i: error: %s", user_defined->m_initial_filename, line, column, msg);
    return;
}

void gltf_yacc_set_default_scene_index_callback(int default_scene_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    user_defined->m_gltf_root->default_scene_index = default_scene_index;
}

int gltf_yacc_scene_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int scene_index = user_defined->m_gltf_root->m_scenes.size();
    user_defined->m_gltf_root->m_scenes.emplace_back();
    return scene_index;
}

void gltf_yacc_scene_set_nodes_callback(int scene_index, int const *nodes_data, int nodes_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(scene_index < user_defined->m_gltf_root->m_scenes.size());
    user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.insert(user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.end(), nodes_data, nodes_data + nodes_size);
}

int gltf_yacc_scene_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int scene_index = user_defined->m_gltf_root->m_scenes.size();
    return scene_index;
}