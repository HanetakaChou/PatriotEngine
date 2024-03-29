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

#ifndef _PT_MCRT_LOG_H_
#define _PT_MCRT_LOG_H_ 1

#include <stddef.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_MCRT void PT_CALL mcrt_log_write(char const *msg);
    PT_ATTR_MCRT void PT_CALL mcrt_log_print(char const *fmt, ...);
#ifdef __cplusplus
}
#endif

#endif