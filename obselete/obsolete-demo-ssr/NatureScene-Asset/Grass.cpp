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

#include "Grass.h"

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

#define DDS_MAGIC MAKEFOURCC('D','D','S',' ')

struct DDS_PIXELFORMAT
{
	uint32_t    size;
	uint32_t    flags;
	uint32_t    fourCC;
	uint32_t    RGBBitCount;
	uint32_t    RBitMask;
	uint32_t    GBitMask;
	uint32_t    BBitMask;
	uint32_t    ABitMask;
};

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC

struct DDS_HEADER
{
	uint32_t        size;
	uint32_t        flags;
	uint32_t        height;
	uint32_t        width;
	uint32_t        pitchOrLinearSize;
	uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	uint32_t        mipMapCount;
	uint32_t        reserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t        caps;
	uint32_t        caps2;
	uint32_t        caps3;
	uint32_t        caps4;
	uint32_t        reserved2;
};

#define D3DFMT_R32F 114

void Scene_Geometry_Grass::Init(float(*pTextureData)[256])
{
	::srand(::time(NULL));

	for (int i_x = 0; i_x < 16; ++i_x)
	{
		for (int i_y = 0; i_y < 16; ++i_y)
		{
			int Num_Grass = ::rand() % 64;

			for (int i_Grass = 0; i_Grass < Num_Grass; ++i_Grass)
			{
				GrassData l_GrassData;

				int l_X_16 = ::rand() % 16;
				int l_Y_16 = ::rand() % 16;

				l_GrassData.m_Billborad_Origin.x = 16 * i_x - 128 + l_X_16;
				l_GrassData.m_Billborad_Origin.z = 16 * i_y - 128 + l_Y_16;
				int l_V = 255 - (16 * i_y + l_Y_16);
				int l_U = (16 * i_x + l_X_16);
				assert((l_V >= 0) && (l_V < 256));
				assert((l_U >= 0) && (l_U < 256));
				l_GrassData.m_Billborad_Origin.y = pTextureData[l_V][l_U] * 30.0f + 0.25f;
				l_GrassData.m_Billborad_Origin.w = 1;

				l_GrassData.m_Wave_Translation_InitialPhase.x = ::fmodf(static_cast<float>(::rand()), 6.28f);
				l_GrassData.m_Wave_Translation_InitialPhase.y = ::fmodf(static_cast<float>(::rand()), 6.28f);
				
				l_GrassData.m_Texture_Index.x = ::rand() % 16;
				l_GrassData.m_Texture_Index.y = ::rand() % 16;
				l_GrassData.m_Texture_Index.z = ::rand() % 16;
				l_GrassData.m_Texture_Index.w = ::rand() % 16;
				
				m_Patch[i_x][i_y].m_Grass.push_back(l_GrassData);
			}
		}
	}
}



