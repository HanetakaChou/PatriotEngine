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

#include "mesh_vertex.h"
#include <cmath>
#include <algorithm>
#include <assert.h>

extern int8_t mesh_vertex_float_to_8_snorm(float unpacked_input)
{
    // UE: [FPackedNormal](https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/RenderCore/Public/PackedNormal.h#L98)

    assert((-1.01F < unpacked_input) && (unpacked_input < 1.01F));

    // D3DX_DXGIFormatConvert.inl
    // D3DX_FLOAT4_to_R8G8B8A8_SNORM
    float saturate_signed_float = std::min(std::max(unpacked_input, -1.0F), 1.0F);
    float float_to_int = saturate_signed_float * static_cast<float>(INT8_MAX) + ((saturate_signed_float >= 0.0F) ? 0.5F : -0.5F);
    float truncate_float = (float_to_int >= 0.0F) ? std::floor(float_to_int) : std::ceil(float_to_int);
    int8_t packed_output = static_cast<int8_t>(truncate_float);
    return packed_output;
}

extern uint16_t mesh_vertex_float_to_16_unorm(float unpacked_input)
{
    assert((-0.01F < unpacked_input) && (unpacked_input < 1.01F));

    // D3DX_DXGIFormatConvert.inl
    // D3DX_FLOAT4_to_R8G8B8A8_UNORM
    float saturate_float = std::min(std::max(unpacked_input, 0.0F), 1.0F);
    float float_to_uint = saturate_float * static_cast<float>(UINT16_MAX) + 0.5F;
    float truncate_float = std::floor(float_to_uint);
    uint16_t packed_output = static_cast<uint16_t>(truncate_float);
    return packed_output;
}

