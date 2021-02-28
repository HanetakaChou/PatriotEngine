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

#include "Water.h"

static char const *const Geometry_Water_GBufferPass_Vertex =
R"(#version 310 es

layout(location = 0) in vec3 IA_Position;
layout(location = 1) in vec3 IA_Tangent;
layout(location = 2) in vec3 IA_Bitangent;
layout(location = 3) in vec2 IA_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_WaterTile
{
	highp mat4x4 g_TangentToWorld;
	highp vec4 g_UVAnimated;
};

layout(location = 5) out vec3 RS_Tangent;
layout(location = 6) out vec3 RS_Bitangent;
layout(location = 7) out vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) out vec2 RS_UV_NormalMap_HighFreq;

void main() 
{
    gl_Position = g_Projection * g_View * g_TangentToWorld * vec4(IA_Position,1.0f);

	RS_Tangent =  mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * IA_Tangent;
	RS_Bitangent =  mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * IA_Bitangent;
	RS_UV_NormalMap_LowFreq = IA_UV + g_UVAnimated.xy;
	RS_UV_NormalMap_HighFreq = IA_UV + g_UVAnimated.zw;
})";

static char const *const Geometry_Water_GBufferPass_Fragment = 
R"(#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(binding = 4) uniform sampler2D g_NormalMap_LowFreq;
layout(binding = 5) uniform sampler2D g_NormalMap_HighFreq;

layout(location = 5) in sample highp vec3 RS_Tangent;
layout(location = 6) in sample highp vec3 RS_Bitangent;
layout(location = 7) in sample highp vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) in sample highp vec2 RS_UV_NormalMap_HighFreq;

layout(location = 0) out highp vec4 SV_Target0; //NormalMacro
layout(location = 1) out highp vec4 SV_Target1; //NormalMeso
layout(location = 2) out highp vec4 SV_Target2; //Albedo
layout(location = 3) out highp vec4 SV_Target3; //Specular

layout (early_fragment_tests) in;

void main() 
{
	highp vec3 NormalMap_LowFreq = texture(g_NormalMap_LowFreq, RS_UV_NormalMap_LowFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalMap_HighFreq = texture(g_NormalMap_HighFreq, RS_UV_NormalMap_HighFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalTangent = normalize(NormalMap_LowFreq + NormalMap_HighFreq);
	//highp vec3 NormalTangent = NormalMap_HighFreq;

	highp vec3 Axis_Tangent = normalize(RS_Tangent);
	highp vec3 Axis_Bitangent = normalize(RS_Bitangent);
	highp vec3 Axis_Normal = cross(Axis_Bitangent, Axis_Tangent);

	highp vec3 NormalMacro = Axis_Normal;
	highp vec3 NormalMeso = mat3x3(Axis_Tangent.xyz, Axis_Bitangent.xyz, Axis_Normal.xyz)*NormalTangent;
	highp vec3 ColorAlbedo = vec3(0.5f,0.5f,0.5f);
    highp vec3 ColorSpecular = vec3(0.2f,0.2f,0.4f);

	SV_Target0 = vec4((NormalMacro+1.0f)*0.5f,1.0f);
	SV_Target1 = vec4((NormalMeso+1.0f)*0.5f,1.0f);
	SV_Target2 = vec4(ColorAlbedo,1.0f);	
	SV_Target3 = vec4(ColorSpecular,1.0f);	
})";

static char const *const Geometry_Water_ForwardPass_Vertex =
R"(#version 310 es

layout(location = 0) in vec3 IA_Position;
layout(location = 1) in vec3 IA_Tangent;
layout(location = 2) in vec3 IA_Bitangent;
layout(location = 3) in vec2 IA_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_WaterTile
{
	highp mat4x4 g_TangentToWorld;
	highp vec4 g_UVAnimated;
};

layout(location = 5) out vec3 RS_Tangent;
layout(location = 6) out vec3 RS_Bitangent;
layout(location = 7) out vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) out vec2 RS_UV_NormalMap_HighFreq;

layout(location = 10) out vec3 RS_PositionWorld;

void main() 
{
	vec4 Position_World = g_TangentToWorld * vec4(IA_Position,1.0f);

	RS_PositionWorld = Position_World.xyz;

    gl_Position = g_Projection * g_View * Position_World;

	RS_Tangent =  mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * IA_Tangent;
	
	RS_Bitangent =  mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * IA_Bitangent;
	
	RS_UV_NormalMap_LowFreq = IA_UV + g_UVAnimated.xy;
	
	RS_UV_NormalMap_HighFreq = IA_UV + g_UVAnimated.zw;
})";