void Scene_Geometry_Grass::CullAndAnimate_Helper(
	int XMax, int XMin, int ZMax, int ZMin,//Iterate
	PTFrustumSIMD &Frustum,//Cull
	float fTime,//Animate
	std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index
)
{
	PTVector3F AAB_Min{ XMin ,0.0f,ZMin };
	PTVector3F AAB_Max{ XMax ,2000.0f,ZMax};//Height����ȷ��
	PTAABSIMD AAB = ::PTAABSIMDLoad(&AAB_Min, &AAB_Max);

	PTIntersectionType IntersectionType =::PTFrustumSIMDAABSIMDIntersect(Frustum, AAB);

	if (IntersectionType == PTDISJOINT)
	{
		//Early Out
		//All Child Node Disjoint
		//return;
	}
	else
	{
		assert((XMax - XMin) == (ZMax - ZMin));
		assert(((XMax - XMin)&(XMax - XMin - 1)) == 0);
		assert((XMax - XMin) >= 16);

		if ((XMax - XMin) == 16)//Leaf Node
		{
			//Ҷ�ڵ�
			//�ݹ����

			int i_x = (XMin + 128) / 16;
			int i_y = (ZMin + 128) / 16;

			for (GrassData &rGrassData : m_Patch[i_x][i_y].m_Grass)
			{
				Billborad_Origin.push_back(rGrassData.m_Billborad_Origin);
				//Animate
				Wave_Translation.push_back(
					PTVector4F{
					0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.x),//��� Amplitude = 0.2f
					0.0f,
					0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.y),//��� Amplitude = 0.2f
					1.0f });
				Texture_Index.push_back(rGrassData.m_Texture_Index);
			}

			//return;
		}
		else//Internal Node
		{
			if (IntersectionType == PTCONTAIN)
			{
				//Early Out
				//All Child Node Contain

				int i_x_min = (XMin + 128) / 16;
				int i_x_max = (XMax + 128) / 16 - 1;
				int i_y_min = (ZMin + 128) / 16;
				int i_y_max = (ZMax + 128) / 16 - 1;

				assert(i_x_min >= 0);
				assert(i_x_max <= 15);
				assert(i_y_min >= 0);
				assert(i_y_max <= 15);

				for (int i_x = i_x_min; i_x <= i_x_max; ++i_x)
				{
					for (int i_y = i_y_min; i_y <= i_y_max; ++i_y)
					{
						for (GrassData &rGrassData : m_Patch[i_x][i_y].m_Grass)
						{
							Billborad_Origin.push_back(rGrassData.m_Billborad_Origin);
							//Animate
							Wave_Translation.push_back(
								PTVector4F{
								0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.x),//��� Amplitude = 0.2f
								0.0f,
								0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.y),//��� Amplitude = 0.2f
								1.0f });
							Texture_Index.push_back(rGrassData.m_Texture_Index);
						}
					}
				}

				//return;

			}
			else if (IntersectionType == PTINTERSECT)
			{

				//�ݹ�����
				//Four Child Node

				this->CullAndAnimate_Helper(
					XMax, (XMax + XMin) / 2, ZMax, (ZMax + ZMin) / 2,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					XMax, (XMax + XMin) / 2, (ZMax + ZMin) / 2, ZMin,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					(XMax + XMin) / 2, XMin, ZMax, (ZMax + ZMin) / 2,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					(XMax + XMin) / 2, XMin, (ZMax + ZMin) / 2, ZMin,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);
			}
		}
	}	
}

