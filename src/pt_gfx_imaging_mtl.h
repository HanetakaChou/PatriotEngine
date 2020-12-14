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

#ifndef _GFX_IMAGING_MTL_H_
#define _GFX_IMAGING_MTL_H_ 1

#include "pt_gfx_imaging.h"

class gfx_iimaging_mtl : public gfx_iimaging
{
    void destroy() override;
};

class gfx_iimaging_mtl *gfx_imaging_mtl_init(struct wsi_iwindow *window);

#endif