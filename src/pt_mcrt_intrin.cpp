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
#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_intrin.h>

#if defined(PT_X64) || defined(PT_X86)

extern "C" mcrt_uuid mcrt_uuid_load(uint8_t bytes[16])
{
    return _mm_loadu_si128(reinterpret_cast<__m128i*>(&bytes[0]));
}

extern "C" bool mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b)
{
    //DirectX::XMVectorEqualIntR
    __m128i v = _mm_cmpeq_epi8(a, b);
    int mask = _mm_movemask_epi8(v);
    return (0XFFFF == mask);
}


#elif defined(PT_ARM64) || defined(PT_ARM)

extern "C" mcrt_uuid mcrt_uuid_load(uint8_t bytes[16])
{
    return vld1q_u32(reinterpret_cast<uint32_t*>(&bytes[0]));
}

extern "C" bool mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b)
{
    //DirectX::XMVectorEqualIntR
    uint32x4_t v_result = vceqq_u32(a, b);
    uint8x8x2_t v_temp_1 = vzip_u8(vget_low_u8(v_result), vget_high_u8(v_result));
    uint16x4x2_t v_temp_2 = vzip_u16(v_temp_1.val[0], v_temp_1.val[1]);
    uint32_t mask = vget_lane_u32(v_temp_2.val[1], 1);
    return (0xFFFFFFFFU == mask);
}

#else
#error Unknown Architecture
#endif