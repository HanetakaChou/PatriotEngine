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

#ifndef _PT_GFX_CONNECTION_MTL_H_
#define _PT_GFX_CONNECTION_MTL_H_ 1

#include <pt_gfx_connection.h>
#include "pt_gfx_connection_common.h"

class gfx_connection_mtl : public gfx_connection_common
{
public:
    static class gfx_connection_mtl *init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);
};

#endif