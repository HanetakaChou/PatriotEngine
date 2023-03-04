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

#include <time.h>
#include <vector>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_map.h>
#include <pt_mcrt_string.h>
#include "pt_wsi_app_base.h"
#include "../asset/material_asset_load_pmx.h"

static pt_gfx_connection_ref my_gfx_connection = NULL;
void launcher_app::init(pt_gfx_connection_ref gfx_connection)
{
    my_gfx_connection = gfx_connection;
}

static bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture, char const *initial_filename);
static bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, char const *initial_filename);
static bool load_material_count_from_file(char const *initial_filename, size_t *out_material_count);
static bool load_material_texture_paths_from_file(char const *initial_filename, char **out_material_texture_paths, size_t *out_material_texture_path_size);

int launcher_app::main()
{
    // char const* my_filename = "Genshin_Impact-Ayaka_kamisato/ayaka_kamisato.pmx";
    // char const* my_filename_dirname = "Genshin_Impact-Ayaka_kamisato";
    char const *my_filename = "Honkai_Impact_3rd-Herrscher_of_Thunder/herrscher_of_thunder.pmx";
    char const *my_filename_dirname = "Honkai_Impact_3rd-Herrscher_of_Thunder";

    pt_gfx_mesh_ref my_mesh = pt_gfx_connection_create_mesh(my_gfx_connection);

    gfx_mesh_read_file(my_gfx_connection, my_mesh, my_filename);

    size_t material_count;
    load_material_count_from_file(my_filename, &material_count);

    pt_gfx_node_ref my_node = pt_gfx_connection_create_node(my_gfx_connection, material_count);

    pt_gfx_node_set_mesh(my_gfx_connection, my_node, my_mesh);

    // node hold the mesh
    pt_gfx_mesh_destroy(my_gfx_connection, my_mesh);

    size_t *material_texture_path_size = static_cast<size_t *>(mcrt_aligned_malloc(sizeof(size_t) * material_count, alignof(size_t)));
    load_material_texture_paths_from_file(my_filename, NULL, material_texture_path_size);

    char **material_texture_paths = static_cast<char **>(mcrt_aligned_malloc(sizeof(char *) * material_count, alignof(char *)));
    for (size_t material_index = 0U; material_index < material_count; ++material_index)
    {
        material_texture_paths[material_index] = static_cast<char *>(mcrt_aligned_malloc(sizeof(char) * material_texture_path_size[material_index], alignof(char)));
    }
    load_material_texture_paths_from_file(my_filename, material_texture_paths, NULL);

    mcrt_map<mcrt_string, pt_gfx_texture_ref> my_textures;
    for (size_t material_index = 0U; material_index < material_count; ++material_index)
    {
        mcrt_string material_texture_path;
        material_texture_path.append(my_filename_dirname);
        material_texture_path.append("/");
        material_texture_path.append(material_texture_paths[material_index]);

        size_t material_texture_path_length = material_texture_path.length();
        if ((material_texture_path_length >= 4U) &&
            ('.' == material_texture_path[material_texture_path_length - 4U]) &&
            ('p' == material_texture_path[material_texture_path_length - 3U] || 'P' == material_texture_path[material_texture_path_length - 3U]) &&
            ('n' == material_texture_path[material_texture_path_length - 2U] || 'N' == material_texture_path[material_texture_path_length - 2U]) &&
            ('g' == material_texture_path[material_texture_path_length - 1U] || 'G' == material_texture_path[material_texture_path_length - 1U]))
        {
            material_texture_path[material_texture_path_length - 3U] = 'd';
            material_texture_path[material_texture_path_length - 2U] = 'd';
            material_texture_path[material_texture_path_length - 1U] = 's';
        }

        pt_gfx_texture_ref my_texture;
        {
            mcrt_map<mcrt_string, pt_gfx_texture_ref>::const_iterator material_texture_iterator = my_textures.find(material_texture_path);
            if (my_textures.end() != material_texture_iterator)
            {
                my_texture = material_texture_iterator->second;
            }
            else
            {
                my_texture = pt_gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_gfx_connection, my_texture, material_texture_path.c_str());

                my_textures.insert(material_texture_iterator, std::make_pair(std::move(material_texture_path), my_texture));
            }
        }

        pt_gfx_material_ref my_material = pt_gfx_connection_create_material(my_gfx_connection);
        pt_gfx_material_init_with_texture(my_gfx_connection, my_material, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, const_cast<pt_gfx_texture_ref *>(&my_texture));

        pt_gfx_node_set_material(my_gfx_connection, my_node, material_index, my_material);

        // node hold the material
        pt_gfx_material_destroy(my_gfx_connection, my_material);
    }

    for (auto const &texture_path_pair : my_textures)
    {
        // material hold the refcount
        pt_gfx_texture_destroy(my_gfx_connection, texture_path_pair.second);
    }
    my_textures.clear();

    for (size_t material_index = 0U; material_index < material_count; ++material_index)
    {
        mcrt_aligned_free(material_texture_paths[material_index]);
    }

    mcrt_aligned_free(material_texture_paths);

    mcrt_aligned_free(material_texture_path_size);

    mcrt_os_sleep(3000);

    pt_gfx_node_destroy(my_gfx_connection, my_node);

