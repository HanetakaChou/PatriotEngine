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

#ifndef _PT_GFX_NODE_VK_H_
#define _PT_GFX_NODE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_node_base.h"

class gfx_node_vk final : public gfx_node_base
{
    void destroy(class gfx_connection_base *gfx_connection) override;

public:
    inline gfx_node_vk() : gfx_node_base(){};
};

#endif