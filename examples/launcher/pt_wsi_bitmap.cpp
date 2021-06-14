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
#include <assert.h>
#include "pt_gfx_connection_utils.h"

int main(int argc, char **argv)
{

    gfx_connection_ref gfx_connection = gfx_connection_init(NULL, NULL, NULL);

    gfx_texture_ref texture = gfx_connection_create_texture(gfx_connection);
    gfx_texture_read_file(texture, "third_party/assets/lenna/l_hires_directx_tex.dds");

    //gfx_connection_wsi_on_resized(gfx_connection, NULL, 0, 0);

    gfx_connection_destroy(gfx_connection);

    return 0;
}