static char const *const Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment =
R"(#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(binding = 4) uniform sampler2D g_NormalMap_LowFreq;
layout(binding = 5) uniform sampler2D g_NormalMap_HighFreq;

layout(location = 5) in sample highp vec3 RS_Tangent;
layout(location = 6) in sample highp vec3 RS_Bitangent;
layout(location = 7) in sample highp vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) in sample highp vec2 RS_UV_NormalMap_HighFreq;

layout(location = 10) in sample highp vec3 RS_PositionWorld;

layout(binding = 12) uniform highp sampler2D g_Texture_GBufferDepth;
layout(binding = 17) uniform highp sampler2D g_Texture_RadianceDirect;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 7) uniform g_BufferConstant_Light
{
	highp mat4x4 g_LightView;
	highp mat4x4 g_LightProjection;
	highp vec4 g_LightIrradiance;
	highp vec4 g_LightDirection;
};

layout(location = 0) out highp vec4 OM_RadianceTotal;

layout (early_fragment_tests) in;

void main() 
{
	highp vec3 NormalMap_LowFreq = texture(g_NormalMap_LowFreq, RS_UV_NormalMap_LowFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalMap_HighFreq = texture(g_NormalMap_HighFreq, RS_UV_NormalMap_HighFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalTangent = normalize(NormalMap_LowFreq + NormalMap_HighFreq);

	highp vec3 Axis_Tangent = normalize(RS_Tangent);
	highp vec3 Axis_Bitangent = normalize(RS_Bitangent);
	highp vec3 Axis_Normal = cross(Axis_Bitangent, Axis_Tangent);

	//Material
	highp vec3 NormalMacro = Axis_Normal;
	highp vec3 NormalMeso = mat3x3(Axis_Tangent.xyz, Axis_Bitangent.xyz, Axis_Normal.xyz)*NormalTangent;
	highp vec3 ColorAlbedo = vec3(0.5f,0.5f,0.5f);
    highp vec3 ColorSpecular = vec3(0.2f,0.2f,0.4f);
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

	//Forward
	//Light_ScreenSpaceReflection
	do{
		highp vec3 Position_World = RS_PositionWorld;
		highp vec3 ReflectionDirection = NormalMacro * dot(ViewDirection,NormalMacro)*2.0f - ViewDirection;

		//RayTrace
    	highp vec3 Position_RayFrom_FrameBuffer = vec3(gl_FragCoord.xyz);

		highp vec3 Position_RayTo_FrameBuffer;
    	{
        	highp vec3 Position_RayTo_World = Position_World + ReflectionDirection;

        	highp vec4 Position_RayTo_View = g_View * vec4(Position_RayTo_World, 1.0f);
                       
        	//Clip to NearZ 
        	if(Position_RayTo_View.z > -0.1f) //����ϵ
			{
				break; //Alpha Blend ADD
			}

        	highp vec4 Position_RayTo_NormalizedDevice = g_Projection * Position_RayTo_View;
        	Position_RayTo_NormalizedDevice.xyzw /= Position_RayTo_NormalizedDevice.wwww;
            
        	Position_RayTo_FrameBuffer = vec3(((Position_RayTo_NormalizedDevice.x+1.0f)*0.5f)*800.0f, ((Position_RayTo_NormalizedDevice.y+1.0f)*0.5f)*600.0f, Position_RayTo_NormalizedDevice.z);//Viewport
    	}

		//Permute Axis
    	bool bLongX = abs(Position_RayTo_FrameBuffer.x - Position_RayFrom_FrameBuffer.x) > abs(Position_RayTo_FrameBuffer.y - Position_RayFrom_FrameBuffer.y);
    	highp float LongFrom = bLongX ? Position_RayFrom_FrameBuffer.x : Position_RayFrom_FrameBuffer.y;
    	highp float LongTo = bLongX ? Position_RayTo_FrameBuffer.x : Position_RayTo_FrameBuffer.y;
    	highp float ShortFrom = bLongX ? Position_RayFrom_FrameBuffer.y : Position_RayFrom_FrameBuffer.x;
    	highp float ShortTo = bLongX ? Position_RayTo_FrameBuffer.y : Position_RayTo_FrameBuffer.x;

		highp float DeltaLong;
    	highp float DeltaShort;
    	highp float DeltaDepth;
		{
        	DeltaLong = sign(LongTo - LongFrom);
        	//Line Degenerate
	    	highp float AbsLong = abs(LongTo - LongFrom);
        	if(AbsLong <= 1.0f)
        	{
            	break; //Alpha Blend ADD
        	}
        	highp float InverseAbsLong = 1.0f / AbsLong;
        	DeltaShort = (ShortTo - ShortFrom) * InverseAbsLong;
	    	DeltaDepth = (Position_RayTo_FrameBuffer.z - Position_RayFrom_FrameBuffer.z) * InverseAbsLong; //����Ҫ͸��У����ֵ
		}
		//Stride
		DeltaLong*=4.0f;
		DeltaShort*=4.0f;
		DeltaDepth*=4.0f;
	
    	//1/2 Pixel
    	highp float LongCurrent = LongFrom + DeltaLong*0.5f;
    	highp float ShortCurrent = ShortFrom + DeltaShort*0.5f;
    	highp float DepthCurrent = Position_RayFrom_FrameBuffer.z + DeltaDepth*0.5f;
    	highp vec2 TexelCoord = bLongX ? vec2(LongCurrent, ShortCurrent) : vec2(ShortCurrent, LongCurrent);

		highp vec3 LightRadiance = vec3(0.0f,0.0f,0.0f);

    	for(int i_Step = 0; i_Step < 100; ++i_Step)
    	{
        	LongCurrent += DeltaLong;
        	ShortCurrent += DeltaShort;
        	DepthCurrent += DeltaDepth;
        	TexelCoord = bLongX ? vec2(LongCurrent, ShortCurrent) : vec2(ShortCurrent, LongCurrent);

        	if(TexelCoord.x <= 0.0f || TexelCoord.x >= 799.0f || TexelCoord.y <= 0.0f || TexelCoord.y >= 599.0f)
        	{
            	//LightRadiance = vec3(0.0f,0.0f,0.0f);
            	//LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(TexelCoord.xy), 0).xyz;
				break;
        	}

			highp float DepthScene = texelFetch(g_Texture_GBufferDepth, ivec2(TexelCoord.xy), 0).x;

        	if(DepthCurrent < DepthScene) //&& DepthScene > 0.00001f) //��SkyBox֮ǰ���Ƽ��� //Reverse-Z //RayCastTerrain 
        	{
				LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(TexelCoord.xy), 0).xyz;
            	break;
        	}
    	}

		//Environment Map
    	//highp vec3 Fresnel = ColorSpecular;
    	ViewRadiance +=  LightRadiance;//Fresnel*LightRadiance;
	}while(false);

	//Forward
	//Light_Refraction
	{
		highp vec3 LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(gl_FragCoord.xy+NormalMeso.xy*vec2(800.0f,600.0f)*0.025f), 0).xyz;//vScale:0.05f
		//highp vec3 Transmittance = ColorAlbedo;
		ViewRadiance += LightRadiance;//Transmittance*LightRadiance;
	}

	OM_RadianceTotal = vec4(ViewRadiance,1.0f);
})";

