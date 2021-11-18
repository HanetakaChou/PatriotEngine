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

#include "Terrain.h"
#include <stdint.h>
#include <assert.h>

static char const *const Geometry_Terrain_Vertex =
R"(#version 310 es

layout(location = 0) in vec2 IA_Patch_PositionTangentXY;
layout(location = 1) in vec2 IA_Patch_UV;

layout(location = 6) out vec2 HS_Patch_PositionTangentXY;
layout(location = 7) out vec2 HS_Patch_UV;

void main() 
{
	HS_Patch_PositionTangentXY = IA_Patch_PositionTangentXY;
	HS_Patch_UV = IA_Patch_UV;
})";

static char const *const Geometry_Terrain_TessellationControl =
R"(#version 310 es
#extension GL_EXT_tessellation_shader : enable
#extension GL_OES_tessellation_shader : enable

//glPatchParameteriOES(GL_PATCH_VERTICES_OES, 2)

layout(location = 6) in vec2 VS_Patch_PositionTangentXY[];
layout(location = 7) in vec2 VS_Patch_UV[];

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_TerrainTile
{
    //Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	highp mat4x4 g_TangentToWorld;//���任 Tangent Bitangent Normal ƽ�Ʊ任 Origin
};

layout(binding = 10) uniform highp sampler2D g_TextureHeightMaxMin;

layout(location = 6) out vec2 DS_Patch_PositionTangentXY[];
layout(location = 7) out vec2 DS_Patch_UV[];

layout(vertices = 2) out; //Output Patch Control Point

void main()
{
    DS_Patch_PositionTangentXY[gl_InvocationID] = VS_Patch_PositionTangentXY[gl_InvocationID];
    DS_Patch_UV[gl_InvocationID] = VS_Patch_UV[gl_InvocationID];

	//Patch Constant Function   
    barrier();

	//Frustum Cull
	vec2 l_PatchCenter_UV;
	l_PatchCenter_UV.x = (VS_Patch_UV[0].x + VS_Patch_UV[1].x) * 0.5f;
	l_PatchCenter_UV.y = (VS_Patch_UV[0].y + VS_Patch_UV[1].y) * 0.5f;

	vec2 l_Patch_Displacement_MaxMin = texture(g_TextureHeightMaxMin, l_PatchCenter_UV).rg * 30.0f;

	vec3 l_Patch_AABB_Point_PositionTangent[8]; 
	l_Patch_AABB_Point_PositionTangent[0] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[1] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[2] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[3] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[4] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[5] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[6] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[7] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.y);

	vec3 l_Patch_AABB_Point_PositionWorld[8]; 
	for(int i = 0; i < 8; ++i)
	{
		l_Patch_AABB_Point_PositionWorld[i] = (g_TangentToWorld * vec4(l_Patch_AABB_Point_PositionTangent[i], 1.0f)).xyz;
	}

	vec3 l_Patch_AABB_World_Max = max(
		max(
			max(l_Patch_AABB_Point_PositionWorld[0], l_Patch_AABB_Point_PositionWorld[1]),
			max(l_Patch_AABB_Point_PositionWorld[2], l_Patch_AABB_Point_PositionWorld[3])
		),
		max(
			max(l_Patch_AABB_Point_PositionWorld[4], l_Patch_AABB_Point_PositionWorld[5]),
			max(l_Patch_AABB_Point_PositionWorld[6], l_Patch_AABB_Point_PositionWorld[7])
		)
	);

	vec3 l_Patch_AABB_World_Min = min(
		min(
			min(l_Patch_AABB_Point_PositionWorld[0], l_Patch_AABB_Point_PositionWorld[1]),
			min(l_Patch_AABB_Point_PositionWorld[2], l_Patch_AABB_Point_PositionWorld[3])
		),
		min(
			min(l_Patch_AABB_Point_PositionWorld[4], l_Patch_AABB_Point_PositionWorld[5]),
			min(l_Patch_AABB_Point_PositionWorld[6], l_Patch_AABB_Point_PositionWorld[7])
		)
	);

	vec3 l_Patch_AABB_World_Center = (l_Patch_AABB_World_Max + l_Patch_AABB_World_Min) * 0.5f;
	vec3 l_Patch_AABB_World_HalfDiagonal = (l_Patch_AABB_World_Max - l_Patch_AABB_World_Min) * 0.5f;

	bool l_InVisible = false;
	for(int i = 0; i < 6; ++i)
	{
		//���������ĵ�ƽ��������һ����ƽ�淨�����ϵ�ͶӰ
		float l_Distance = dot(g_FrustumPlane[i], vec4(l_Patch_AABB_World_Center,1.0f));
		
		//������8����Խ�����ƽ�淨�����ϵ�ͶӰ�����ֵ
		//ӳ��
		//��������
		vec3 l_Temp =  l_Patch_AABB_World_HalfDiagonal.xyz * g_FrustumPlane[i].xyz;
		//ȡ����ֵ
		l_Temp = abs(l_Temp);
		//��Լ
		float l_HalfDiagonalProjection = l_Temp.x + l_Temp.y + l_Temp.z;

		if(l_Distance < -l_HalfDiagonalProjection)
		{
			l_InVisible = true;
			break;
		}
	}

	if(l_InVisible)
	{
		gl_TessLevelOuter[0] = -1.0f;
		gl_TessLevelOuter[1] = -1.0f;
		gl_TessLevelOuter[2] = -1.0f;
		gl_TessLevelOuter[3] = -1.0f;
		gl_TessLevelInner[0] = -1.0f;
		gl_TessLevelInner[1] = -1.0f;
	}
	else
	{
		//Level Of Detail

		//����->��ӳDensity
		vec2 l_Patch_Displacement_MaxMin_R = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(1,0)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_T = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(0,1)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_L = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(-1,0)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_B = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(0,-1)).rg * 30.0f;

		float l_Patch_Range_M = l_Patch_Displacement_MaxMin.x - l_Patch_Displacement_MaxMin.y;
		float l_Patch_Range_R = l_Patch_Displacement_MaxMin_R.x - l_Patch_Displacement_MaxMin_R.y;
		float l_Patch_Range_T = l_Patch_Displacement_MaxMin_T.x - l_Patch_Displacement_MaxMin_T.y;
		float l_Patch_Range_L = l_Patch_Displacement_MaxMin_L.x - l_Patch_Displacement_MaxMin_L.y;
		float l_Patch_Range_B = l_Patch_Displacement_MaxMin_B.x - l_Patch_Displacement_MaxMin_B.y;

		float l_Range[4];
		l_Range[0] = (l_Patch_Range_M + l_Patch_Range_L) * 0.5f;
		l_Range[1] = (l_Patch_Range_M + l_Patch_Range_B) * 0.5f;
		l_Range[2] = (l_Patch_Range_M + l_Patch_Range_R) * 0.5f;
		l_Range[3] = (l_Patch_Range_M + l_Patch_Range_T) * 0.5f;

		//ͶӰ->��ӳDistance
		vec3 l_Patch_Point_PositionTangent[4];
		l_Patch_Point_PositionTangent[0] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, 0.0f);
		l_Patch_Point_PositionTangent[1] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, 0.0f);
		l_Patch_Point_PositionTangent[2] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, 0.0f);
		l_Patch_Point_PositionTangent[3] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, 0.0f);

		vec3 l_Patch_Point_PositionView[5];
		for(int i = 0; i < 4; ++i)
		{
			l_Patch_Point_PositionView[i] = (g_View * g_TangentToWorld * vec4(l_Patch_Point_PositionTangent[i], 1.0f)).xyz;
		}
		l_Patch_Point_PositionView[4] = l_Patch_Point_PositionView[0];
		
		float l_Diameter_Projection[4];
		for(int i = 0; i < 4; ++i)
		{
			vec3 l_Center = (l_Patch_Point_PositionView[i] + l_Patch_Point_PositionView[i+1]) * 0.5f;
			vec3 l_SensorToCenter = l_Center;//l_Center - 0,0,0
			//vec3 l_ViewDirection = vec3(0.0f,0.0f,-1.0f);
			//float l_Distance = abs(dot(l_SensorToCenter,l_ViewDirection));//�Ͻ���Edge��Center���ܱ�NearZƽ��������Ӷ�Ϊ��ֵ
			float l_Distance = abs(l_SensorToCenter.z);
			float l_Diameter = length(l_Patch_Point_PositionView[i] - l_Patch_Point_PositionView[i+1]);
			l_Diameter_Projection[i] =  l_Diameter * (1.0f / l_Distance);
		}


		gl_TessLevelOuter[0] = pow(l_Range[0],1.25f)*pow(l_Diameter_Projection[0],1.25f)*8.0f; //5.0f = 1.0f/2.0f(NormalizedDevice)*800.0f(ScreenSize)/80.0f(TesselationTriangleSize)
		gl_TessLevelOuter[1] = pow(l_Range[1],1.25f)*pow(l_Diameter_Projection[1],1.25f)*8.0f;
		gl_TessLevelOuter[2] = pow(l_Range[2],1.25f)*pow(l_Diameter_Projection[2],1.25f)*8.0f;
		gl_TessLevelOuter[3] = pow(l_Range[3],1.25f)*pow(l_Diameter_Projection[3],1.25f)*8.0f;
		gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) * 0.5f;
		gl_TessLevelInner[1] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]) * 0.5f;
	}
})";