#if 1
void Scene_Geometry_Grass::CullAndAnimate(PTMatrixSIMD ViewProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index)
{
	PTFrustumSIMD Frustum = ::PTFrustumSIMDLoadRH(ViewProjection);
	this->CullAndAnimate_Helper(
		128, -128, 128, -128,
		Frustum,
		fTime,
		Billborad_Origin, Wave_Translation, Texture_Index
	);
}
#else
void Scene_Geometry_Grass::CullAndAnimate(PTMatrixSIMD InvView, PTMatrixSIMD InvProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index)
{
	alignas(16) float fDevice[8][4] = {
		{ -1.0f ,1.0f ,1.0f ,1.0f },
		{ 1.0f ,1.0f ,1.0f ,1.0f },
		{ -1.0f ,-1.0f ,1.0f ,1.0f },
		{ 1.0f ,-1.0f ,1.0f ,1.0f },
		{ -1.0f ,1.0f ,0.0f ,1.0f },
		{ 1.0f ,1.0f ,0.0f ,1.0f },
		{ -1.0f ,-1.0f ,0.0f ,1.0f },
		{ 1.0f ,-1.0f ,0.0f ,1.0f }
	};

	__m128 vMin;
	__m128 vMax;

	__m128 vDevice = ::_mm_load_ps(fDevice[0]);
	__m128 vView = ::PTVector4SIMDTransform(InvProjection, PTVectorSIMD{ vDevice }).v;
	__m128 vView_w = ::_mm_shuffle_ps(vView, vView, _MM_SHUFFLE(3, 3, 3, 3));
	vView = ::_mm_div_ps(vView, vView_w);
	__m128 vWorld = ::PTVector4SIMDTransform(InvView, PTVectorSIMD{ vView }).v;
	vMin = vWorld;
	vMax = vWorld;

	for (int i = 1; i < 8; ++i)
	{
		__m128 vDevice = ::_mm_load_ps(fDevice[i]);
		__m128 vView = ::PTVector4SIMDTransform(InvProjection, PTVectorSIMD{ vDevice }).v;
		__m128 vView_w = ::_mm_shuffle_ps(vView, vView, _MM_SHUFFLE(3, 3, 3, 3));
		vView = ::_mm_div_ps(vView, vView_w);
		__m128 vWorld = ::PTVector4SIMDTransform(InvView, PTVectorSIMD{ vView }).v;
		vMin = ::_mm_min_ps(vMin, vWorld);
		vMax = ::_mm_max_ps(vMax, vWorld);
	}

	alignas(16) float fMin[4];
	alignas(16) float fMax[4];
	::_mm_store_ps(fMin, vMin);
	::_mm_store_ps(fMax, vMax);

	int i_x_min = static_cast<int>((fMin[0] + 128.0f) / 16.0f);
	int i_x_max = static_cast<int>((fMax[0] + 128.0f) / 16.0f);
	int i_y_min = static_cast<int>((fMin[2] + 128.0f) / 16.0f);
	int i_y_max = static_cast<int>((fMax[2] + 128.0f) / 16.0f);


	Billborad_Origin.clear();
	Wave_Translation.clear();
	Texture_Index.clear();

	for (int i_x = max(0, i_x_min - 1); i_x < min(16, i_x_max + 1); ++i_x)
	{
		for (int i_y = max(0, i_y_min - 1); i_y < min(16, i_y_max + 1); ++i_y)
		{
			for (GrassData &rGrassData : m_Patch[i_x][i_y].m_Grass)
			{
				Billborad_Origin.push_back(rGrassData.m_Billborad_Origin);
				Wave_Translation.push_back(PTVector4F{ ::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.x),0.0f,::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.y),1.0f });
				Texture_Index.push_back(rGrassData.m_Texture_Index);
			}
		}
	}
}
#endif

static char const *const Geometry_Grass_GBufferPass_Vertex =
R"(#version 310 es
#extension GL_EXT_shader_io_blocks : enable
#extension GL_OES_shader_io_blocks : enable
#extension GL_EXT_texture_buffer : enable
#extension GL_OES_texture_buffer : enable

layout(location = 0) in vec4 IA_Position;
layout(location = 1) in vec2 IA_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	mat4x4 g_View;
	mat4x4 g_Projection;
	mat4x4 g_InvView;
	mat4x4 g_InvProjection;
};

layout(binding = 5) uniform highp samplerBuffer g_BufferReadOnly_Billborad_Origin;

layout(binding = 7) uniform highp samplerBuffer g_BufferReadOnly_Wave_Translation;

layout(binding = 8) uniform highp samplerBuffer g_BufferReadOnly_Texture_Index;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 6) out vec3 GS_Grass_PositionWorld;
layout(location = 7) out vec3 GS_Grass_UVW;

void main() 
{
    vec3 l_Position_Billboard = IA_Position.xyz;
	vec3 l_Billborad_Origin = texelFetch(g_BufferReadOnly_Billborad_Origin, gl_InstanceID).xyz;
	vec3 l_Wave_Translation = (((gl_VertexID%2) == 1) ? texelFetch(g_BufferReadOnly_Wave_Translation, gl_InstanceID).xyz : vec3(0.0f,0.0f,0.0f));
    GS_Grass_PositionWorld = l_Position_Billboard + l_Billborad_Origin + l_Wave_Translation;
    gl_Position = g_Projection * g_View * vec4(GS_Grass_PositionWorld,1.0f);
	
    vec4 l_Texture_Index_Vector = texelFetch(g_BufferReadOnly_Texture_Index, gl_InstanceID).xyzw;
    float l_Texture_Index_Array[4];
    l_Texture_Index_Array[0] = l_Texture_Index_Vector.x;
    l_Texture_Index_Array[1] = l_Texture_Index_Vector.y;
    l_Texture_Index_Array[2] = l_Texture_Index_Vector.z;
    l_Texture_Index_Array[3] = l_Texture_Index_Vector.w;
	GS_Grass_UVW = vec3(IA_UV,l_Texture_Index_Array[gl_VertexID/4]);
})";

