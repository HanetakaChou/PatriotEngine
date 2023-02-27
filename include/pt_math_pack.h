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

#ifndef _PT_MATH_PACK_H_
#define _PT_MATH_PACK_H_ 1

#include "pt_math_common.h"
#include <stddef.h>
#include <stdint.h>
#include <DirectXPackedVector.h>

inline uint32_t pt_math_float4_to_r10g10b10a2_unorm(float unpacked_input[4])
{
    // UE: [FPackedNormal](https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/RenderCore/Public/PackedNormal.h#L98)

    // D3DX_DXGIFormatConvert.inl
    // D3DX_FLOAT4_to_R10G10B10A2_UNORM

    assert((-0.01F < unpacked_input[0]) && (unpacked_input[0] < 1.01F));
    assert((-0.01F < unpacked_input[1]) && (unpacked_input[1] < 1.01F));
    assert((-0.01F < unpacked_input[2]) && (unpacked_input[2] < 1.01F));
    assert((-0.01F < unpacked_input[3]) && (unpacked_input[3] < 1.01F));

    DirectX::PackedVector::XMUDECN4 vector_packed_output;
    {
        DirectX::XMFLOAT4A vector_unpacked_input(unpacked_input[0], unpacked_input[1], unpacked_input[2], unpacked_input[3]);
        DirectX::PackedVector::XMStoreUDecN4(&vector_packed_output, DirectX::XMLoadFloat4A(&vector_unpacked_input));
    }

    return (vector_packed_output.v);
}

inline uint16_t pt_math_float_to_r16_float(float unpacked_input)
{
    return DirectX::PackedVector::XMConvertFloatToHalf(unpacked_input);
}

#endif