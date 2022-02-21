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

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_gfx_mesh_base_load.h"
#include "pt_gfx_mesh_base_load_pmx.h"

#include <vector>
#include <string>
#include <set>
#include <map>

template <typename T>
using mcrt_vector = std::vector<T, mcrt::scalable_allocator<T>>;

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

//--------------------------------------------------------------------------------------
// This is an English description of the .PMX file format used in Miku Miku Dance (MMD)
//
// https://gist.github.com/felixjones/f8a06bd48f9da9a4539f
//--------------------------------------------------------------------------------------

// [Model::load](https://github.com/sugiany/blender_mmd_tools/blob/master/mmd_tools/core/pmx/__init__.py)
// [ReadPMXFile](https://github.com/benikabocha/saba/blob/master/src/Saba/Model/MMD/PMXFile.cpp)

enum
{
    PMX_HEADER_SIGNATURE = 0X20584d50, //'P''M''X'' '
};


struct mesh_internal_pmx_header_t
{
    bool is_text_utf8_encoding;
    uint32_t additional_vec4_count;
    uint32_t bone_index_size;
    uint32_t vertex_index_size;
    uint32_t texture_index_size;
    int64_t vertex_section_offset;
    int64_t surface_section_offset;
    int64_t material_section_offset;
    uint32_t material_count;
};