static char const *const Geometry_Grass_GBufferPass_Geometry = 
R"(#version 310 es
#extension GL_EXT_geometry_shader : enable
#extension GL_OES_geometry_shader : enable

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 6) in vec3 GS_Grass_PositionWorld[];
layout(location = 7) in vec3 GS_Grass_UVW[];

layout(location = 6) out vec3 RS_Grass_PositionWorld;
layout(location = 7) out vec3 RS_Grass_UVW;
layout(location = 8) out vec3 RS_Normal;

void main()
{
    vec3 NormalFace = normalize(cross(GS_Grass_PositionWorld[1] - GS_Grass_PositionWorld[0], GS_Grass_PositionWorld[2] - GS_Grass_PositionWorld[1]));//gl_FrontFacing

    RS_Grass_PositionWorld = GS_Grass_PositionWorld[0];
    gl_Position = gl_in[0].gl_Position;
    RS_Grass_UVW = GS_Grass_UVW[0];
    RS_Normal = NormalFace;
    EmitVertex();
    
    RS_Grass_PositionWorld = GS_Grass_PositionWorld[1];
    gl_Position = gl_in[1].gl_Position;
    RS_Grass_UVW = GS_Grass_UVW[1];
    RS_Normal = NormalFace;
    EmitVertex();
    
    RS_Grass_PositionWorld = GS_Grass_PositionWorld[2];
    gl_Position = gl_in[2].gl_Position;
    RS_Grass_UVW = GS_Grass_UVW[2];
    RS_Normal = NormalFace;
    EmitVertex();
    
    EndPrimitive();
})";

static char const *const Geometry_Grass_GBufferPass_Fragment =
R"(#version 310 es
layout(binding = 13) uniform highp sampler2DArray g_TexAlbedoAndAlpha;

layout(location = 6) in centroid highp vec3 RS_Grass_PositionWorld;
layout(location = 7) in centroid highp vec3 RS_Grass_UVW;
layout(location = 8) in centroid highp vec3 RS_Normal;

layout(location = 0) out highp vec4 SV_Target0; //NormalMacro
layout(location = 1) out highp vec4 SV_Target1; //NormalMeso
layout(location = 2) out highp vec4 SV_Target2; //Albedo
layout(location = 3) out highp vec4 SV_Target3; //Specular

//layout (early_fragment_tests) in; �����д���ͻ

void main() 
{
	highp vec3 NormalMacro = normalize(RS_Normal);
	highp vec3 NormalMeso = NormalMacro;
	highp vec4 AlbedoAndAlpha = texture(g_TexAlbedoAndAlpha,RS_Grass_UVW).xyzw;//Alpha Coverage
	highp float Alpha = AlbedoAndAlpha.w;
	highp vec3 Albedo = AlbedoAndAlpha.xyz * 2.0f;
	highp vec3 Specular = vec3(0.0f,0.0f,0.0f);


	SV_Target0 = vec4((NormalMacro+1.0f)*0.5f,Alpha);	
	SV_Target1 = vec4((NormalMeso+1.0f)*0.5f,Alpha);	
	SV_Target2 = vec4(Albedo,Alpha);	
	SV_Target3 = vec4(Specular,Alpha);	
})";

