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

#include <pt_common.h>
#include <pt_mcrt_common.h>

#if defined(PT_X64) || defined(PT_X86)
#include "pt_mcrt_memcpy_dpdk_rte_memcpy_x86.h"

extern void *rte_memcpy_x86_avx512f(void *__restrict dest, void const *__restrict src, size_t count)
{
    return rte_memcpy(dest, src, count);
}

#endif
