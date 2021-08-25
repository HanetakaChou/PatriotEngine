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
#include <pt_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_memcmp.h>
#include <string.h>

PT_ATTR_MCRT int PT_CALL mcrt_memcmp(void const *ptr1, void const *ptr2, size_t num)
{
    //TODO //SSE //NEON
    return memcmp(ptr1, ptr2, num);
}
