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
#include <pt_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_intrin.h>

#if defined(PT_X64) || defined(PT_X86)
#if defined(PT_MSVC)
#define __SSSE3__ 1
#define __extension__(x) x
#endif
#include "../third_party/DPDK/lib/librte_eal/x86/include/rte_memcpy.h"
#elif defined(PT_ARM64)
#include <string.h>
#define rte_likely(x) PT_LIKELY((x))
#include "../third_party/DPDK/lib/librte_eal/arm/include/rte_memcpy_64.h"
#elif defined(PT_ARM)
#include <string.h>
#include "../third_party/DPDK/lib/librte_eal/arm/include/rte_memcpy_32.h"
#else
#error Unknown Architecture
#endif

PT_ATTR_MCRT void *PT_CALL mcrt_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return rte_memcpy(dest, src, count);
}
