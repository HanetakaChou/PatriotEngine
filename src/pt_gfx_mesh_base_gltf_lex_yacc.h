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
    void *gltf_alloc_callback(size_t size, void *user_defined);
    void *gltf_realloc_callback(void *ptr, size_t size, void *user_defined);
    void gltf_free_callback(void *ptr, void *user_defined);

    ptrdiff_t gltf_lex_input_callback(void *input_stream, void *buf, size_t size, void *user_defined);
    void gltf_lex_fatal_error_callback(int line, int column, char const *msg, void *user_defined);

    int gltf_lex_memcmp_callback(void const *ptr1, void const *ptr2, size_t num, void *user_defined);
    void gltf_lex_memcpy_callback(char *dest, char const *src, int count, void *user_defined);

    void gltf_yacc_error_callback(int line, int column, char const *msg, void *user_defined);
#ifdef __cplusplus
}
#endif

#endif