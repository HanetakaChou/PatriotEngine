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

#ifndef _PT_WSI_MAIN_H_
#define _PT_WSI_MAIN_H_ 1

#include "pt_wsi_common.h"
#include "pt_gfx_connection.h"

typedef struct pt_wsi_opaque_app_t *pt_wsi_app_ref;

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef PT_POSIX
#ifdef PT_POSIX_LINUX
#ifdef PT_POSIX_LINUX_ANDROID
    typedef pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(struct ANativeActivity *native_activity, void *, size_t, pt_gfx_connection_ref);
    typedef int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref);

    PT_ATTR_WSI void PT_CALL pt_wsi_main(
        struct ANativeActivity *native_activity, void *, size_t,
        pt_gfx_input_stream_init_callback cache_input_stream_init_callback, pt_gfx_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_gfx_input_stream_read_callback cache_input_stream_read_callback, pt_gfx_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_gfx_output_stream_init_callback cache_output_stream_init_callback, pt_gfx_output_stream_write_callback cache_output_stream_write_callback, pt_gfx_output_stream_destroy_callback cache_output_stream_destroy_callback,
        pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback);
#elif defined(PT_POSIX_LINUX_X11)
    typedef pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(int argc, char *argv[], pt_gfx_connection_ref);
    typedef int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref);

    PT_ATTR_WSI int PT_CALL pt_wsi_main(
        int argc, char *argv[],
        pt_gfx_input_stream_init_callback cache_input_stream_init_callback, pt_gfx_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_gfx_input_stream_read_callback cache_input_stream_read_callback, pt_gfx_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_gfx_output_stream_init_callback cache_output_stream_init_callback, pt_gfx_output_stream_write_callback cache_output_stream_write_callback, pt_gfx_output_stream_destroy_callback cache_output_stream_destroy_callback,
        pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback);
#else
#error Unknown Linux Platform
#endif
#elif defined(PT_POSIX_MACH)
#ifdef PT_POSIX_MATH_IOS
    typedef pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(int argc, char *argv[], pt_gfx_connection_ref);
    typedef int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref);

    PT_ATTR_WSI int PT_CALL pt_wsi_main(
        int argc, char *argv[],
        pt_gfx_input_stream_init_callback cache_input_stream_init_callback, pt_gfx_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_gfx_input_stream_read_callback cache_input_stream_read_callback, pt_gfx_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_gfx_output_stream_init_callback cache_output_stream_init_callback, pt_gfx_output_stream_write_callback cache_output_stream_write_callback, pt_gfx_output_stream_destroy_callback cache_output_stream_destroy_callback,
        pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback);
#elif defined(PT_POSIX_MATH_OSX)
    typedef pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(int argc, char const *argv[], pt_gfx_connection_ref);
    typedef int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref);

    PT_ATTR_WSI int PT_CALL pt_wsi_main(
        int argc, char const *argv[],
        pt_gfx_input_stream_init_callback cache_input_stream_init_callback, pt_gfx_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_gfx_input_stream_read_callback cache_input_stream_read_callback, pt_gfx_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_gfx_output_stream_init_callback cache_output_stream_init_callback, pt_gfx_output_stream_write_callback cache_output_stream_write_callback, pt_gfx_output_stream_destroy_callback cache_output_stream_destroy_callback,
        pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback);
#else
#error Unknown Mach Platform
#endif
#else
#error Unknown Posix Platform
#endif
#elif defined(PT_WIN32)
#ifdef PT_WIN32_DESKTOP
    typedef pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(int argc, char *argv[], pt_gfx_connection_ref);
    typedef int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref);
    PT_ATTR_WSI int PT_CALL pt_wsi_main(
        int argc, char* argv[],
        pt_gfx_input_stream_init_callback cache_input_stream_init_callback, pt_gfx_input_stream_stat_size_callback cache_input_stream_stat_size_callback, pt_gfx_input_stream_read_callback cache_input_stream_read_callback, pt_gfx_input_stream_destroy_callback cache_input_stream_destroy_callback,
        pt_gfx_output_stream_init_callback cache_output_stream_init_callback, pt_gfx_output_stream_write_callback cache_output_stream_write_callback, pt_gfx_output_stream_destroy_callback cache_output_stream_destroy_callback,
        pt_wsi_app_init_callback app_init_callback, pt_wsi_app_main_callback app_main_callback);
#elif defined(PT_WIN32_RUNTIME)
//
#else
#error Unknown Win32 Platform
#endif
#else
#error Unknown Platform
#endif

#ifdef __cplusplus
}
#endif

#endif
