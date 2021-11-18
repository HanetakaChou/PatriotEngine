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

#pragma once

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../libGL/include/EGL/egl.h"
#include "../libGL/include/EGL/eglext.h"
#include "../libGL/include/GLES3/gl31.h"
#define GL_GLEXT_PROTOTYPES
#include "../libGL/include/GLES3/gl2ext.h"

#include "../libGL/include/glex.h"

struct Render_SkyBox_ForwardPass
{
private:
	GLuint m_hProgram_ForwardPass_SkyBox;
	GLuint m_hTextureRadiance_SkyBox;
public:
	void Init();
	void Execute(GLuint hBufferConstant_Sensor);
};

extern Render_SkyBox_ForwardPass g_Render_SkyBox_ForwardPass;