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
extern "C" mcrt_uuid mcrt_uuid_load_x86_sse2(uint8_t bytes[16]);
extern "C" bool mcrt_uuid_equal_x86_sse2(mcrt_uuid a, mcrt_uuid b);

PT_ATTR_MCRT mcrt_uuid PT_CALL mcrt_uuid_load(uint8_t bytes[16])
{
    return mcrt_uuid_load_x86_sse2(bytes);
}

PT_ATTR_MCRT bool PT_CALL mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b)
{
    return mcrt_uuid_equal_x86_sse2(a, b);
}

#elif defined(PT_ARM64) || defined(PT_ARM)
extern "C" mcrt_uuid mcrt_uuid_load_arm_neon(uint8_t bytes[16]);
extern "C" bool mcrt_uuid_equal_arm_neon(mcrt_uuid a, mcrt_uuid b);

PT_ATTR_MCRT mcrt_uuid PT_CALL mcrt_uuid_load(uint8_t bytes[16])
{
    return mcrt_uuid_load_arm_neon(bytes);
}

PT_ATTR_MCRT bool PT_CALL mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b)
{
    return mcrt_uuid_equal_arm_neon(a, b);
}

#else
#error Unknown Architecture
#endif