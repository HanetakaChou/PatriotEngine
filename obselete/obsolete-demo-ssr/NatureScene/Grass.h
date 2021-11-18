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

#include <vector>

struct Scene_Geometry_Grass
{
private:
	struct GrassData
	{
		PTVector4F m_Billborad_Origin;
		PTVector2F m_Wave_Translation_InitialPhase;
		PTVector4F m_Texture_Index;
	};

	//�����Ĳ���������Ҷ�ڵ����ȶ���ͬ
	//��˲�����Ҫ�洢�м�ڵ����״�ṹ�����Ը��ݳ����ε�λ�ã��������Ӧ�Ľڵ������е�λ��
	struct Patch
	{
		std::vector<GrassData> m_Grass;
	} m_Patch[16][16];//Tile:16��16*(Patch:16��16) //��Terrainһ�� 

	void CullAndAnimate_Helper(
		int XMax, int XMin, int ZMax, int ZMin,//Iterate
		PTFrustumSIMD &Frustum,//Cull
		float fTime,//Animate
		std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index
	);
public:
	void Init(const char*szFileName);
#if 1
	void CullAndAnimate(PTMatrixSIMD ViewProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index);
#else
	void CullAndAnimate(PTMatrixSIMD InvView, PTMatrixSIMD InvProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index);
#endif
};

extern Scene_Geometry_Grass g_Scene_Geometry_Grass;

struct Render_Geometry_Grass_GBufferPass
{
private:
	GLuint m_hBufferVertex_Grass;
	GLuint m_hBufferIndex_Grass;
	GLuint m_hInputLayout_Grass;
	GLuint m_hProgram_Geometry_Grass_GBufferPass;
	GLuint m_hBufferReadOnly_Billborad_Origin;
	GLuint m_hTextureBuffer_Billborad_Origin;
	GLuint m_hBufferReadOnly_Wave_Translation;
	GLuint m_hTextureBuffer_Wave_Translation;
	GLuint m_hBufferReadOnly_Texture_Index;
	GLuint m_hTextureBuffer_Texture_Index;
	GLuint m_hTextureAlbedo_Grass;
public:
	void Init();
	void Execute(GLuint hBufferConstant_Sensor, PTVector4F Billborad_Origin[], PTVector4F Wave_Translation[], PTVector4F Texture_Index[], GLsizei instancecount);

};

extern Render_Geometry_Grass_GBufferPass g_Render_Geometry_Grass_GBufferPass;