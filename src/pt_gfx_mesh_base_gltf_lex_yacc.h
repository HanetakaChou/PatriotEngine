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

#ifndef _PT_GFX_MESH_BASE_GLTF_LEX_YACC_H_
#define _PT_GFX_MESH_BASE_GLTF_LEX_YACC_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif
    void *gltf_lex_yacc_alloc_callback(size_t size, void *user_defined);
    void *gltf_lex_yacc_realloc_callback(void *ptr, size_t size, void *user_defined);
    void gltf_lex_yacc_free_callback(void *ptr, void *user_defined);

    struct temp_string_version_t *gltf_lex_yacc_temp_string_init_callback(void *user_defined);
    void gltf_lex_yacc_temp_string_set_callback(struct temp_string_version_t *temp_string_version, char const *text, int length, void *user_defined);
    void gltf_lex_yacc_temp_string_destroy_callback(struct temp_string_version_t *temp_string_version, void *user_defined);

    ptrdiff_t gltf_lex_input_callback(void *input_stream, void *buf, size_t size, void *user_defined);
    void gltf_lex_fatal_error_callback(int line, int column, char const *msg, void *user_defined);
    int gltf_lex_memcmp_callback(void const *ptr1, void const *ptr2, size_t num, void *user_defined);
    
    void gltf_yacc_error_callback(int line, int column, char const *msg, void *user_defined);
    struct temp_int_array_version_t *gltf_yacc_temp_int_array_init_callback(void *user_defined);
    void gltf_yacc_temp_int_array_push_callback(struct temp_int_array_version_t *temp_int_array_version, int int_element, void *user_defined);
    void gltf_yacc_temp_int_array_destroy_callback(struct temp_int_array_version_t *temp_int_array_version, void *user_defined);
    struct temp_float_array_version_t *gltf_yacc_temp_float_array_init_callback(void *user_defined);
    void gltf_yacc_temp_float_array_push_callback(struct temp_float_array_version_t *temp_float_array_version, float float_element, void *user_defined);
    void gltf_yacc_temp_float_array_destroy_callback(struct temp_float_array_version_t *temp_float_array_version, void *user_defined);
    void gltf_yacc_gltf_set_scene_index_callback(int scene_index, void *user_defined);
    int gltf_yacc_scene_push_callback(void *user_defined);
    void gltf_yacc_scene_set_nodes_callback(int scene_index, struct temp_int_array_version_t *temp_int_array_version, void *user_defined);
    void gltf_yacc_scene_set_name_callback(int scene_index, struct temp_string_version_t *temp_string_version, void *user_defined);
    int gltf_yacc_scene_size_callback(void *user_defined);
    int gltf_yacc_node_push_callback(void *user_defined);
    void gltf_yacc_node_set_camera_callback(int node_index, int camera_index, void *user_defined);
    void gltf_yacc_node_set_children_callback(int node_index, struct temp_int_array_version_t *temp_int_array_version, void *user_defined);
    void gltf_yacc_node_set_skin_callback(int node_index, int skin_index, void *user_defined);
    void gltf_yacc_node_set_matrix_callback(int node_index, float mat4x4[16], void *user_defined);
    void gltf_yacc_node_set_mesh_callback(int node_index, int mesh_index, void *user_defined);
    void gltf_yacc_node_set_rotation_callback(int node_index, float vec4[4], void *user_defined);
    void gltf_yacc_node_set_scale_callback(int node_index, float vec3[3], void *user_defined);
    void gltf_yacc_node_set_translation_callback(int node_index, float vec3[3], void *user_defined);
    void gltf_yacc_node_set_weights_callback(int node_index, struct temp_float_array_version_t *temp_float_array_version, void *user_defined);
    void gltf_yacc_node_set_name_callback(int node_index, struct temp_string_version_t *temp_string_version, void *user_defined);
    int gltf_yacc_node_size_callback(void *user_defined);
    int gltf_yacc_buffer_push_callback(void *user_defined);
    void gltf_yacc_buffer_set_bytelength_callback(int buffer_index, int bytelength, void *user_defined);
    void gltf_yacc_buffer_set_url_callback(int buffer_index, struct temp_string_version_t *temp_string_version, void *user_defined);
    int gltf_yacc_buffer_size_callback(void *user_defined);
    int gltf_yacc_bufferview_push_callback(void *user_defined);
    void gltf_yacc_bufferview_set_buffer_callback(int bufferview_index, int buffer_index, void *user_defined);
    void gltf_yacc_bufferview_set_byteoffset_callback(int bufferview_index, int byteoffset, void *user_defined);
    void gltf_yacc_bufferview_set_bytelength_callback(int bufferview_index, int bytelength, void *user_defined);
    void gltf_yacc_bufferview_set_bytestride_callback(int bufferview_index, int bytestride, void *user_defined);
    void gltf_yacc_bufferview_set_target_callback(int bufferview_index, int target, void *user_defined);
    int gltf_yacc_bufferview_size_callback(void *user_defined);
    int gltf_yacc_accessor_push_callback(void *user_defined);
    void gltf_yacc_accessor_set_bufferview_callback(int accessor_index, int bufferview_index, void *user_defined);
    void gltf_yacc_accessor_set_byteoffset_callback(int accessor_index, int byteoffset, void *user_defined);
    void gltf_yacc_accessor_set_componenttype_callback(int accessor_index, int componenttype, void *user_defined);
    void gltf_yacc_accessor_set_normalized_callback(int accessor_index, bool normalized, void *user_defined);
    void gltf_yacc_accessor_set_count_callback(int accessor_index, int count, void *user_defined);
    void gltf_yacc_accessor_set_type_callback(int accessor_index, int type, void *user_defined);
    void gltf_yacc_accessor_set_max_callback(int accessor_index, float max[16], void *user_defined);
    void gltf_yacc_accessor_set_min_callback(int accessor_index, float min[16], void *user_defined);
    void gltf_yacc_accessor_set_name_callback(int accessor_index, struct temp_string_version_t *temp_string_version, void *user_defined);
    int gltf_yacc_accessor_size_callback(void *user_defined);
#ifdef __cplusplus
}
#endif

#endif