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

#ifndef _PT_GFX_MESH_BASE_GLTF_PARSE_H_
#define _PT_GFX_MESH_BASE_GLTF_PARSE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_scalable_allocator.h>
#include <vector>
#include <string>

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

enum gltf_target
{
    GLTF_TARGET_UNKNOWN = -1,
    GLTF_TARGET_ARRAY_BUFFER = 34962,        //vertex
    GLTF_TARGET_ELEMENT_ARRAY_BUFFER = 34963 //index
};

struct gltf_bufferview
{
    int m_buffer;
    int m_byteOffset;
    int m_byteLength;
    int m_byteStride;
    enum gltf_target m_target;
    mcrt_string m_name;

    inline gltf_bufferview() : m_buffer(-1), m_byteOffset(0), m_byteLength(-1), m_byteStride(-1), m_target(GLTF_TARGET_UNKNOWN) {}
};

enum gltf_component_type
{
    GLTF_COMPONENT_TYPE_UNKNOWN = -1,
    GLTF_COMPONENT_TYPE_BYTE = 5120,
    GLTF_COMPONENT_TYPE_UNSIGNED_BYTE = 5121,
    GLTF_COMPONENT_TYPE_SHORT = 5122,
    GLTF_COMPONENT_TYPE_UNSIGNED_SHORT = 5123,
    GLTF_COMPONENT_TYPE_UNSIGNED_INT = 5125,
    GLTF_COMPONENT_TYPE_FLOAT = 5126
};

enum gltf_type
{
    GLTF_TYPE_UNKNOWN = -1,
    GLTF_TYPE_SCALAR = 1,
    GLTF_TYPE_VEC2 = 2,
    GLTF_TYPE_VEC3 = 3,
    GLTF_TYPE_VEC4 = 4,
    GLTF_TYPE_MAT2 = 5,
    GLTF_TYPE_MAT3 = 9,
    GLTF_TYPE_MAT4 = 16
};

struct gltf_accessor
{
    int m_bufferview;
    int m_byteoffset;
    enum gltf_component_type m_componenttype;
    bool m_normalized;
    int m_count;
    enum gltf_type m_type;
    float m_max[16];
    float m_min[16];
    // sparse
    mcrt_string m_name;
    inline gltf_accessor() : m_bufferview(-1), m_byteoffset(0), m_componenttype(GLTF_COMPONENT_TYPE_UNKNOWN), m_normalized(false), m_count(-1), m_type(GLTF_TYPE_UNKNOWN), m_max{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, m_min{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} {}
};

struct gltf_attributes
{
    int m_position;
    int m_normal;
    int m_tangent;
    int m_texcoord_0;
    int m_texcoord_1;
    int m_color_0;
    int m_joints_0;
    int m_weights_0;
    inline gltf_attributes() : m_position(-1), m_normal(-1), m_tangent(-1), m_texcoord_0(-1), m_texcoord_1(-1), m_color_0(-1), m_joints_0(-1), m_weights_0(-1) {}
};

enum gltf_mode
{
    GLTF_MODE_POINTS = 0,
    GLTF_MODE_LINES = 1,
    GLTF_MODE_LINE_LOOP = 2,
    GLTF_MODE_LINE_STRIP = 3,
    GLTF_MODE_TRIANGLES = 4,
    GLTF_MODE_TRIANGLE_STRIP = 5,
    GLTF_MODE_TRIANGLE_FAN = 6
};

struct gltf_primitive
{
    struct gltf_attributes m_attributes;
    int m_indices;
    int m_material;
    enum gltf_mode m_mode;
    // targets
};

struct gltf_mesh
{
    mcrt_vector<struct gltf_primitive> m_primitives;
    mcrt_vector<float> m_weights;
    mcrt_string m_name;
};

struct gltf_root
{
    int m_scene_index;
    mcrt_vector<struct gltf_scene> m_scenes;
    mcrt_vector<struct gltf_node> m_nodes;
    mcrt_vector<struct gltf_buffer> m_buffers;
    mcrt_vector<struct gltf_bufferview> m_bufferviews;
    mcrt_vector<struct gltf_accessor> m_accessors;
    mcrt_vector<struct gltf_mesh> m_meshes;
};

bool gltf_parse_input_stream(struct gltf_root *out_gltf_root, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), void *error_callback_data, void (*error_callback)(int line, int column, char const *msg, void *error_callback_data));

#endif