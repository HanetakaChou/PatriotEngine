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

#include <vector>
#include "pt_wsi_app_base.h"
#include <pt_mcrt_thread.h>

extern bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename);
extern bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename);

static pt_gfx_connection_ref my_gfx_connection = NULL;
void wsi_app_base::init(pt_gfx_connection_ref gfx_connection)
{
    my_gfx_connection = gfx_connection;
}

//static gfx_texture_ref my_texture1 = NULL;
//static gfx_texture_ref my_texture2 = NULL;
static gfx_mesh_ref my_mesh = NULL;
static gfx_node_ref my_node = NULL;
static gfx_texture_ref my_texture = NULL;
static gfx_material_ref my_material = NULL;

int wsi_app_base::main()
{
    gfx_mesh_ref my_mesh1 = gfx_connection_create_mesh(my_gfx_connection);
    //gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.gltf");
    gfx_mesh_read_file(my_gfx_connection, my_mesh1, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.bin");

    my_mesh = gfx_connection_create_mesh(my_gfx_connection);
    //gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.gltf");
    gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.bin");
    //gfx_mesh_destroy(my_gfx_connection, my_mesh);

    my_node = gfx_connection_create_node(my_gfx_connection);
    gfx_node_set_mesh(my_gfx_connection, my_node, my_mesh);

    gfx_texture_ref my_texture2 = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture2, "lenna/lena_std_directx_tex.dds");

    my_texture = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture, "lenna/l_hires_astc.pvr");

    gfx_texture_ref my_texture3 = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture3, "lenna/lena_std_rgba.pvr");

    my_material = gfx_connection_create_material(my_gfx_connection);
    gfx_material_init_with_texture(my_gfx_connection, my_material, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture);

    gfx_node_set_material(my_gfx_connection, my_node, my_material);

    // material hold the refcount
    gfx_texture_destroy(my_gfx_connection, my_texture);

    // node hold the material
    gfx_material_destroy(my_gfx_connection, my_material);

    gfx_texture_destroy(my_gfx_connection, my_texture2);

    gfx_texture_destroy(my_gfx_connection, my_texture3);

    gfx_mesh_destroy(my_gfx_connection, my_mesh1);

#if 1
    mcrt_os_sleep(3000);

    bool has_set = false;

    std::vector<gfx_texture_ref> my_textures;

    unsigned rand_buf = (unsigned)time(NULL);

    for (int i = 0; i < 500; ++i)
    {
        long int r1 = rand_r(&rand_buf);

        if (0 == r1 % 9 || 1 == r1 % 9 || 2 == r1 % 9 || 3 == r1 % 9)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "lenna/lena_std_rgba.pvr");
            my_textures.push_back(my_texture);

            if ((i > 100) && (!has_set))
            {
                gfx_material_ref my_material = gfx_connection_create_material(my_gfx_connection);
                gfx_material_init_with_texture(my_gfx_connection, my_material, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture);

                gfx_node_set_material(my_gfx_connection, my_node, my_material);

                gfx_material_destroy(my_gfx_connection, my_material);

                has_set = true;
            }
        }
        else if (4 == r1 % 9 || 5 == r1 % 9 || 6 == r1 % 9 || 7 == r1 % 9)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "lenna/l_hires_directx_tex.dds");
            my_textures.push_back(my_texture);
        }
        else
        {
            if (!my_textures.empty())
            {
                long int r2 = rand_r(&rand_buf);
                size_t vec_idx = (r2 % (my_textures.size() / 2 + 1));
                gfx_texture_ref my_texture = my_textures[vec_idx];
                my_textures[vec_idx] = my_textures.back();
                my_textures.pop_back();
                gfx_texture_destroy(my_gfx_connection, my_texture);
            }
            else
            {
                gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_gfx_connection, my_texture, "lenna/l_hires_nvidia_texture_tools.dds");
                my_textures.push_back(my_texture);
            }
        }
    }

    mcrt_os_sleep(3000);

    for (gfx_texture_ref my_texture : my_textures)
    {
        gfx_texture_destroy(my_gfx_connection, my_texture);
    }
#endif

    gfx_texture_ref my_texture1 = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture1, "lenna/l_hires_rgba.pvr");

    gfx_material_ref my_material1 = gfx_connection_create_material(my_gfx_connection);
    gfx_material_init_with_texture(my_gfx_connection, my_material1, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture1);

    gfx_texture_destroy(my_gfx_connection, my_texture1);

    gfx_node_set_material(my_gfx_connection, my_node, my_material1);

    gfx_material_destroy(my_gfx_connection, my_material1);

    mcrt_os_sleep(3000);

    gfx_mesh_destroy(my_gfx_connection, my_mesh);
    //gfx_node_destroy(my_gfx_connection, my_node);

#if 0
    sleep(15);

    my_mesh = gfx_connection_create_mesh(my_gfx_connection);
    //gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.gltf");
    gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.bin");
    //gfx_mesh_destroy(my_gfx_connection, my_mesh);

    my_node = gfx_connection_create_node(my_gfx_connection);
    gfx_node_set_mesh(my_gfx_connection, my_node, my_mesh);

    sleep(15);

    gfx_mesh_destroy(my_gfx_connection, my_mesh);
    gfx_node_destroy(my_gfx_connection, my_node);
#endif

    return 0;
}
