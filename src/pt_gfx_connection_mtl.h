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

#ifndef _PT_GFX_CONNECTION_MTL_H_
#define _PT_GFX_CONNECTION_MTL_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_connection_base.h"
#include "pt_gfx_api_mtl.h"

class gfx_connection_mtl : public gfx_connection_base
{
    class gfx_api_mtl m_api_mtl;

    class gfx_texture_base *create_texture() override;
    void on_wsi_resized(float width, float height) override;
    void draw_acquire() override;
    void draw_release() override;

    inline gfx_connection_mtl();
    inline ~gfx_connection_mtl();
    bool init(pt_gfx_wsi_window_ref wsi_window);
    void destroy() override;

    friend class gfx_connection_base *gfx_connection_mtl_init(pt_gfx_wsi_window_ref wsi_window);

public:
};

class gfx_connection_base *gfx_connection_mtl_init(pt_gfx_wsi_window_ref wsi_window);

#endif