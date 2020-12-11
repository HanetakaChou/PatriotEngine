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

#ifndef _WSI_COMMON_H_
#define _WSI_COMMON_H_ 1

#include "pt_common.h"

#ifndef PT_WSI_ATTR
#define PT_WSI_ATTR PT_IMPORT
#endif

struct wsi_iwindow
{
    //ANativeActivityCallbacks::onNativeWindowResized
    //MTKViewDelegate::drawableSizeWillChange
    virtual void listen_size_change(void (*size_change_callback)(float width, float height, void *user_data), void *user_data) = 0;

    //ANativeActivityCallbacks::onNativeWindowRedrawNeeded
    //MTKViewDelegate::drawInMTKView 
    virtual void listen_draw_request(void (*draw_request_callback)(uintptr_t connection, uintptr_t window, void *user_data), void *user_data) = 0; //We may use the given window to recreate the swapchain.
};

#endif
