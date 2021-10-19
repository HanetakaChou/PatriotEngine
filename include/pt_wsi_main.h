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

typedef struct _pt_gfx_app_t_ *pt_wsi_app_ref;

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef PT_POSIX
#ifdef PT_POSIX_LINUX
#ifdef PT_POSIX_LINUX_ANDROID
    PT_ATTR_WSI void PT_CALL pt_wsi_main(struct ANativeActivity *native_activity, void *, size_t, pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(pt_gfx_connection_ref, char const *internal_data_path), int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref));
#elif defined(PT_POSIX_LINUX_X11)
    PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char *argv[], pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref));
#else
#error Unknown Linux Platform
#endif
#elif defined(PT_POSIX_MACH)
#ifdef PT_POSIX_MATH_IOS
    PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char *argv[], pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref));
#elif defined(PT_POSIX_MATH_OSX)
    PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char const *argv[], pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref));
#else
#error Unknown Mach Platform
#endif
#else
#error Unknown Posix Platform
#endif
#elif defined(PT_WIN32)
#ifdef PT_WIN32_DESKTOP
    PT_ATTR_WSI int PT_CALL pt_wsi_main(wchar_t *cmd_line, int cmd_show, pt_wsi_app_ref(PT_PTR *pt_wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *pt_wsi_app_main_callback)(pt_wsi_app_ref));
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