void Render_Geometry_Grass_GBufferPass::Init()
{
	//m_hBufferVertex_Grass
	{
		::glGenBuffers(1, &m_hBufferVertex_Grass);
		::glBindBuffer(GL_ARRAY_BUFFER, m_hBufferVertex_Grass);
		float BufferVertexData[] =
		{
			0.707f,0.0f,-0.707f,  1.0f,1.0f,
			0.707f,2.0f,-0.707f,  1.0f,0.0f,
			-0.707f,0.0f,0.707f,  0.0f,1.0f,
			-0.707f,2.0f,0.707f,  0.0f,0.0f,

			0.707f,0.0f,0.707f,   1.0f,1.0f,
			0.707f,2.0f,0.707f,   1.0f,0.0f,
			-0.707f,0.0f,-0.707f, 0.0f,1.0f,
			-0.707f,2.0f,-0.707f, 0.0f,0.0f,

			0.0f,0.0f,1.0f,     1.0f,1.0f,
			0.0f,2.0f,1.0f,		1.0f,0.0f,
			0.0f,0.0f,-1.0f,    0.0f,1.0f,
			0.0f,2.0f,-1.0f,    0.0f,0.0f,

			1.0f,0.0f,0.0f,     1.0f,1.0f,
			1.0f,2.0f,0.0f,		1.0f,0.0f,
			-1.0f,0.0f,0.0f,    0.0f,1.0f,
			-1.0f,2.0f,0.0f,    0.0f,0.0f
		};
		::glBufferData(GL_ARRAY_BUFFER, sizeof(BufferVertexData), BufferVertexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ARRAY_BUFFER, 0U);
	}

	//m_hBufferIndex_Grass
	{
		::glGenBuffers(1, &m_hBufferIndex_Grass);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Grass);
		uint16_t BufferIndexData[] =
		{
			0, 1, 2, 3, 0XFFFF,
			4, 5, 6, 7, 0XFFFF,
			8, 9, 10, 11, 0XFFFF,
			12, 13, 14, 15
		};
		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BufferIndexData), BufferIndexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);

	}

	//m_hInputLayout_Grass
	{
		::glGenVertexArrays(1, &m_hInputLayout_Grass);
		::glBindVertexArray(m_hInputLayout_Grass);
		::glEnableVertexAttribArray(0);
		::glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0U);
		::glVertexAttribBinding(0U, 0U);
		::glEnableVertexAttribArray(1);
		::glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
		::glVertexAttribBinding(1U, 0U);
		::glBindVertexArray(0U);
	}

	GLuint hShader_Geometry_Grass_GBufferPass_Vertex;
	{
		hShader_Geometry_Grass_GBufferPass_Vertex = ::glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_Geometry_Grass_GBufferPass_Vertex, 1U, &::Geometry_Grass_GBufferPass_Vertex, NULL);
		::glCompileShader(hShader_Geometry_Grass_GBufferPass_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Grass_GBufferPass_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Grass_GBufferPass_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Grass_GBufferPass_Geometry;
	{
		hShader_Geometry_Grass_GBufferPass_Geometry = ::glCreateShader(GL_GEOMETRY_SHADER_EXT);
		::glShaderSource(hShader_Geometry_Grass_GBufferPass_Geometry, 1U, &Geometry_Grass_GBufferPass_Geometry, NULL);
		::glCompileShader(hShader_Geometry_Grass_GBufferPass_Geometry);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Grass_GBufferPass_Geometry, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Grass_GBufferPass_Geometry, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Grass_GBufferPass_Fragment;
	{
		hShader_Geometry_Grass_GBufferPass_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Grass_GBufferPass_Fragment, 1U, &Geometry_Grass_GBufferPass_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Grass_GBufferPass_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Grass_GBufferPass_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Grass_GBufferPass_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	//m_hProgram_Geometry_Grass_GBufferPass
	{
		m_hProgram_Geometry_Grass_GBufferPass = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Grass_GBufferPass, hShader_Geometry_Grass_GBufferPass_Vertex);
		::glAttachShader(m_hProgram_Geometry_Grass_GBufferPass, hShader_Geometry_Grass_GBufferPass_Geometry);
		::glAttachShader(m_hProgram_Geometry_Grass_GBufferPass, hShader_Geometry_Grass_GBufferPass_Fragment);
		
		::glLinkProgram(m_hProgram_Geometry_Grass_GBufferPass);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Grass_GBufferPass, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Grass_GBufferPass, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//m_hBufferReadOnly_Billborad_Origin
	//m_hTextureBuffer_Billborad_Origin
	{
		::glGenBuffers(1, &m_hBufferReadOnly_Billborad_Origin);
		::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Billborad_Origin);
		::glBufferData(GL_TEXTURE_BUFFER_OES, sizeof(PTVector4F) * 200000, NULL, GL_DYNAMIC_DRAW);

		::glGenTextures(1, &m_hTextureBuffer_Billborad_Origin);
		::glActiveTexture(GL_TEXTURE6);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Billborad_Origin);
		::glTexBufferOES(GL_TEXTURE_BUFFER_OES, GL_RGBA32F, m_hBufferReadOnly_Billborad_Origin);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, 0U);
	}

	//m_hBufferReadOnly_Wave_Translation
	//m_hTextureBuffer_Wave_Translation
	{
		::glGenBuffers(1, &m_hBufferReadOnly_Wave_Translation);
		::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Wave_Translation);
		::glBufferData(GL_TEXTURE_BUFFER_OES, sizeof(PTVector4F) * 200000, NULL, GL_DYNAMIC_DRAW);

		::glGenTextures(1, &m_hTextureBuffer_Wave_Translation);
		::glActiveTexture(GL_TEXTURE6);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Wave_Translation);
		::glTexBufferOES(GL_TEXTURE_BUFFER_OES, GL_RGBA32F, m_hBufferReadOnly_Wave_Translation);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, 0U);
	}

	//m_hBufferReadOnly_Texture_Index;
	//m_hTextureBuffer_Texture_Index;
	{
		::glGenBuffers(1, &m_hBufferReadOnly_Texture_Index);
		::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Texture_Index);
		::glBufferData(GL_TEXTURE_BUFFER_OES, sizeof(PTVector4F) * 200000, NULL, GL_DYNAMIC_DRAW);

		::glGenTextures(1, &m_hTextureBuffer_Texture_Index);
		::glActiveTexture(GL_TEXTURE6);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Texture_Index);
		::glTexBufferOES(GL_TEXTURE_BUFFER_OES, GL_RGBA32F, m_hBufferReadOnly_Texture_Index);
		::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, 0U);
	}

	m_hTextureAlbedo_Grass = ::glexCreateDDSTextureFromFile("Grass.dds");
}

