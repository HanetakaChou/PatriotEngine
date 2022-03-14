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

#ifndef _PT_GFX_MESH_BASE_LOAD_PMX_H_
#define _PT_GFX_MESH_BASE_LOAD_PMX_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

extern bool mesh_load_pmx_header_from_input_stream(struct pt_gfx_mesh_neutral_header_t *out_neutral_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

extern bool mesh_load_pmx_primitive_headers_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t *out_neutral_primitive_headers, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

extern bool mesh_load_pmx_primitive_data_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t const *neutral_primitive_headers_for_validate, void *staging_pointer, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

#endif