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

#if defined(_PT_MCRT_INTRIN_H_) && !defined(_PT_MCRT_INTRIN_INL_)
#define _PT_MCRT_INTRIN_INL_ 1

#include <assert.h>

template <typename T>
inline T mcrt_intrin_round_down(T value, T alignment)
{
    // power-of-2 alignment
    assert((alignment != static_cast<T>(0)) && ((alignment & (alignment - static_cast<T>(1))) == static_cast<T>(0)));

    //
    //  Copyright (c) 2005-2019 Intel Corporation
    //
    //  Licensed under the Apache License, Version 2.0 (the "License");
    //  you may not use this file except in compliance with the License.
    //  You may obtain a copy of the License at
    //
    //      http://www.apache.org/licenses/LICENSE-2.0
    //
    //  Unless required by applicable law or agreed to in writing, software
    //  distributed under the License is distributed on an "AS IS" BASIS,
    //  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    //  See the License for the specific language governing permissions and
    //  limitations under the License.
    //

    /** @file src/tbbmalloc/shared_utils.h */

    return ((value) & (~(alignment - static_cast<T>(1))));
}

template <typename T>
inline T mcrt_intrin_round_up(T value, T alignment)
{

    // power-of-2 alignment
    assert((alignment != static_cast<T>(0)) && ((alignment & (alignment - static_cast<T>(1))) == static_cast<T>(0)));

    //
    //  Copyright (c) 2005-2019 Intel Corporation
    //
    //  Licensed under the Apache License, Version 2.0 (the "License");
    //  you may not use this file except in compliance with the License.
    //  You may obtain a copy of the License at
    //
    //      http://www.apache.org/licenses/LICENSE-2.0
    //
    //  Unless required by applicable law or agreed to in writing, software
    //  distributed under the License is distributed on an "AS IS" BASIS,
    //  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    //  See the License for the specific language governing permissions and
    //  limitations under the License.
    //

    /** @file src/tbbmalloc/shared_utils.h */

    return (((value - static_cast<T>(1)) | (alignment - static_cast<T>(1))) + static_cast<T>(1));
}

#if defined(PT_X64) || defined(PT_X86)
#include <emmintrin.h>
inline mcrt_uuid mcrt_uuid_load(uint8_t bytes[16])
{
    return _mm_loadu_si128(reinterpret_cast<__m128i *>(&bytes[0]));
}

inline bool mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b)
{
    //DirectX::XMVectorEqualIntR
    __m128i v = _mm_cmpeq_epi8(a, b);
    int mask = _mm_movemask_epi8(v);
    return (0XFFFF == mask);
}
#elif defined(PT_ARM64) || defined(PT_ARM)
#else
#error Unknown Architecture
#endif

#else
#error "Never use <pt_mcrt_intrin.inl> directly; include <pt_mcrt_intrin.h> instead."
#endif