static char const *const Geometry_Water_ForwardPass_PlanarReflection_Fragment = 
R"(#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(binding = 4) uniform sampler2D g_NormalMap_LowFreq;
layout(binding = 5) uniform sampler2D g_NormalMap_HighFreq;

layout(location = 5) in sample highp vec3 RS_Tangent;
layout(location = 6) in sample highp vec3 RS_Bitangent;
layout(location = 7) in sample highp vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) in sample highp vec2 RS_UV_NormalMap_HighFreq;

layout(location = 10) in sample highp vec3 RS_PositionWorld;

layout(binding = 16) uniform highp sampler2D g_Texture_PlanarReflection;
layout(binding = 17) uniform highp sampler2D g_Texture_RadianceDirect;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 7) uniform g_BufferConstant_Light
{
	highp mat4x4 g_LightView;
	highp mat4x4 g_LightProjection;
	highp vec4 g_LightIrradiance;
	highp vec4 g_LightDirection;
};

layout(location = 0) out highp vec4 OM_RadianceTotal;

layout (early_fragment_tests) in;

void main() 
{
	highp vec3 NormalMap_LowFreq = texture(g_NormalMap_LowFreq, RS_UV_NormalMap_LowFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalMap_HighFreq = texture(g_NormalMap_HighFreq, RS_UV_NormalMap_HighFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalTangent = normalize(NormalMap_LowFreq + NormalMap_HighFreq);

	highp vec3 Axis_Tangent = normalize(RS_Tangent);
	highp vec3 Axis_Bitangent = normalize(RS_Bitangent);
	highp vec3 Axis_Normal = cross(Axis_Bitangent, Axis_Tangent);

	//Material
	highp vec3 NormalMacro = Axis_Normal;
	highp vec3 NormalMeso = mat3x3(Axis_Tangent.xyz, Axis_Bitangent.xyz, Axis_Normal.xyz)*NormalTangent;
	highp vec3 ColorAlbedo = vec3(0.5f,0.5f,0.5f);
    highp vec3 ColorSpecular = vec3(0.2f,0.2f,0.4f);
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

	//Forward
	//Light_PlanarReflection
	highp vec3 LightRadiance = texelFetch(g_Texture_PlanarReflection, ivec2(800.0f - gl_FragCoord.x, gl_FragCoord.y), 0).xyz;

	//Environment Map
    //highp vec3 Fresnel = ColorSpecular;
    ViewRadiance +=  LightRadiance;//Fresnel*LightRadiance;

	//Forward
	//Light_Refraction
	{
		highp vec3 LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(gl_FragCoord.xy+NormalMeso.xy*vec2(800.0f,600.0f)*0.1f), 0).xyz;//vScale:0.05f
		//highp vec3 Transmittance = ColorAlbedo;
		ViewRadiance += LightRadiance*2.0f;//Transmittance*LightRadiance;
	}

	OM_RadianceTotal = vec4(ViewRadiance,1.0f);
})";

