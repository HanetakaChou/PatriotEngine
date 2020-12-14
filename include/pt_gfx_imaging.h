/*
 * Copyright (C) YuqiaoZhang
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

#ifndef _GFX_IMAGING_H_
#define _GFX_IMAGING_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif

#include "pt_gfx_common.h"
#include "pt_wsi_window.h"

struct gfx_iimaging
{
    virtual void destroy() = 0;
};

PT_GFX_ATTR gfx_iimaging *PT_CALL gfx_imaging_init(struct wsi_iwindow *window);

#ifdef __cplusplus
}
#endif

#endif