static char const *const Geometry_Terrain_TessellationEvaluation =
R"(#version 310 es
#extension GL_EXT_tessellation_shader : enable
#extension GL_OES_tessellation_shader : enable

layout(quads, equal_spacing, ccw) in; //domain partitioning outputtopology

layout(location = 6) in highp vec2 HS_Patch_PositionTangentXY[];
layout(location = 7) in highp vec2 HS_Patch_UV[];

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_TerrainTile
{
    //Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	highp mat4x4 g_TangentToWorld;//���任 Tangent Bitangent Normal ƽ�Ʊ任 Origin
};

layout(binding = 11) uniform highp sampler2D g_TextureHeight;

out gl_PerVertex{
    vec4 gl_Position;
};

//layout(location = 6) out highp vec3 RS_Tessellation_Normal;
layout(location = 7) out highp vec2 RS_Tessellation_UV_Height;
layout(location = 10) out highp vec3 RS_PositionWorld;

void main()
{
    //UV
    vec2 l_Tessellation_UV;
    l_Tessellation_UV.x = mix(HS_Patch_UV[0].x, HS_Patch_UV[1].x, gl_TessCoord.x);
    l_Tessellation_UV.y = mix(HS_Patch_UV[0].y, HS_Patch_UV[1].y, gl_TessCoord.y);

#if 0
    //Gather
    //(0,1) (1,1) (1,0) (0,0)
    vec4 l_Tessellation_NXRM = textureGather(g_TextureDisplacement, l_Tessellation_UV, 0);
    vec4 l_Tessellation_LMBX = textureGatherOffset(g_TextureDisplacement, l_Tessellation_UV, ivec2(-1,-1), 0);
#endif

    float l_Tessellation_M = texture(g_TextureHeight, l_Tessellation_UV).r * 30.0f;

    //Displacement
    vec3 l_Tessellation_PositionTangent;
    l_Tessellation_PositionTangent.x = mix(HS_Patch_PositionTangentXY[0].x, HS_Patch_PositionTangentXY[1].x, gl_TessCoord.x);
    l_Tessellation_PositionTangent.y = mix(HS_Patch_PositionTangentXY[0].y, HS_Patch_PositionTangentXY[1].y, gl_TessCoord.y);
    l_Tessellation_PositionTangent.z = l_Tessellation_M;

//����!!!
//��Pixel Shader�м���Normal��ʹ�����θ���ά���ڽϵ�ˮƽ��ԭ����NormalMap����
#if 0
    //Normal
	vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_LMBX.x - l_Tessellation_NXRM.z) * 30.0f, 
			(l_Tessellation_LMBX.z - l_Tessellation_NXRM.x) * 30.0f, 
			0.5f//256Tile/512Texture
			));

    RS_Tessellation_Normal = mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * l_Tessellation_NormalTangent;
#endif

    vec4 l_Tessellation_PositionWorld = g_TangentToWorld * vec4(l_Tessellation_PositionTangent, 1.0f);
    RS_PositionWorld = l_Tessellation_PositionWorld.xyz;

    gl_Position = g_Projection * g_View * l_Tessellation_PositionWorld;
    RS_Tessellation_UV_Height = l_Tessellation_UV;
})";

static char const *const Geometry_Terrain_GBufferPass_Fragment =
R"(#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(location = 7) in sample highp vec2 RS_Tessellation_UV_Height;

layout(binding = 11) uniform highp sampler2D g_TextureHeight;
layout(binding = 13) uniform highp sampler2DArray g_TextureAlbedo;

layout(row_major, binding = 4) uniform g_BufferConstant_TerrainTile
{
    //Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	highp mat4x4 g_TangentToWorld;//���任 Tangent Bitangent Normal ƽ�Ʊ任 Origin
};

layout(location = 0) out highp vec4 SV_Target0; //NormalMacro
layout(location = 1) out highp vec4 SV_Target1; //NormalMeso
layout(location = 2) out highp vec4 SV_Target2; //Albedo
layout(location = 3) out highp vec4 SV_Target3; //Specular

layout (early_fragment_tests) in;