Scene_Geometry_Grass g_Scene_Geometry_Grass;

void Render_Geometry_Grass_GBufferPass::Execute(GLuint hBufferConstant_Sensor, PTVector4F Billborad_Origin[], PTVector4F Wave_Translation[], PTVector4F Texture_Index[], GLsizei instancecount)
{
	//��ֵ
	assert(200000 >= instancecount);
	instancecount = min(200000, instancecount);

	//Update
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Billborad_Origin);
	::glBufferSubData(GL_TEXTURE_BUFFER_OES, 0, sizeof(PTVector4F)*instancecount, Billborad_Origin);
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, 0U);
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Wave_Translation);
	::glBufferSubData(GL_TEXTURE_BUFFER_OES, 0, sizeof(PTVector4F)*instancecount, Wave_Translation);
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, 0U);
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, m_hBufferReadOnly_Texture_Index);
	::glBufferSubData(GL_TEXTURE_BUFFER_OES, 0, sizeof(PTVector4F)*instancecount, Texture_Index);
	::glBindBuffer(GL_TEXTURE_BUFFER_OES, 0U);

	//RS
	::glDisable(GL_CULL_FACE);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Grass_GBufferPass);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	//SRV
	::glActiveTexture(GL_TEXTURE5);
	::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Billborad_Origin);
	::glActiveTexture(GL_TEXTURE7);
	::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Wave_Translation);
	::glActiveTexture(GL_TEXTURE8);
	::glBindTexture(GL_TEXTURE_BUFFER_BINDING_OES, m_hTextureBuffer_Texture_Index);
	//::glBindSampler(13, hSampler_Linear);
	::glActiveTexture(GL_TEXTURE13);
	::glBindTexture(GL_TEXTURE_2D_ARRAY, m_hTextureAlbedo_Grass);

	//IA
	::glBindVertexArray(m_hInputLayout_Grass);
	::glBindVertexBuffer(0U, m_hBufferVertex_Grass, 0U, sizeof(float) * 3 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Grass);

	::glDrawElementsInstanced(GL_TRIANGLE_STRIP, 19, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0), instancecount);
}

Render_Geometry_Grass_GBufferPass g_Render_Geometry_Grass_GBufferPass;