void Render_Geometry_Water::Init()
{
	//m_hBufferVertex_Water
	{
		::glGenBuffers(1, &m_hBufferVertex_Water);
		::glBindBuffer(GL_ARRAY_BUFFER, m_hBufferVertex_Water);
		float BufferVertexData[] =
		{
			128.0f,0.0f,-128.0f,	1.0f,0.0f,0.0f,		0.0f,0.0f,1.0f,		10.0f,10.0f,
			-128.0f,0.0f,-128.0f,	1.0f,0.0f,0.0f,		0.0f,0.0f,1.0f,		0.0f,10.0f,
			128.0f,0.0f,128.0f,		1.0f,0.0f,0.0f,		0.0f,0.0f,1.0f,		10.0f,0.0f,
			-128.0f,0.0f,128.0f,	1.0f,0.0f,0.0f,		0.0f,0.0f,1.0f,		0.0f,0.0f
		};
		::glBufferData(GL_ARRAY_BUFFER, sizeof(BufferVertexData), BufferVertexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ARRAY_BUFFER, 0U);
	}

	//m_hBufferIndex_Water
	{
		::glGenBuffers(1, &m_hBufferIndex_Water);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Water);
		uint16_t BufferIndexData[] =
		{
			0,1,2,3
		};
		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BufferIndexData), BufferIndexData, GL_STATIC_COPY);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
	}

	//m_hInputLayout_Water
	{
		::glGenVertexArrays(1, &m_hInputLayout_Water);
		::glBindVertexArray(m_hInputLayout_Water);
		::glEnableVertexAttribArray(0);
		::glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0U);
		::glVertexAttribBinding(0U, 0U);
		::glEnableVertexAttribArray(1);
		::glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
		::glVertexAttribBinding(1U, 0U);
		::glEnableVertexAttribArray(2);
		::glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3 + sizeof(float) * 3);
		::glVertexAttribBinding(2U, 0U);
		::glEnableVertexAttribArray(3);
		::glVertexAttribFormat(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 3);
		::glVertexAttribBinding(3U, 0U);
		::glBindVertexArray(0U);
	}

	GLuint hShader_Geometry_Water_GBufferPass_Vertex;
	{
		hShader_Geometry_Water_GBufferPass_Vertex = ::glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_Geometry_Water_GBufferPass_Vertex, 1U, &::Geometry_Water_GBufferPass_Vertex, NULL);
		::glCompileShader(hShader_Geometry_Water_GBufferPass_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Water_GBufferPass_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Water_GBufferPass_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Water_GBufferPass_Fragment;
	{
		hShader_Geometry_Water_GBufferPass_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Water_GBufferPass_Fragment, 1U, &Geometry_Water_GBufferPass_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Water_GBufferPass_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Water_GBufferPass_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Water_GBufferPass_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	//m_hProgram_Geometry_Water_GBufferPass
	{
		m_hProgram_Geometry_Water_GBufferPass = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Water_GBufferPass, hShader_Geometry_Water_GBufferPass_Vertex);
		::glAttachShader(m_hProgram_Geometry_Water_GBufferPass, hShader_Geometry_Water_GBufferPass_Fragment);

		::glLinkProgram(m_hProgram_Geometry_Water_GBufferPass);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Water_GBufferPass, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Water_GBufferPass, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	GLuint hShader_Geometry_Water_ForwardPass_Vertex;
	{
		hShader_Geometry_Water_ForwardPass_Vertex = ::glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_Geometry_Water_ForwardPass_Vertex, 1U, &::Geometry_Water_ForwardPass_Vertex, NULL);
		::glCompileShader(hShader_Geometry_Water_ForwardPass_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Water_ForwardPass_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Water_ForwardPass_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment;
	{
		hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment, 1U, &Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment;
	{
		hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment, 1U, &Geometry_Water_ForwardPass_PlanarReflection_Fragment, NULL);
		::glCompileShader(hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	//m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection
	{
		m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection, hShader_Geometry_Water_ForwardPass_Vertex);
		::glAttachShader(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection, hShader_Geometry_Water_ForwardPass_ScreenSpaceReflection_Fragment);

		::glLinkProgram(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//m_hProgram_Geometry_Water_ForwardPass_PlanarReflection
	{
		m_hProgram_Geometry_Water_ForwardPass_PlanarReflection = ::glCreateProgram();
		::glAttachShader(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection, hShader_Geometry_Water_ForwardPass_Vertex);
		::glAttachShader(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection, hShader_Geometry_Water_ForwardPass_PlanarReflection_Fragment);

		::glLinkProgram(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//m_hBufferConstant_Water
	{
		::glGenBuffers(1, &m_hBufferConstant_WaterTile);
		::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_WaterTile);
		::glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferConstant_WaterTileData), NULL, GL_STATIC_COPY);
		::glBindBuffer(GL_UNIFORM_BUFFER, 0U);
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

	//m_hTexture_NormalMap_HighFreq
	m_hTexture_NormalMap_HighFreq = ::glexCreateDDSTextureFromFile("NormalMap_HighFreq.dds");

	//m_hTexture_NormalMap_LowFreq
	m_hTexture_NormalMap_LowFreq = ::glexCreateDDSTextureFromFile("NormalMap_LowFreq.dds");
}

void Render_Geometry_Water::Execute_GBufferPass(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData)
{
	//Update
	::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_WaterTile);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferConstant_WaterTileData), pBufferConstant_WaterData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

	//RS
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Water_GBufferPass);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_hBufferConstant_WaterTile);
	//SRV
	::glBindSampler(4, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE4);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_HighFreq);
	::glBindSampler(5, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE5);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_LowFreq);
	
	//IA
	::glBindVertexArray(m_hInputLayout_Water);
	::glBindVertexBuffer(0U, m_hBufferVertex_Water, 0U, sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Water);

	::glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0));
}