#if 0
    //static pt_gfx_texture_ref my_texture1 = NULL;
    //static pt_gfx_texture_ref my_texture2 = NULL;
    static pt_gfx_mesh_ref my_mesh = NULL;
    static pt_gfx_node_ref my_node = NULL;
    static pt_gfx_texture_ref my_texture = NULL;
    static pt_gfx_material_ref my_material = NULL;

    pt_gfx_mesh_ref my_mesh1 = pt_gfx_connection_create_mesh(my_gfx_connection);
    gfx_mesh_read_file(my_gfx_connection, my_mesh1, "genshin_impact/ayaka_kamisato/ayaka_kamisato.pmx");

    my_mesh = pt_gfx_connection_create_mesh(my_gfx_connection);
    gfx_mesh_read_file(my_gfx_connection, my_mesh, "genshin_impact/ayaka_kamisato/ayaka_kamisato.pmx");

    my_node = pt_gfx_connection_create_node(my_gfx_connection);
    pt_gfx_node_set_mesh(my_gfx_connection, my_node, my_mesh);

    pt_gfx_texture_ref my_texture2 = pt_gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture2, "genshin_impact/ayaka_kamisato/cloth.pvr");

    my_texture = pt_gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture, "genshin_impact/ayaka_kamisato/cloth.dds");

    pt_gfx_texture_ref my_texture3 = pt_gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture3, "genshin_impact/ayaka_kamisato/face.dds");

    my_material = pt_gfx_connection_create_material(my_gfx_connection);
    pt_gfx_material_init_with_texture(my_gfx_connection, my_material, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture);

    pt_gfx_node_set_material(my_gfx_connection, my_node, my_material);

    // material hold the refcount
    pt_gfx_texture_destroy(my_gfx_connection, my_texture);

    // node hold the material
    pt_gfx_material_destroy(my_gfx_connection, my_material);

    pt_gfx_texture_destroy(my_gfx_connection, my_texture2);

    pt_gfx_texture_destroy(my_gfx_connection, my_texture3);

    pt_gfx_mesh_destroy(my_gfx_connection, my_mesh1);

#if 1
    mcrt_os_sleep(3000);

    bool has_set = false;

    std::vector<pt_gfx_texture_ref> my_textures;

    srand((unsigned)time(NULL));

    for (int i = 0; i < 500; ++i)
    {
        long int r1 = rand();

        if (0 == r1 % 9 || 1 == r1 % 9 || 2 == r1 % 9 || 3 == r1 % 9)
        {
            pt_gfx_texture_ref my_texture = pt_gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "genshin_impact/ayaka_kamisato/cloth.dds");
            my_textures.push_back(my_texture);

            if ((i > 100) && (!has_set))
            {
                pt_gfx_material_ref my_material = pt_gfx_connection_create_material(my_gfx_connection);
                pt_gfx_material_init_with_texture(my_gfx_connection, my_material, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture);

                pt_gfx_node_set_material(my_gfx_connection, my_node, my_material);

                pt_gfx_material_destroy(my_gfx_connection, my_material);

                has_set = true;
            }
        }
        else if (4 == r1 % 9 || 5 == r1 % 9 || 6 == r1 % 9 || 7 == r1 % 9)
        {
            pt_gfx_texture_ref my_texture = pt_gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "genshin_impact/ayaka_kamisato/face.dds");
            my_textures.push_back(my_texture);
        }
        else
        {
            if (!my_textures.empty())
            {
                long int r2 = rand();
                size_t vec_idx = (r2 % (my_textures.size() / 2 + 1));
                pt_gfx_texture_ref my_texture = my_textures[vec_idx];
                my_textures[vec_idx] = my_textures.back();
                my_textures.pop_back();
                pt_gfx_texture_destroy(my_gfx_connection, my_texture);
            }
            else
            {
                pt_gfx_texture_ref my_texture = pt_gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_gfx_connection, my_texture, "genshin_impact/ayaka_kamisato/hair.dds");
                my_textures.push_back(my_texture);
            }
        }
    }

    mcrt_os_sleep(3000);

    for (pt_gfx_texture_ref my_texture : my_textures)
    {
        pt_gfx_texture_destroy(my_gfx_connection, my_texture);
    }