static bool inline internal_load_pmx_header_from_input_stream(struct mesh_internal_pmx_header_t *out_internal_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

extern bool mesh_load_pmx_header_from_input_stream(struct pt_gfx_mesh_neutral_header_t *out_neutral_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    struct mesh_internal_pmx_header_t internal_header;
    if (internal_load_pmx_header_from_input_stream(&internal_header, gfx_input_stream, gfx_input_stream_callbacks))
    {
        out_neutral_header->has_skin = true;
        out_neutral_header->primitive_count = internal_header.material_count;
        return true;
    }
    else
    {
        return false;
    }
}

static bool inline internal_load_pmx_header_from_input_stream(struct mesh_internal_pmx_header_t *out_internal_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

      uint32_t header_signature;
      {
          intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &header_signature, sizeof(uint32_t));
          if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
          {
              return false;
          }
      }

      if (PMX_HEADER_SIGNATURE != header_signature)
      {
          return false;
      }

      float header_version;
      {
          intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &header_version, sizeof(float));
          if (res_read == -1 || sizeof(float) != static_cast<size_t>(res_read))
          {
              return false;
          }
      }

      assert(2.0f == header_version || 2.1f == header_version);

      int8_t header_globals_count;
      {
          intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &header_globals_count, sizeof(int8_t));
          if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
          {
              return false;
          }
      }
      assert(8 == header_globals_count);

      int8_t header_globals[8];
      {
          intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, header_globals, sizeof(int8_t) * 8U);
          if (res_read == -1 || (sizeof(int8_t) * 8U) != static_cast<size_t>(res_read))
          {
              return false;
          }
      }

      if (!(0 == header_globals[0] || 1 == header_globals[0]))
      {
          return false;
      }
      bool is_text_utf8_encoding = header_globals[0];

      if (is_text_utf8_encoding)
      {
          // model name local
          {
              int32_t utf8_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf8_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf8_text_byte_count)
              {
                  return false;
              }

              if (0 != utf8_text_byte_count)
              {
                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf8_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // model name universal
          {
              int32_t utf8_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf8_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf8_text_byte_count)
              {
                  return false;
              }

              if (0 != utf8_text_byte_count)
              {
                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf8_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // comments local
          {
              int32_t utf8_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf8_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf8_text_byte_count)
              {
                  return false;
              }

              if (0 != utf8_text_byte_count)
              {
                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf8_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // comments universal
          {
              int32_t utf8_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf8_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf8_text_byte_count)
              {
                  return false;
              }

              if (0 != utf8_text_byte_count)
              {
                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf8_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }
      }
      else
      {
          // model name local
          {
              int32_t utf16_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf16_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf16_text_byte_count)
              {
                  return false;
              }

              if (0 != utf16_text_byte_count)
              {
                  if (0 != (utf16_text_byte_count % sizeof(uint16_t)))
                  {
                      return false;
                  }

                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf16_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // model name universal
          {
              int32_t utf16_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf16_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf16_text_byte_count)
              {
                  return false;
              }

              if (0 != utf16_text_byte_count)
              {
                  if (0 != (utf16_text_byte_count % sizeof(uint16_t)))
                  {
                      return false;
                  }

                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf16_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // comments local
          {
              int32_t utf16_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf16_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf16_text_byte_count)
              {
                  return false;
              }

              if (0 != utf16_text_byte_count)
              {
                  if (0 != (utf16_text_byte_count % sizeof(uint16_t)))
                  {
                      return false;
                  }

                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf16_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }

          // comments universal
          {
              int32_t utf16_text_byte_count;
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf16_text_byte_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf16_text_byte_count)
              {
                  return false;
              }

              if (0 != utf16_text_byte_count)
              {
                  if (0 != (utf16_text_byte_count % sizeof(uint16_t)))
                  {
                      return false;
                  }

                  if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf16_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                  {
                      return false;
                  }
              }
          }
      }

      if (!(0 == header_globals[1] || 1 == header_globals[1] || 2 == header_globals[1] || 3U == header_globals[1] || 4 == header_globals[1]))
      {
          return false;
      }
      uint32_t additional_vec4_count = header_globals[1];

      if (!(1 == header_globals[5] || 2 == header_globals[5] || 4 == header_globals[5]))
      {
          return false;
      }
      static_assert(1 == sizeof(int8_t), "");
      static_assert(2 == sizeof(int16_t), "");
      static_assert(4 == sizeof(int32_t), "");
      uint32_t bone_index_size = header_globals[5];

      if (!(1 == header_globals[2] || 2 == header_globals[2] || 4 == header_globals[2]))
      {
          return false;
      }
      static_assert(1 == sizeof(uint8_t), "");
      static_assert(2 == sizeof(uint16_t), "");
      static_assert(4 == sizeof(int32_t), "");
      uint32_t vertex_index_size = header_globals[2];

      if (!(1 == header_globals[3] || 2 == header_globals[3] || 4 == header_globals[3]))
      {
          return false;
      }
      static_assert(1 == sizeof(int8_t), "");
      static_assert(2 == sizeof(int16_t), "");
      static_assert(4 == sizeof(int32_t), "");
      uint32_t texture_index_size = header_globals[3];

      // vertex section
      int64_t vertex_section_offset = gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_CUR);
      if (-1 == vertex_section_offset)
      {
          return false;
      }

      // [int] vertex count	| vertices
      {
          int32_t vertex_count;
          {
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &vertex_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > vertex_count)
              {
                  return false;
              }
          }

          for (int32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
          {
              // position
              // normal
              // uv
              // additional vec4
              if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 2U + sizeof(float) * 4U * additional_vec4_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
              {
                  return false;
              }

              int8_t weight_deform_type;
              {
                  intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &weight_deform_type, sizeof(int8_t));
                  if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
                  {
                      return false;
                  }
              }

              // weight deform
              size_t weight_deform_size;
              switch (weight_deform_type)
              {
              case 0:
              {
                  // BDEF 1
                  weight_deform_size = bone_index_size;
              }
              break;
              case 1:
              {
                  // BDEF 2
                  weight_deform_size = (bone_index_size * 2U + sizeof(float));
              }
              break;
              case 2:
              {
                  // BDEF4
                  weight_deform_size = (bone_index_size * 4U + sizeof(float) * 4U);
              }
              break;
              case 3:
              {
                  // SDEF
                  weight_deform_size = (bone_index_size * 2U + sizeof(float) + sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 3U);
              }
              break;
              case 4:
              {
                  // QDEF
                  weight_deform_size = (bone_index_size * 4U + sizeof(float) * 4U);
              }
              break;
              default:
                  return false;
              }


              if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, weight_deform_size, PT_GFX_INPUT_STREAM_SEEK_CUR))
              {
                  return false;
              }

              // Edge scale
              if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, sizeof(float), PT_GFX_INPUT_STREAM_SEEK_CUR))
              {
                  return false;
              }       
          }
      }
      
      // surface section
      int64_t surface_section_offset = gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_CUR);
      if (-1 == surface_section_offset)
      {
          return false;
      }

      // [int] surface count | surfaces
      {
          int32_t surface_count;
          {
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &surface_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > surface_count)
              {
                  return false;
              }
          }

          if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, vertex_index_size * surface_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
          {
              return false;
          }
      }

      // texture section
      // [int] texture count | textures
      {
          int32_t texture_count;
          {
              intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &texture_count, sizeof(int32_t));
              if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > texture_count)
              {
                  return false;
              }
          }

          for (int32_t texture_index = 0; texture_index < texture_count; ++texture_index)
          {
              if (is_text_utf8_encoding)
              {
                  // path
                  {
                      int32_t utf8_text_byte_count;
                      intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf8_text_byte_count, sizeof(int32_t));
                      if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf8_text_byte_count)
                      {
                          return false;
                      }

                      if (0 != utf8_text_byte_count)
                      {
                          if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf8_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                          {
                              return false;
                          }
                      }
                  }
              }
              else
              {
                  // path
                  {
                      int32_t utf16_text_byte_count;
                      intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &utf16_text_byte_count, sizeof(int32_t));
                      if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > utf16_text_byte_count)
                      {
                          return false;
                      }

                      if (0 != utf16_text_byte_count)
                      {
                          if (0 != (utf16_text_byte_count % sizeof(uint16_t)))
                          {
                              return false;
                          }

                          if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, utf16_text_byte_count, PT_GFX_INPUT_STREAM_SEEK_CUR))
                          {
                              return false;
                          }
                      }
                  }
              }
          }
      }

    // material section
    int64_t material_section_offset = gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_CUR);
    if (-1 == material_section_offset)
    {
        return false;
    }

    // [int] material count	| materials
    int32_t material_count;
    {
        intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &material_count, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_count)
        {
            return false;
        }
    }

    out_internal_header->is_text_utf8_encoding = is_text_utf8_encoding;
    out_internal_header->additional_vec4_count = additional_vec4_count;
    out_internal_header->bone_index_size = bone_index_size;
    out_internal_header->vertex_index_size = vertex_index_size;
    out_internal_header->texture_index_size = texture_index_size;
    out_internal_header->vertex_section_offset = vertex_section_offset;
    out_internal_header->surface_section_offset = surface_section_offset;
    out_internal_header->material_section_offset = material_section_offset;
    out_internal_header->material_count = material_count;

    return true;
}