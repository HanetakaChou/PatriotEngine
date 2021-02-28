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

#define PTWIN32
#define PTX86
#include "D:\OSChina\PatriotEngine\Runtime\Public\Math\PTMath.h"

struct BufferConstant_WaterTileData
{
	PTMatrix4x4F m_WaterToWorld;
	PTVector4F m_UVAnimated;
};

struct Render_Geometry_Water
{
private:
	GLuint m_hBufferVertex_Water;
	GLuint m_hBufferIndex_Water;
	GLuint m_hInputLayout_Water;
	GLuint m_hProgram_Geometry_Water_GBufferPass;
	GLuint m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection;
	GLuint m_hProgram_Geometry_Water_ForwardPass_PlanarReflection;
	GLuint m_hBufferConstant_WaterTile;
	GLuint m_hSampler_RepeatLinear;
	GLuint m_hTexture_NormalMap_HighFreq;
	GLuint m_hTexture_NormalMap_LowFreq;
public:
	void Init();
	void Execute_GBufferPass(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData);
	void Execute_ForwardPass_ScreenSpaceReflection(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData, GLuint hBufferConstant_Light);
	void Execute_ForwardPass_PlanarReflection(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData, GLuint hBufferConstant_Light);

};

extern Render_Geometry_Water g_Render_Geometry_Water;