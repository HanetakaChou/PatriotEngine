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

#ifndef _PT_IO_H_
#define _PT_IO_H_ 1

#include <pt_common.h>

typedef struct pt_opaque_input_stream_t *pt_input_stream_ref;
enum
{
    PT_INPUT_STREAM_SEEK_SET = 0,
    PT_INPUT_STREAM_SEEK_CUR = 1,
    PT_INPUT_STREAM_SEEK_END = 2
};
typedef pt_input_stream_ref(PT_PTR *pt_input_stream_init_callback)(char const *);
typedef int(PT_PTR *pt_input_stream_stat_size_callback)(pt_input_stream_ref, int64_t *);
typedef intptr_t(PT_PTR *pt_input_stream_read_callback)(pt_input_stream_ref, void *, size_t);
typedef int64_t(PT_PTR *pt_input_stream_seek_callback)(pt_input_stream_ref, int64_t, int);
typedef void(PT_PTR *pt_input_stream_destroy_callback)(pt_input_stream_ref);

typedef struct pt_opaque_output_stream_t *pt_output_stream_ref;
typedef pt_output_stream_ref(PT_PTR *pt_output_stream_init_callback)(char const *);
typedef intptr_t(PT_PTR *pt_output_stream_write_callback)(pt_output_stream_ref, void *, size_t);
typedef void(PT_PTR *pt_output_stream_destroy_callback)(pt_output_stream_ref);

#endif