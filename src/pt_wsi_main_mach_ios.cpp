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

#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>

extern "C" bool wsi_mach_ios_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg)
{
    return mcrt_native_thread_create(tid, func, arg);
}

extern "C" void wsi_mach_ios_os_yield(void)
{
    return mcrt_os_yield();
}

extern "C" bool wsi_mach_ios_atomic_load(bool volatile *src)
{
    return mcrt_atomic_load(src);
}

extern "C" void wsi_mach_ios_atomic_store(bool volatile *dst, bool val)
{
    return mcrt_atomic_store(dst, val);
}