#endif

#if 1
    pt_gfx_texture_ref my_texture1 = pt_gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_gfx_connection, my_texture1, "genshin_impact/ayaka_kamisato/flesh.dds");

    pt_gfx_material_ref my_material1 = pt_gfx_connection_create_material(my_gfx_connection);
    pt_gfx_material_init_with_texture(my_gfx_connection, my_material1, GFX_MATERIAL_MODEL_PBR_SPECULAR_GLOSSINESS, 1U, &my_texture1);

    pt_gfx_texture_destroy(my_gfx_connection, my_texture1);

    pt_gfx_node_set_material(my_gfx_connection, my_node, my_material1);

    pt_gfx_material_destroy(my_gfx_connection, my_material1);

    mcrt_os_sleep(3000);

    pt_gfx_mesh_destroy(my_gfx_connection, my_mesh);
    pt_gfx_node_destroy(my_gfx_connection, my_node);
#endif

#if 0
    mcrt_os_sleep(15);

    my_mesh = pt_gfx_connection_create_mesh(my_gfx_connection);
    gfx_mesh_read_file(my_gfx_connection, my_mesh, "genshin_impact/ayaka_kamisato/ayaka_kamisato.pmx");

    my_node = pt_gfx_connection_create_node(my_gfx_connection);
    pt_gfx_node_set_mesh(my_gfx_connection, my_node, my_mesh);

    mcrt_os_sleep(15);

    pt_gfx_mesh_destroy(my_gfx_connection, my_mesh);
    pt_gfx_node_destroy(my_gfx_connection, my_node);
#endif

#endif

    return 0;
}

extern pt_input_stream_ref PT_PTR asset_input_stream_init_callback(char const *);
extern intptr_t PT_PTR asset_input_stream_read_callback(pt_input_stream_ref, void *, size_t);
extern int64_t PT_PTR asset_input_stream_seek_callback(pt_input_stream_ref, int64_t, int);
extern void PT_PTR asset_input_stream_destroy_callback(pt_input_stream_ref);

static bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture, char const *initial_filename)
{
    return pt_gfx_texture_read_input_stream(
        gfx_connection,
        texture,
        initial_filename,
        asset_input_stream_init_callback,
        asset_input_stream_read_callback,
        asset_input_stream_seek_callback,
        asset_input_stream_destroy_callback);
}

static bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, char const *initial_filename)
{
    return pt_gfx_mesh_read_input_stream(
        gfx_connection,
        mesh,
        initial_filename,
        asset_input_stream_init_callback,
        asset_input_stream_read_callback,
        asset_input_stream_seek_callback,
        asset_input_stream_destroy_callback);
}

static bool load_material_count_from_file(char const *initial_filename, size_t *out_material_count)
{
    pt_input_stream_ref input_stream = asset_input_stream_init_callback(initial_filename);
    if (pt_input_stream_ref(-1) == input_stream)
    {
        return false;
    }

    if (!material_asset_load_pmx_header_from_input_stream(input_stream, asset_input_stream_read_callback, asset_input_stream_seek_callback, out_material_count))
    {
        asset_input_stream_destroy_callback(input_stream);
        return false;
    }

    asset_input_stream_destroy_callback(input_stream);
    return true;
}

static bool load_material_texture_paths_from_file(char const *initial_filename, char **out_material_texture_paths, size_t *out_material_texture_path_size)
{
    pt_input_stream_ref input_stream = asset_input_stream_init_callback(initial_filename);
    if (pt_input_stream_ref(-1) == input_stream)
    {
        return false;
    }

    if (!mmaterial_asset_load_pmx_primitive_data_from_input_stream(input_stream, asset_input_stream_read_callback, asset_input_stream_seek_callback, out_material_texture_paths, out_material_texture_path_size))
    {
        asset_input_stream_destroy_callback(input_stream);
        return false;
    }

    asset_input_stream_destroy_callback(input_stream);
    return true;
}