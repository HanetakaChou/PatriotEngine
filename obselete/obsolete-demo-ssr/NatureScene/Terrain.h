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

struct BufferConstant_TerrainTileData
{
	PTMatrix4x4F m_TangentToWorld;
};

struct Render_Geometry_Terrain
{
private:
	GLuint m_hBufferVertex_Terrain;
	GLuint m_hBufferIndex_Terrain;
	GLuint m_hInputLayout_Terrain;
	GLuint m_hProgram_Geometry_Terrain_GBufferPass;
	GLuint m_hProgram_Geometry_Terrain_PlanarReflectionPass;
	GLuint m_hSampler_Point;
	GLuint m_hSampler_ClampLinear;
	GLuint m_hSampler_RepeatLinear;
	GLuint m_hBufferConstant_TerrainTile;
public:
	void Init();
	void Execute_GBufferPass(GLuint hBufferConstant_Sensor, BufferConstant_TerrainTileData *pBufferConstant_TerrainTileData, GLuint hTexture_Terrain_Displacement, GLuint hTexture_Terrain_Displacement_MaxMin, GLuint hTexture_Terrain_Albedo);
	void Execute_PlanarReflectionPass(GLuint hBufferConstant_Sensor, BufferConstant_TerrainTileData *pBufferConstant_TerrainTileData, GLuint hTexture_Terrain_Displacement, GLuint hTexture_Terrain_Displacement_MaxMin, GLuint hTexture_Terrain_Albedo, GLuint hBufferConstant_Light);
};

extern Render_Geometry_Terrain g_Render_Geometry_Terrain;