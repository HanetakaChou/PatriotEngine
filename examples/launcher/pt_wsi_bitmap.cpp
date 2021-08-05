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
#include <assert.h>

#include <pt_gfx_connection.h>
#include "pt_gfx_connection_utils.h"

#if defined(PT_POSIX_MACH)
#include <pt_apple_sdk_posix_mach_foundation.h>
#endif

#include <string>

int main(int argc, char **argv)
{
    
#if defined(PT_POSIX_MACH)
    //Lunarg Vulkan SDK
    {
        void *__here_auto_release_pool_object = AutoReleasePool_Push();

        std::string my_path = NSString_UTF8String(NSBundle_resourcePath(NSBundle_mainBundle()));

        int ret_vk_layer_path = setenv("VK_LAYER_PATH", my_path.c_str(), 1);
        assert(0 == ret_vk_layer_path);

        my_path += "/MoltenVK_icd.json";

        int ret_vk_icd_file_names = setenv("VK_ICD_FILENAMES", my_path.c_str(), 1);
        assert(0 == ret_vk_icd_file_names);

        AutoReleasePool_Pop(__here_auto_release_pool_object);
    }
#endif

    gfx_connection_ref gfx_connection = gfx_connection_init(NULL, NULL, NULL);

    gfx_texture_ref texture1 = gfx_connection_create_texture(gfx_connection);
    gfx_texture_read_file(texture1, "third_party/assets/lenna/l_hires_rgba.pvr");
    //gfx_texture_read_file(texture1, "third_party/assets/lenna/l_hires_directx_tex.dds");
    //gfx_connection_wsi_on_resized(gfx_connection, NULL, 0, 0);

    gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection, NULL, 0, 0);

    gfx_texture_ref texture2 = gfx_connection_create_texture(gfx_connection);
    gfx_texture_read_file(texture2, "third_party/assets/lenna/l_hires_directx_tex.dds");

    gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection, NULL, 0, 0);
    gfx_connection_wsi_on_redraw_needed_acquire(gfx_connection, NULL, 0, 0);

    gfx_texture_destroy(texture1);
    gfx_texture_destroy(texture2);
    gfx_connection_destroy(gfx_connection);

    return 0;
}
