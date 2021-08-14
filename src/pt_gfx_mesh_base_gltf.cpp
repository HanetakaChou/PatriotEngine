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
#include <string>
#include "pt_gfx_mesh_base_gltf_lex_yacc.h"
#include <string.h>
#include <assert.h>
#include <pt_math.h>

static_assert(sizeof(math_vec3) == sizeof(float[3]), "");
static_assert(alignof(math_vec3) == alignof(float[3]), "");
static_assert(sizeof(math_vec4) == sizeof(float[4]), "");
static_assert(alignof(math_vec4) == alignof(float[4]), "");
static_assert(sizeof(math_mat4x4) == sizeof(float[16]), "");
static_assert(alignof(math_mat4x4) == alignof(float[16]), "");

inline math_vec3 *unwrap_vec3(float vec3[3]) { return reinterpret_cast<math_vec3 *>(vec3); }
inline math_vec4 *unwrap_vec4(float vec4[4]) { return reinterpret_cast<math_vec4 *>(vec4); }
inline math_mat4x4 *unwrap_mat4x4(float mat4x4[16]) { return reinterpret_cast<math_mat4x4 *>(mat4x4); }

template <typename T>
using mcrt_vector = std::vector<T, mcrt::scalable_allocator<T>>;

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

struct gltf_scene
{
    mcrt_vector<int> m_nodes;
    mcrt_string m_name;
};

struct gltf_node
{
    int m_camera;
    mcrt_vector<int> m_children;
    int m_skin;
    float m_matrix[16];
    int m_mesh;
    float m_rotation[4];
    float m_scale[3];
    float m_translation[3];
    mcrt_vector<float> m_weights;
    mcrt_string m_name;

    inline gltf_node() : m_camera(-1), m_skin(-1), m_matrix{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, m_mesh(-1), m_rotation{0.0f, 0.0f, 0.0f, 1.0f}, m_scale{1.0f, 1.0f, 1.0f}, m_translation{0.0f, 0.0f, 0.0f} {}
};

struct gltf_buffer
{
    int m_byteLength;
    mcrt_string m_url;

    inline gltf_buffer() : m_byteLength(-1) {}
};

struct gltf_root
{
    int default_scene_index;
    mcrt_vector<struct gltf_scene> m_scenes;
    mcrt_vector<struct gltf_node> m_nodes;
    mcrt_vector<struct gltf_buffer> m_buffers;
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
    assert(0 == user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.size());
    user_defined->m_gltf_root->m_scenes[scene_index].m_nodes.assign(nodes_data, nodes_data + nodes_size);
}

void gltf_yacc_scene_set_name_callback(int scene_index, char const *name_data, int name_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(scene_index < user_defined->m_gltf_root->m_scenes.size());
    assert(0 == user_defined->m_gltf_root->m_scenes[scene_index].m_name.length());
    user_defined->m_gltf_root->m_scenes[scene_index].m_name.assign(name_data, name_size);
}

int gltf_yacc_scene_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int scene_index = user_defined->m_gltf_root->m_scenes.size();
    return scene_index;
}

int gltf_yacc_node_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int node_index = user_defined->m_gltf_root->m_nodes.size();
    user_defined->m_gltf_root->m_nodes.emplace_back();
    return node_index;
}

void gltf_yacc_node_set_camera_callback(int node_index, int camera_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_camera);
    user_defined->m_gltf_root->m_nodes[node_index].m_camera = camera_index;
}

void gltf_yacc_node_set_children_callback(int node_index, int const *nodes_data, int nodes_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_children.size());
    user_defined->m_gltf_root->m_nodes[node_index].m_children.assign(nodes_data, nodes_data + nodes_size);
}

void gltf_yacc_node_set_skin_callback(int node_index, int skin_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_skin);
    user_defined->m_gltf_root->m_nodes[node_index].m_skin = skin_index;
}

void gltf_yacc_node_set_matrix_callback(int node_index, float mat4x4[16], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[2]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[3]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[4]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[5]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[6]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[7]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[8]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[9]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[10]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[11]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[12]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[13]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[14]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_matrix[15]);
    math_store_mat4x4(unwrap_mat4x4(user_defined->m_gltf_root->m_nodes[node_index].m_matrix), math_load_mat4x4(unwrap_mat4x4(mat4x4)));
}

void gltf_yacc_node_set_mesh_callback(int node_index, int mesh_index, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(-1 == user_defined->m_gltf_root->m_nodes[node_index].m_mesh);
    user_defined->m_gltf_root->m_nodes[node_index].m_mesh = mesh_index;
}

void gltf_yacc_node_set_rotation_callback(int node_index, float vec4[4], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[2]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_rotation[3]);
    math_store_vec4(unwrap_vec4(user_defined->m_gltf_root->m_nodes[node_index].m_rotation), math_load_vec4(unwrap_vec4(vec4)));
}

void gltf_yacc_node_set_scale_callback(int node_index, float vec3[3], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[0]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[1]);
    assert(1.0f == user_defined->m_gltf_root->m_nodes[node_index].m_scale[2]);
    math_store_vec3(unwrap_vec3(user_defined->m_gltf_root->m_nodes[node_index].m_scale), math_load_vec3(unwrap_vec3(vec3)));
}

void gltf_yacc_node_set_translation_callback(int node_index, float vec3[3], void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[0]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[1]);
    assert(0.0f == user_defined->m_gltf_root->m_nodes[node_index].m_translation[2]);
    math_store_vec3(unwrap_vec3(user_defined->m_gltf_root->m_nodes[node_index].m_translation), math_load_vec3(unwrap_vec3(vec3)));
}

void gltf_yacc_node_set_weights_callback(int node_index, float *weights_data, int weights_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_nodes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_weights.size());
    user_defined->m_gltf_root->m_nodes[node_index].m_weights.assign(weights_data, weights_data + weights_size);
}

void gltf_yacc_node_set_name_callback(int node_index, char const *name_data, int name_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(node_index < user_defined->m_gltf_root->m_scenes.size());
    assert(0 == user_defined->m_gltf_root->m_nodes[node_index].m_name.length());
    user_defined->m_gltf_root->m_nodes[node_index].m_name.assign(name_data, name_size);
}

int gltf_yacc_node_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int node_index = user_defined->m_gltf_root->m_nodes.size();
    return node_index;
}

int gltf_yacc_buffer_push_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int buffer_index = user_defined->m_gltf_root->m_buffers.size();
    user_defined->m_gltf_root->m_buffers.emplace_back();
    return buffer_index;
}

void gltf_yacc_buffer_set_bufferlength_callback(int buffer_index, int bufferlength, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(buffer_index < user_defined->m_gltf_root->m_buffers.size());
    assert(-1 == user_defined->m_gltf_root->m_buffers[buffer_index].m_byteLength);
    user_defined->m_gltf_root->m_buffers[buffer_index].m_byteLength = bufferlength;
}

void gltf_yacc_buffer_set_url_callback(int buffer_index, char const *name_data, int name_size, void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    assert(buffer_index < user_defined->m_gltf_root->m_buffers.size());
    assert(0 == user_defined->m_gltf_root->m_buffers[buffer_index].m_url.length());
    user_defined->m_gltf_root->m_buffers[buffer_index].m_url.assign(name_data, name_size);
}

int gltf_yacc_buffer_size_callback(void *user_defined_void)
{
    struct gltf_yy_extra_type *user_defined = static_cast<struct gltf_yy_extra_type *>(user_defined_void);
    int buffer_index = user_defined->m_gltf_root->m_buffers.size();
    return buffer_index;
}