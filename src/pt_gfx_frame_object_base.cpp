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

#include <assert.h>
#include "pt_gfx_frame_object_base.h"

void gfx_frame_object_base::frame_destroy_request(class gfx_connection_base *gfx_connection)
{
    return gfx_connection->frame_object_destroy_list_push(this);
}

void gfx_frame_object_base::frame_destroy_execute(class gfx_connection_base *gfx_connection)
{
    return this->frame_destroy_callback(gfx_connection);
}