void main() 
{

//��Domain Shader�м���Normal�ķ����Ѿ�������
#if 0
	//highp vec3 Normal = normalize(RS_Tessellation_Normal);
#endif

//Gather�ᵼ�¡�դ�񻯡���ʹ��Linear Sample����Ч���⡰դ�񻯡�����
#if 0
	//Gather
    //(0,1) (1,1) (1,0) (0,0)
    highp vec4 l_Tessellation_NXRM = textureGather(g_TextureDisplacement, RS_Tessellation_UV_Height, 0);
    //highp vec4 l_Tessellation_LMBX = textureGatherOffset(g_TextureDisplacement, RS_Tessellation_UV_Height, ivec2(-1,-1), 0);
	
	//Normal
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.z) * 30.0f, 
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.x) * 30.0f, 
			0.5f//256Tile/512Texture
			));
#endif

	highp float l_Tessellation_M = texture(g_TextureHeight, RS_Tessellation_UV_Height).r * 30.0f;
	highp float l_Tessellation_R = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(1,0)).r * 30.0f;
	highp float l_Tessellation_N = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(0,1)).r * 30.0f;
	
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_M - l_Tessellation_R), 
			(l_Tessellation_M - l_Tessellation_N), 
			0.5f//256Tile/512Texture
			));

	highp vec3 NormalMeso = mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * l_Tessellation_NormalTangent;
	highp vec3 NormalMacro = NormalMeso;

	//����������Terrain Tessellation�޹�------------------------------------------------------

    //NVIDIA GameWorks Vulkan and OpenGL Samples / Texture Array Terrain Sample
    //https://developer.nvidia.com/gameworks-vulkan-and-opengl-samples
    
    //�Թ���ʽ�ķ�ʽ���ݸ߶�ȷ���ں�����
    //������Ϸ��������������һ����ȫ��ͬ���������ں�����������������������Asset�У���С��HeightField��ͬ

    highp vec3 l_Tessellation_UVW_Albedo = vec3(RS_Tessellation_UV_Height*16.0f, l_Tessellation_M / 15.0f); //PerPatch //Sample Wrap MirroredRepeat

	highp vec3 Albedo = texture(g_TextureAlbedo,l_Tessellation_UVW_Albedo).xyz;
    
	highp vec3 Specular = vec3(0.0f,0.0f,0.0f);

	SV_Target0 = vec4((NormalMacro+1.0f)*0.5f,1.0f);	
	SV_Target1 = vec4((NormalMeso+1.0f)*0.5f,1.0f);	
	SV_Target2 = vec4(Albedo,1.0f);	
	SV_Target3 = vec4(Specular,1.0f);	
})";

static char const *const Geometry_Terrain_PlanarReflectionPass_Fragment =
R"(#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(location = 7) in sample highp vec2 RS_Tessellation_UV_Height;

layout(location = 10) in sample highp vec3 RS_PositionWorld;

layout(binding = 11) uniform highp sampler2D g_TextureHeight;
layout(binding = 13) uniform highp sampler2DArray g_TextureAlbedo;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_TerrainTile
{
    //Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	highp mat4x4 g_TangentToWorld;//���任 Tangent Bitangent Normal ƽ�Ʊ任 Origin
};

layout(row_major, binding = 7) uniform g_BufferConstant_Light
{
	highp mat4x4 g_LightView;
	highp mat4x4 g_LightProjection;
	highp vec4 g_LightIrradiance;
	highp vec4 g_LightDirection;
};

layout(location = 0) out highp vec4 OM_RadianceDirect;

layout (early_fragment_tests) in;

void main() 
{

//��Domain Shader�м���Normal�ķ����Ѿ�������
#if 0
	//highp vec3 Normal = normalize(RS_Tessellation_Normal);
#endif

//Gather�ᵼ�¡�դ�񻯡���ʹ��Linear Sample����Ч���⡰դ�񻯡�����
#if 0
	//Gather
    //(0,1) (1,1) (1,0) (0,0)
    highp vec4 l_Tessellation_NXRM = textureGather(g_TextureDisplacement, RS_Tessellation_UV_Height, 0);
    //highp vec4 l_Tessellation_LMBX = textureGatherOffset(g_TextureDisplacement, RS_Tessellation_UV_Height, ivec2(-1,-1), 0);
	
	//Normal
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.z) * 30.0f, 
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.x) * 30.0f, 
			0.5f//256Tile/512Texture
			));
#endif

	highp float l_Tessellation_M = texture(g_TextureHeight, RS_Tessellation_UV_Height).r * 30.0f;
	highp float l_Tessellation_R = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(1,0)).r * 30.0f;
	highp float l_Tessellation_N = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(0,1)).r * 30.0f;
	
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_M - l_Tessellation_R), 
			(l_Tessellation_M - l_Tessellation_N), 
			0.5f//256Tile/512Texture
			));

	//Material
	highp vec3 NormalMeso = mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * l_Tessellation_NormalTangent;
	highp vec3 NormalMacro = NormalMeso;

	//����������Terrain Tessellation�޹�------------------------------------------------------

    //NVIDIA GameWorks Vulkan and OpenGL Samples / Texture Array Terrain Sample
    //https://developer.nvidia.com/gameworks-vulkan-and-opengl-samples
    
    //�Թ���ʽ�ķ�ʽ���ݸ߶�ȷ���ں�����
    //������Ϸ��������������һ����ȫ��ͬ���������ں�����������������������Asset�У���С��HeightField��ͬ

    highp vec3 l_Tessellation_UVW_Albedo = vec3(RS_Tessellation_UV_Height*16.0f, l_Tessellation_M / 15.0f); //PerPatch //Sample Wrap MirroredRepeat

	highp vec3 ColorAlbedo = texture(g_TextureAlbedo,l_Tessellation_UVW_Albedo).xyz;
    
	highp vec3 ColorSpecular = vec3(0.0f,0.0f,0.0f);
	
	highp float Glossiness = 1.0f;

	//Forward
	//Light_Directional
	highp vec3 ViewRadiance = vec3(0.0f, 0.0f, 0.0f);

	highp vec3 ViewDirection = normalize(g_InvView[3].xyz - RS_PositionWorld); //PositionSensor_World - Position_World

	highp vec3 LightDirection = - normalize(g_LightDirection.xyz);

	//E_N
    highp vec3 IrradianceNormal = g_LightIrradiance.xyz*max(0.0f, dot(NormalMeso, LightDirection));

    //Normalized Blinn-Phong
    highp vec3 HalfVector = ViewDirection + LightDirection; //��NormalMicro

    highp float Smoothness = Glossiness * 5.0f; //��Glossiness����� 

    highp float NormalDistribution =  ((Smoothness + 2.0f)/6.28f) * pow(max(0.0f, dot(HalfVector,NormalMeso)),Smoothness);

    highp vec3 Fresnel = ColorSpecular;

    highp vec3 BRDF_Specular = NormalDistribution * Fresnel * 0.25f; //G(l,v) = max(0,cos<LightDirection,NormalMeso>)*max(0,cos<ViewDirection,NormalMeso>)

    //Lambert
    highp vec3 ColorDiffuse = ColorAlbedo * (vec3(1.0f,1.0f,1.0f) - Fresnel);

    highp vec3 BRDF_Diffuse = ColorDiffuse / 3.14f;

    //L_V
    ViewRadiance += (BRDF_Specular + BRDF_Diffuse) * IrradianceNormal;    

	OM_RadianceDirect = vec4(ViewRadiance,1.0f);
})";