void Render_Geometry_Water::Execute_ForwardPass_ScreenSpaceReflection(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData, GLuint hBufferConstant_Light)
{
	//Update
	::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_WaterTile);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferConstant_WaterTileData), pBufferConstant_WaterData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

	//RS
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Water_ForwardPass_ScreenSpaceReflection);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_hBufferConstant_WaterTile);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 7, hBufferConstant_Light);
	//SRV
	::glBindSampler(4, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE4);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_HighFreq);
	::glBindSampler(5, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE5);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_LowFreq);

	//IA
	::glBindVertexArray(m_hInputLayout_Water);
	::glBindVertexBuffer(0U, m_hBufferVertex_Water, 0U, sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Water);

	::glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0));
}

void Render_Geometry_Water::Execute_ForwardPass_PlanarReflection(GLuint hBufferConstant_Sensor, BufferConstant_WaterTileData *pBufferConstant_WaterData, GLuint hBufferConstant_Light)
{
	//Update
	::glBindBuffer(GL_UNIFORM_BUFFER, m_hBufferConstant_WaterTile);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferConstant_WaterTileData), pBufferConstant_WaterData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

	//RS
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_Geometry_Water_ForwardPass_PlanarReflection);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_hBufferConstant_WaterTile);
	::glBindBufferBase(GL_UNIFORM_BUFFER, 7, hBufferConstant_Light);
	//SRV
	::glBindSampler(4, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE4);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_HighFreq);
	::glBindSampler(5, m_hSampler_RepeatLinear);
	::glActiveTexture(GL_TEXTURE5);
	::glBindTexture(GL_TEXTURE_2D, m_hTexture_NormalMap_LowFreq);

	//IA
	::glBindVertexArray(m_hInputLayout_Water);
	::glBindVertexBuffer(0U, m_hBufferVertex_Water, 0U, sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);

	//::glPrimitiveRestartIndex(0XFFFFU);
	::glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hBufferIndex_Water);

	::glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, reinterpret_cast<void const *>(sizeof(uint16_t) * 0));
}

Render_Geometry_Water g_Render_Geometry_Water;