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
#include <pt_mcrt_intrin.h>

#if defined(PT_X64) || defined(PT_X86)

extern "C" mcrt_uuid mcrt_uuid_load_x86_sse2(uint8_t bytes[16])
{
    return _mm_loadu_si128(reinterpret_cast<__m128i *>(&bytes[0]));
}

extern "C" bool mcrt_uuid_equal_x86_sse2(mcrt_uuid a, mcrt_uuid b)
{
    //DirectX::XMVectorEqualIntR
    __m128i v = _mm_cmpeq_epi8(a, b);
    int mask = _mm_movemask_epi8(v);
    return (0XFFFF == mask);
}

#endif