void Render_Geometry_Terrain::Init()
{
	//m_hBufferVertex_Terrain
	{
		::glGenBuffers(1, &m_hBufferVertex_Terrain);
		::glBindBuffer(GL_ARRAY_BUFFER, m_hBufferVertex_Terrain);
		float BufferVertexData[] =
		{
			-128.0f, -128.0f, 		0.0f, 0.0f,
			-128.0f, -112.0f, 		0.0f, 0.0625f,
			-128.0f, -96.0f, 		0.0f, 0.125f,
			-128.0f, -80.0f, 		0.0f, 0.1875f,
			-128.0f, -64.0f, 		0.0f, 0.25f,
			-128.0f, -48.0f, 		0.0f, 0.3125f,
			-128.0f, -32.0f, 		0.0f, 0.375f,
			-128.0f, -16.0f, 		0.0f, 0.4375f,
			-128.0f, 0.0f, 			0.0f, 0.5f,
			-128.0f, 16.0f, 		0.0f, 0.5625f,
			-128.0f, 32.0f, 		0.0f, 0.625f,
			-128.0f, 48.0f, 		0.0f, 0.6875f,
			-128.0f, 64.0f, 		0.0f, 0.75f,
			-128.0f, 80.0f, 		0.0f, 0.8125f,
			-128.0f, 96.0f, 		0.0f, 0.875f,
			-128.0f, 112.0f, 		0.0f, 0.9375f,
			-128.0f, 128.0f, 		0.0f, 1.0f,
			-112.0f, -128.0f, 		0.0625f, 0.0f,
			-112.0f, -112.0f, 		0.0625f, 0.0625f,
			-112.0f, -96.0f, 		0.0625f, 0.125f,
			-112.0f, -80.0f, 		0.0625f, 0.1875f,
			-112.0f, -64.0f, 		0.0625f, 0.25f,
			-112.0f, -48.0f, 		0.0625f, 0.3125f,
			-112.0f, -32.0f, 		0.0625f, 0.375f,
			-112.0f, -16.0f, 		0.0625f, 0.4375f,
			-112.0f, 0.0f, 			0.0625f, 0.5f,
			-112.0f, 16.0f, 		0.0625f, 0.5625f,
			-112.0f, 32.0f, 		0.0625f, 0.625f,
			-112.0f, 48.0f, 		0.0625f, 0.6875f,
			-112.0f, 64.0f, 		0.0625f, 0.75f,
			-112.0f, 80.0f, 		0.0625f, 0.8125f,
			-112.0f, 96.0f, 		0.0625f, 0.875f,
			-112.0f, 112.0f, 		0.0625f, 0.9375f,
			-112.0f, 128.0f, 		0.0625f, 1.0f,
			-96.0f, -128.0f, 		0.125f, 0.0f,
			-96.0f, -112.0f, 		0.125f, 0.0625f,
			-96.0f, -96.0f, 		0.125f, 0.125f,
			-96.0f, -80.0f, 		0.125f, 0.1875f,
			-96.0f, -64.0f, 		0.125f, 0.25f,
			-96.0f, -48.0f, 		0.125f, 0.3125f,
			-96.0f, -32.0f, 		0.125f, 0.375f,
			-96.0f, -16.0f, 		0.125f, 0.4375f,
			-96.0f, 0.0f, 			0.125f, 0.5f,
			-96.0f, 16.0f, 			0.125f, 0.5625f,
			-96.0f, 32.0f, 			0.125f, 0.625f,
			-96.0f, 48.0f, 			0.125f, 0.6875f,
			-96.0f, 64.0f, 			0.125f, 0.75f,
			-96.0f, 80.0f, 			0.125f, 0.8125f,
			-96.0f, 96.0f, 			0.125f, 0.875f,
			-96.0f, 112.0f, 		0.125f, 0.9375f,
			-96.0f, 128.0f, 		0.125f, 1.0f,
			-80.0f, -128.0f, 		0.1875f, 0.0f,
			-80.0f, -112.0f, 		0.1875f, 0.0625f,
			-80.0f, -96.0f, 		0.1875f, 0.125f,
			-80.0f, -80.0f, 		0.1875f, 0.1875f,
			-80.0f, -64.0f, 		0.1875f, 0.25f,
			-80.0f, -48.0f, 		0.1875f, 0.3125f,
			-80.0f, -32.0f, 		0.1875f, 0.375f,
			-80.0f, -16.0f, 		0.1875f, 0.4375f,
			-80.0f, 0.0f, 			0.1875f, 0.5f,
			-80.0f, 16.0f, 			0.1875f, 0.5625f,
			-80.0f, 32.0f, 			0.1875f, 0.625f,
			-80.0f, 48.0f, 			0.1875f, 0.6875f,
			-80.0f, 64.0f, 			0.1875f, 0.75f,
			-80.0f, 80.0f, 			0.1875f, 0.8125f,
			-80.0f, 96.0f, 			0.1875f, 0.875f,
			-80.0f, 112.0f, 		0.1875f, 0.9375f,
			-80.0f, 128.0f, 		0.1875f, 1.0f,
			-64.0f, -128.0f, 		0.25f, 0.0f,
			-64.0f, -112.0f, 		0.25f, 0.0625f,
			-64.0f, -96.0f, 		0.25f, 0.125f,
			-64.0f, -80.0f, 		0.25f, 0.1875f,
			-64.0f, -64.0f, 		0.25f, 0.25f,
			-64.0f, -48.0f, 		0.25f, 0.3125f,
			-64.0f, -32.0f, 		0.25f, 0.375f,
			-64.0f, -16.0f, 		0.25f, 0.4375f,
			-64.0f, 0.0f, 			0.25f, 0.5f,
			-64.0f, 16.0f,			0.25f, 0.5625f,
			-64.0f, 32.0f, 			0.25f, 0.625f,
			-64.0f, 48.0f, 			0.25f, 0.6875f,
			-64.0f, 64.0f, 			0.25f, 0.75f,
			-64.0f, 80.0f, 			0.25f, 0.8125f,
			-64.0f, 96.0f, 			0.25f, 0.875f,
			-64.0f, 112.0f, 		0.25f, 0.9375f,
			-64.0f, 128.0f, 		0.25f, 1.0f,
			-48.0f, -128.0f, 		0.3125f, 0.0f,
			-48.0f, -112.0f, 		0.3125f, 0.0625f,
			-48.0f, -96.0f, 		0.3125f, 0.125f,
			-48.0f, -80.0f, 		0.3125f, 0.1875f,
			-48.0f, -64.0f, 		0.3125f, 0.25f,
			-48.0f, -48.0f, 		0.3125f, 0.3125f,
			-48.0f, -32.0f, 		0.3125f, 0.375f,
			-48.0f, -16.0f, 		0.3125f, 0.4375f,
			-48.0f, 0.0f, 			0.3125f, 0.5f,
			-48.0f, 16.0f, 			0.3125f, 0.5625f,
			-48.0f, 32.0f, 			0.3125f, 0.625f,
			-48.0f, 48.0f, 			0.3125f, 0.6875f,
			-48.0f, 64.0f, 			0.3125f, 0.75f,
			-48.0f, 80.0f, 			0.3125f, 0.8125f,
			-48.0f, 96.0f, 			0.3125f, 0.875f,
			-48.0f, 112.0f, 		0.3125f, 0.9375f,
			-48.0f, 128.0f, 		0.3125f, 1.0f,
			-32.0f, -128.0f, 		0.375f, 0.0f,
			-32.0f, -112.0f, 		0.375f, 0.0625f,
			-32.0f, -96.0f, 		0.375f, 0.125f,
			-32.0f, -80.0f, 		0.375f, 0.1875f,
			-32.0f, -64.0f, 		0.375f, 0.25f,
			-32.0f, -48.0f, 		0.375f, 0.3125f,
			-32.0f, -32.0f, 		0.375f, 0.375f,
			-32.0f, -16.0f, 		0.375f, 0.4375f,
			-32.0f, 0.0f, 			0.375f, 0.5f,
			-32.0f, 16.0f, 			0.375f, 0.5625f,
			-32.0f, 32.0f, 			0.375f, 0.625f,
			-32.0f, 48.0f, 			0.375f, 0.6875f,
			-32.0f, 64.0f, 			0.375f, 0.75f,
			-32.0f, 80.0f, 			0.375f, 0.8125f,
			-32.0f, 96.0f, 			0.375f, 0.875f,
			-32.0f, 112.0f, 		0.375f, 0.9375f,
			-32.0f, 128.0f, 		0.375f, 1.0f,
			-16.0f, -128.0f, 		0.4375f, 0.0f,
			-16.0f, -112.0f, 		0.4375f, 0.0625f,
			-16.0f, -96.0f, 		0.4375f, 0.125f,
			-16.0f, -80.0f, 		0.4375f, 0.1875f,
			-16.0f, -64.0f, 		0.4375f, 0.25f,
			-16.0f, -48.0f, 		0.4375f, 0.3125f,
			-16.0f, -32.0f, 		0.4375f, 0.375f,
			-16.0f, -16.0f, 		0.4375f, 0.4375f,
			-16.0f, 0.0f, 			0.4375f, 0.5f,
			-16.0f, 16.0f, 			0.4375f, 0.5625f,
			-16.0f, 32.0f, 			0.4375f, 0.625f,
			-16.0f, 48.0f, 			0.4375f, 0.6875f,
			-16.0f, 64.0f, 			0.4375f, 0.75f,
			-16.0f, 80.0f, 			0.4375f, 0.8125f,
			-16.0f, 96.0f, 			0.4375f, 0.875f,
			-16.0f, 112.0f, 		0.4375f, 0.9375f,
			-16.0f, 128.0f, 		0.4375f, 1.0f,
			0.0f, -128.0f, 			0.5f, 0.0f,
			0.0f, -112.0f, 			0.5f, 0.0625f,
			0.0f, -96.0f, 			0.5f, 0.125f,
			0.0f, -80.0f, 			0.5f, 0.1875f,
			0.0f, -64.0f, 			0.5f, 0.25f,
			0.0f, -48.0f, 			0.5f, 0.3125f,
			0.0f, -32.0f, 			0.5f, 0.375f,
			0.0f, -16.0f, 			0.5f, 0.4375f,
			0.0f, 0.0f, 			0.5f, 0.5f,
			0.0f, 16.0f, 			0.5f, 0.5625f,
			0.0f, 32.0f, 			0.5f, 0.625f,
			0.0f, 48.0f, 			0.5f, 0.6875f,
			0.0f, 64.0f, 			0.5f, 0.75f,
			0.0f, 80.0f, 			0.5f, 0.8125f,
			0.0f, 96.0f, 			0.5f, 0.875f,
			0.0f, 112.0f, 			0.5f, 0.9375f,
			0.0f, 128.0f, 			0.5f, 1.0f,
			16.0f, -128.0f, 		0.5625f, 0.0f,
			16.0f, -112.0f, 		0.5625f, 0.0625f,
			16.0f, -96.0f, 			0.5625f, 0.125f,
			16.0f, -80.0f, 			0.5625f, 0.1875f,
			16.0f, -64.0f, 			0.5625f, 0.25f,
			16.0f, -48.0f, 			0.5625f, 0.3125f,
			16.0f, -32.0f, 			0.5625f, 0.375f,
			16.0f, -16.0f, 			0.5625f, 0.4375f,
			16.0f, 0.0f, 			0.5625f, 0.5f,
			16.0f, 16.0f, 			0.5625f, 0.5625f,
			16.0f, 32.0f, 			0.5625f, 0.625f,
			16.0f, 48.0f, 			0.5625f, 0.6875f,
			16.0f, 64.0f, 			0.5625f, 0.75f,
			16.0f, 80.0f, 			0.5625f, 0.8125f,
			16.0f, 96.0f, 			0.5625f, 0.875f,
			16.0f, 112.0f, 			0.5625f, 0.9375f,
			16.0f, 128.0f, 			0.5625f, 1.0f,
			32.0f, -128.0f, 		0.625f, 0.0f,
			32.0f, -112.0f, 		0.625f, 0.0625f,
			32.0f, -96.0f, 			0.625f, 0.125f,
			32.0f, -80.0f, 			0.625f, 0.1875f,
			32.0f, -64.0f, 			0.625f, 0.25f,
			32.0f, -48.0f, 			0.625f, 0.3125f,
			32.0f, -32.0f, 			0.625f, 0.375f,
			32.0f, -16.0f, 			0.625f, 0.4375f,
			32.0f, 0.0f, 			0.625f, 0.5f,
			32.0f, 16.0f, 			0.625f, 0.5625f,
			32.0f, 32.0f, 			0.625f, 0.625f,
			32.0f, 48.0f, 			0.625f, 0.6875f,
			32.0f, 64.0f, 			0.625f, 0.75f,
			32.0f, 80.0f, 			0.625f, 0.8125f,
			32.0f, 96.0f, 			0.625f, 0.875f,
			32.0f, 112.0f, 			0.625f, 0.9375f,
			32.0f, 128.0f, 			0.625f, 1.0f,
			48.0f, -128.0f, 		0.6875f, 0.0f,
			48.0f, -112.0f, 		0.6875f, 0.0625f,
			48.0f, -96.0f, 			0.6875f, 0.125f,
			48.0f, -80.0f, 			0.6875f, 0.1875f,
			48.0f, -64.0f, 			0.6875f, 0.25f,
			48.0f, -48.0f, 			0.6875f, 0.3125f,
			48.0f, -32.0f, 			0.6875f, 0.375f,
			48.0f, -16.0f, 			0.6875f, 0.4375f,
			48.0f, 0.0f, 			0.6875f, 0.5f,
			48.0f, 16.0f, 			0.6875f, 0.5625f,
			48.0f, 32.0f, 			0.6875f, 0.625f,
			48.0f, 48.0f, 			0.6875f, 0.6875f,
			48.0f, 64.0f, 			0.6875f, 0.75f,
			48.0f, 80.0f, 			0.6875f, 0.8125f,
			48.0f, 96.0f, 			0.6875f, 0.875f,
			48.0f, 112.0f, 			0.6875f, 0.9375f,
			48.0f, 128.0f, 			0.6875f, 1.0f,
			64.0f, -128.0f, 		0.75f, 0.0f,
			64.0f, -112.0f, 		0.75f, 0.0625f,
			64.0f, -96.0f, 			0.75f, 0.125f,
			64.0f, -80.0f, 			0.75f, 0.1875f,
			64.0f, -64.0f, 			0.75f, 0.25f,
			64.0f, -48.0f, 			0.75f, 0.3125f,
			64.0f, -32.0f, 			0.75f, 0.375f,
			64.0f, -16.0f, 			0.75f, 0.4375f,
			64.0f, 0.0f, 			0.75f, 0.5f,
			64.0f, 16.0f, 			0.75f, 0.5625f,
			64.0f, 32.0f, 			0.75f, 0.625f,
			64.0f, 48.0f, 			0.75f, 0.6875f,
			64.0f, 64.0f, 			0.75f, 0.75f,
			64.0f, 80.0f, 			0.75f, 0.8125f,
			64.0f, 96.0f, 			0.75f, 0.875f,
			64.0f, 112.0f, 			0.75f, 0.9375f,
			64.0f, 128.0f, 			0.75f, 1.0f,
			80.0f, -128.0f, 		0.8125f, 0.0f,
			80.0f, -112.0f, 		0.8125f, 0.0625f,
			80.0f, -96.0f, 			0.8125f, 0.125f,
			80.0f, -80.0f, 			0.8125f, 0.1875f,
			80.0f, -64.0f, 			0.8125f, 0.25f,
			80.0f, -48.0f, 			0.8125f, 0.3125f,
			80.0f, -32.0f, 			0.8125f, 0.375f,
			80.0f, -16.0f, 			0.8125f, 0.4375f,
			80.0f, 0.0f, 			0.8125f, 0.5f,
			80.0f, 16.0f, 			0.8125f, 0.5625f,
			80.0f, 32.0f, 			0.8125f, 0.625f,
			80.0f, 48.0f, 			0.8125f, 0.6875f,
			80.0f, 64.0f, 			0.8125f, 0.75f,
			80.0f, 80.0f, 			0.8125f, 0.8125f,
			80.0f, 96.0f, 			0.8125f, 0.875f,
			80.0f, 112.0f, 			0.8125f, 0.9375f,
			80.0f, 128.0f, 			0.8125f, 1.0f,
			96.0f, -128.0f, 		0.875f, 0.0f,
			96.0f, -112.0f, 		0.875f, 0.0625f,
			96.0f, -96.0f, 			0.875f, 0.125f,
			96.0f, -80.0f, 			0.875f, 0.1875f,
			96.0f, -64.0f, 			0.875f, 0.25f,
			96.0f, -48.0f, 			0.875f, 0.3125f,
			96.0f, -32.0f, 			0.875f, 0.375f,
			96.0f, -16.0f, 			0.875f, 0.4375f,
			96.0f, 0.0f, 			0.875f, 0.5f,
			96.0f, 16.0f, 			0.875f, 0.5625f,
			96.0f, 32.0f, 			0.875f, 0.625f,
			96.0f, 48.0f, 			0.875f, 0.6875f,
			96.0f, 64.0f, 			0.875f, 0.75f,
			96.0f, 80.0f, 			0.875f, 0.8125f,
			96.0f, 96.0f, 			0.875f, 0.875f,
			96.0f, 112.0f, 			0.875f, 0.9375f,
			96.0f, 128.0f, 			0.875f, 1.0f,
			112.0f, -128.0f, 		0.9375f, 0.0f,
			112.0f, -112.0f, 		0.9375f, 0.0625f,
			112.0f, -96.0f, 		0.9375f, 0.125f,
			112.0f, -80.0f, 		0.9375f, 0.1875f,
			112.0f, -64.0f, 		0.9375f, 0.25f,
			112.0f, -48.0f, 		0.9375f, 0.3125f,
			112.0f, -32.0f, 		0.9375f, 0.375f,
			112.0f, -16.0f, 		0.9375f, 0.4375f,
			112.0f, 0.0f, 			0.9375f, 0.5f,
			112.0f, 16.0f, 			0.9375f, 0.5625f,
			112.0f, 32.0f, 			0.9375f, 0.625f,
			112.0f, 48.0f, 			0.9375f, 0.6875f,
			112.0f, 64.0f, 			0.9375f, 0.75f,
			112.0f, 80.0f, 			0.9375f, 0.8125f,
			112.0f, 96.0f, 			0.9375f, 0.875f,
			112.0f, 112.0f, 		0.9375f, 0.9375f,
			112.0f, 128.0f, 		0.9375f, 1.0f,
			128.0f, -128.0f, 		1.0f, 0.0f,
			128.0f, -112.0f, 		1.0f, 0.0625f,
			128.0f, -96.0f, 		1.0f, 0.125f,
			128.0f, -80.0f, 		1.0f, 0.1875f,
			128.0f, -64.0f, 		1.0f, 0.25f,
			128.0f, -48.0f, 		1.0f, 0.3125f,
			128.0f, -32.0f, 		1.0f, 0.375f,
			128.0f, -16.0f, 		1.0f, 0.4375f,
			128.0f, 0.0f, 			1.0f, 0.5f,
			128.0f, 16.0f, 			1.0f, 0.5625f,
			128.0f, 32.0f, 			1.0f, 0.625f,
			128.0f, 48.0f, 			1.0f, 0.6875f,
			128.0f, 64.0f, 			1.0f, 0.75f,
			128.0f, 80.0f, 			1.0f, 0.8125f,
			128.0f, 96.0f, 			1.0f, 0.875f,
			128.0f, 112.0f, 		1.0f, 0.9375f,
			128.0f, 128.0f, 		1.0f, 1.0f
		};
		::glBufferData(GL_ARRAY_BUFFER, sizeof(BufferVertexData), BufferVertexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ARRAY_BUFFER, 0U);
	}

	//m_hBufferIndex_Terrain
	{
		::glGenBuffers(1, &m_hBufferIndex_Terrain);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Terrain);
		uint16_t BufferIndexData[] =
		{
			0, 18,
			1, 19,
			2, 20,
			3, 21,
			4, 22,
			5, 23,
			6, 24,
			7, 25,
			8, 26,
			9, 27,
			10, 28,
			11, 29,
			12, 30,
			13, 31,
			14, 32,
			15, 33,
			17, 35,
			18, 36,
			19, 37,
			20, 38,
			21, 39,
			22, 40,
			23, 41,
			24, 42,
			25, 43,
			26, 44,
			27, 45,
			28, 46,
			29, 47,
			30, 48,
			31, 49,
			32, 50,
			34, 52,
			35, 53,
			36, 54,
			37, 55,
			38, 56,
			39, 57,
			40, 58,
			41, 59,
			42, 60,
			43, 61,
			44, 62,
			45, 63,
			46, 64,
			47, 65,
			48, 66,
			49, 67,
			51, 69,
			52, 70,
			53, 71,
			54, 72,
			55, 73,
			56, 74,
			57, 75,
			58, 76,
			59, 77,
			60, 78,
			61, 79,
			62, 80,
			63, 81,
			64, 82,
			65, 83,
			66, 84,
			68, 86,
			69, 87,
			70, 88,
			71, 89,
			72, 90,
			73, 91,
			74, 92,
			75, 93,
			76, 94,
			77, 95,
			78, 96,
			79, 97,
			80, 98,
			81, 99,
			82, 100,
			83, 101,
			85, 103,
			86, 104,
			87, 105,
			88, 106,
			89, 107,
			90, 108,
			91, 109,
			92, 110,
			93, 111,
			94, 112,
			95, 113,
			96, 114,
			97, 115,
			98, 116,
			99, 117,
			100, 118,
			102, 120,
			103, 121,
			104, 122,
			105, 123,
			106, 124,
			107, 125,
			108, 126,
			109, 127,
			110, 128,
			111, 129,
			112, 130,
			113, 131,
			114, 132,
			115, 133,
			116, 134,
			117, 135,
			119, 137,
			120, 138,
			121, 139,
			122, 140,
			123, 141,
			124, 142,
			125, 143,
			126, 144,
			127, 145,
			128, 146,
			129, 147,
			130, 148,
			131, 149,
			132, 150,
			133, 151,
			134, 152,
			136, 154,
			137, 155,
			138, 156,
			139, 157,
			140, 158,
			141, 159,
			142, 160,
			143, 161,
			144, 162,
			145, 163,
			146, 164,
			147, 165,
			148, 166,
			149, 167,
			150, 168,
			151, 169,
			153, 171,
			154, 172,
			155, 173,
			156, 174,
			157, 175,
			158, 176,
			159, 177,
			160, 178,
			161, 179,
			162, 180,
			163, 181,
			164, 182,
			165, 183,
			166, 184,
			167, 185,
			168, 186,
			170, 188,
			171, 189,
			172, 190,
			173, 191,
			174, 192,
			175, 193,
			176, 194,
			177, 195,
			178, 196,
			179, 197,
			180, 198,
			181, 199,
			182, 200,
			183, 201,
			184, 202,
			185, 203,
			187, 205,
			188, 206,
			189, 207,
			190, 208,
			191, 209,
			192, 210,
			193, 211,
			194, 212,
			195, 213,
			196, 214,
			197, 215,
			198, 216,
			199, 217,
			200, 218,
			201, 219,
			202, 220,
			204, 222,
			205, 223,
			206, 224,
			207, 225,
			208, 226,
			209, 227,
			210, 228,
			211, 229,
			212, 230,
			213, 231,
			214, 232,
			215, 233,
			216, 234,
			217, 235,
			218, 236,
			219, 237,
			221, 239,
			222, 240,
			223, 241,
			224, 242,
			225, 243,
			226, 244,
			227, 245,
			228, 246,
			229, 247,
			230, 248,
			231, 249,
			232, 250,
			233, 251,
			234, 252,
			235, 253,
			236, 254,
			238, 256,
			239, 257,
			240, 258,
			241, 259,
			242, 260,
			243, 261,
			244, 262,
			245, 263,
			246, 264,
			247, 265,
			248, 266,
			249, 267,
			250, 268,
			251, 269,
			252, 270,
			253, 271,
			255, 273,
			256, 274,
			257, 275,
			258, 276,
			259, 277,
			260, 278,
			261, 279,
			262, 280,
			263, 281,
			264, 282,
			265, 283,
			266, 284,
			267, 285,
			268, 286,
			269, 287,
			270, 288
		};
		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BufferIndexData), BufferIndexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
	}

	//m_hInputLayout_Terrain
	{
		::glGenVertexArrays(1, &m_hInputLayout_Terrain);
		::glBindVertexArray(m_hInputLayout_Terrain);
		::glEnableVertexAttribArray(0);
		::glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0U);
		::glVertexAttribBinding(0U, 0U);
		::glEnableVertexAttribArray(1);
		::glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
		::glVertexAttribBinding(1U, 0U);
		::glBindVertexArray(0U);
	}

	GLuint hShader_Geometry_Terrain_Vertex;
	{
		hShader_Geometry_Terrain_Vertex = ::glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_Geometry_Terrain_Vertex, 1U, &::Geometry_Terrain_Vertex, NULL);
		::glCompileShader(hShader_Geometry_Terrain_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Terrain_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Terrain_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Terrain_TessellationControl;
	{
		hShader_Geometry_Terrain_TessellationControl = ::glCreateShader(GL_TESS_CONTROL_SHADER_OES);
		::glShaderSource(hShader_Geometry_Terrain_TessellationControl, 1U, &::Geometry_Terrain_TessellationControl, NULL);
		::glCompileShader(hShader_Geometry_Terrain_TessellationControl);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Terrain_TessellationControl, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Terrain_TessellationControl, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Terrain_TessellationEvaluation;
	{
		hShader_Geometry_Terrain_TessellationEvaluation = ::glCreateShader(GL_TESS_EVALUATION_SHADER_OES);
		::glShaderSource(hShader_Geometry_Terrain_TessellationEvaluation, 1U, &::Geometry_Terrain_TessellationEvaluation, NULL);
		::glCompileShader(hShader_Geometry_Terrain_TessellationEvaluation);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Terrain_TessellationEvaluation, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Terrain_TessellationEvaluation, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Terrain_GBufferPass_Fragment;
	{
		hShader_Geometry_Terrain_GBufferPass_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Terrain_GBufferPass_Fragment, 1U, &Geometry_Terrain_GBufferPass_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Terrain_GBufferPass_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Terrain_GBufferPass_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Terrain_GBufferPass_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Terrain_PlanarReflectionPass_Fragment;
	{
		hShader_Geometry_Terrain_PlanarReflectionPass_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Terrain_PlanarReflectionPass_Fragment, 1U, &Geometry_Terrain_PlanarReflectionPass_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Terrain_PlanarReflectionPass_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Terrain_PlanarReflectionPass_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Terrain_PlanarReflectionPass_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	//m_hProgram_Geometry_Terrain_GBufferPass;
	{
		m_hProgram_Geometry_Terrain_GBufferPass = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Terrain_GBufferPass, hShader_Geometry_Terrain_Vertex);
		::glAttachShader(m_hProgram_Geometry_Terrain_GBufferPass, hShader_Geometry_Terrain_TessellationControl);
		::glAttachShader(m_hProgram_Geometry_Terrain_GBufferPass, hShader_Geometry_Terrain_TessellationEvaluation);
		::glAttachShader(m_hProgram_Geometry_Terrain_GBufferPass, hShader_Geometry_Terrain_GBufferPass_Fragment);

		::glLinkProgram(m_hProgram_Geometry_Terrain_GBufferPass);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Terrain_GBufferPass, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Terrain_GBufferPass, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//m_hProgram_Geometry_Terrain_PlanarReflectionPass
	{
		m_hProgram_Geometry_Terrain_PlanarReflectionPass = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Terrain_PlanarReflectionPass, hShader_Geometry_Terrain_Vertex);
		::glAttachShader(m_hProgram_Geometry_Terrain_PlanarReflectionPass, hShader_Geometry_Terrain_TessellationControl);
		::glAttachShader(m_hProgram_Geometry_Terrain_PlanarReflectionPass, hShader_Geometry_Terrain_TessellationEvaluation);
		::glAttachShader(m_hProgram_Geometry_Terrain_PlanarReflectionPass, hShader_Geometry_Terrain_PlanarReflectionPass_Fragment);

		::glLinkProgram(m_hProgram_Geometry_Terrain_PlanarReflectionPass);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Terrain_PlanarReflectionPass, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Terrain_PlanarReflectionPass, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//m_hSampler_Point
	{
		::glGenSamplers(1, &m_hSampler_Point);
		::glSamplerParameteri(m_hSampler_Point, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_Point, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_Point, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_Point, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		::glSamplerParameteri(m_hSampler_Point, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	//m_hSampler_Linear
	{
		::glGenSamplers(1, &m_hSampler_ClampLinear);
		::glSamplerParameteri(m_hSampler_ClampLinear, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_ClampLinear, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_ClampLinear, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(m_hSampler_ClampLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glSamplerParameteri(m_hSampler_ClampLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//m_hSampler_WrapLinear
	{
		::glGenSamplers(1, &m_hSampler_RepeatLinear);
		::glSamplerParameteri(m_hSampler_RepeatLinear, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
		::glSamplerParameteri(m_hSampler_RepeatLinear, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		::glSamplerParameteri(m_hSampler_RepeatLinear, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		::glSamplerParameteri(m_hSampler_RepeatLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glSamplerParameteri(m_hSampler_RepeatLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//m_hBufferConstant_TerrainTile
	{
		::glGenBuffers(1, &m_hBufferConstant_TerrainTile);
		::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_TerrainTile);
		::glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferConstant_TerrainTileData), NULL, GL_DYNAMIC_DRAW);
		::glBindBuffer(GL_UNIFORM_BUFFER, 0U);
	}
}

void Render_Geometry_Terrain::Execute_GBufferPass(GLuint hBufferConstant_Sensor, BufferConstant_TerrainTileData *pBufferConstant_TerrainTileData, GLuint hTexture_Terrain_Displacement, GLuint hTexture_Terrain_Displacement_MaxMin, GLuint hTexture_Terrain_Albedo)
{
	//Update
	::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_TerrainTile);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferConstant_TerrainTileData), pBufferConstant_TerrainTileData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

	//RS
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);
	//::glPolygonModeNV(GL_FRONT_AND_BACK, GL_LINE_NV);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Terrain_GBufferPass);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_hBufferConstant_TerrainTile);
	//SRV
	::glBindSampler(10, m_hSampler_Point);
	::glActiveTexture(GL_TEXTURE10);
	::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement_MaxMin);
	::glBindSampler(11, m_hSampler_ClampLinear);
	::glActiveTexture(GL_TEXTURE11);
	::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement);
	::glBindSampler(13, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE13);
	::glBindTexture(GL_TEXTURE_2D_ARRAY, hTexture_Terrain_Albedo);

	//IA
	::glBindVertexArray(m_hInputLayout_Terrain);
	::glBindVertexBuffer(0U, m_hBufferVertex_Terrain, 0U, sizeof(float) * 2 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Terrain);

	::glPatchParameteriOES(GL_PATCH_VERTICES_OES, 2);
	::glDrawElements(GL_PATCHES_OES, 512, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0));

	//::glPolygonModeNV(GL_FRONT_AND_BACK, GL_FILL_NV);
}

void Render_Geometry_Terrain::Execute_PlanarReflectionPass(GLuint hBufferConstant_Sensor, BufferConstant_TerrainTileData *pBufferConstant_TerrainTileData, GLuint hTexture_Terrain_Displacement, GLuint hTexture_Terrain_Displacement_MaxMin, GLuint hTexture_Terrain_Albedo, GLuint hBufferConstant_Light)
{
	//Update
	::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_TerrainTile);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferConstant_TerrainTileData), pBufferConstant_TerrainTileData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

	//RS
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);
	//::glPolygonModeNV(GL_FRONT_AND_BACK, GL_LINE_NV);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Terrain_PlanarReflectionPass);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_hBufferConstant_TerrainTile);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 7, hBufferConstant_Light);
	//SRV
	::glBindSampler(10, m_hSampler_Point);
	::glActiveTexture(GL_TEXTURE10);
	::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement_MaxMin);
	::glBindSampler(11, m_hSampler_ClampLinear);
	::glActiveTexture(GL_TEXTURE11);
	::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement);
	::glBindSampler(13, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE13);
	::glBindTexture(GL_TEXTURE_2D_ARRAY, hTexture_Terrain_Albedo);

	//IA
	::glBindVertexArray(m_hInputLayout_Terrain);
	::glBindVertexBuffer(0U, m_hBufferVertex_Terrain, 0U, sizeof(float) * 2 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Terrain);

	::glPatchParameteriOES(GL_PATCH_VERTICES_OES, 2);
	::glDrawElements(GL_PATCHES_OES, 512, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0));

	//::glPolygonModeNV(GL_FRONT_AND_BACK, GL_FILL_NV);
}

Render_Geometry_Terrain g_Render_Geometry_Terrain;