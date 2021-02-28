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

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../libGL/include/EGL/egl.h"
#include "../libGL/include/EGL/eglext.h"
#include "../libGL/include/GLES3/gl31.h"
#define GL_GLEXT_PROTOTYPES
#include "../libGL/include/GLES3/gl2ext.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <vector>

#define PTWIN32
#define PTX86
#include "D:\OSChina\PatriotEngine\Runtime\Public\Math\PTMath.h"
#include <dxgi.h>
PTVector3F volatile g_EyePosition = { 0.0f,0.0f, 5.0f };
PTVector3F volatile g_EyeDirection = { 0.0f,-1.0f,-1.0f };
PTVector3F volatile g_UpDirection = { 0.0f,1.0f,0.0f };

#include "../libGL/include/glex.h"

#include "Grass.h"
#include "Terrain.h"
#include "Water.h"
#include "SkyBox.h"

static char const *const FullScreenTriangle_Vertex =
R"(#version 310 es
#extension GL_EXT_shader_io_blocks : enable
#extension GL_OES_shader_io_blocks : enable

out gl_PerVertex {
    vec4 gl_Position;
};
layout(location = 7) out vec2 l_Screen_UV;

void main() 
{
	switch(gl_VertexID)
	{
	case 0:
		gl_Position = vec4(-1.0f,-1.0f,0.0f,1.0f);
		l_Screen_UV = vec2(0.0f,0.0f);
	break;
	case 1:
		gl_Position = vec4(3.0f,-1.0f,0.0f,1.0f);
		l_Screen_UV = vec2(2.0f,0.0f);
	break;
	case 2:
		gl_Position = vec4(-1.0f,3.0f,0.0f,1.0f);
		l_Screen_UV = vec2(0.0f,2.0f);
	break;
	}
})";

static char const *const Light_Directional_Fragment =
R"(#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
};

layout(binding = 12) uniform highp sampler2DMS g_Texture_GBufferDepth;
layout(binding = 13) uniform highp sampler2DMS g_Texture_GBufferNormalMacro;
layout(binding = 14) uniform highp sampler2DMS g_Texture_GBufferNormalMeso;
layout(binding = 15) uniform highp sampler2DMS g_Texture_GBufferAlbedo;
layout(binding = 16) uniform highp sampler2DMS g_Texture_GBufferSpecular;

layout(binding = 5) uniform sampler2D g_Texture_Shadow;

layout(row_major, binding = 7) uniform g_BufferConstant_Light
{
	highp mat4x4 g_LightView;
	highp mat4x4 g_LightProjection;
	highp vec4 g_LightIrradiance;
	highp vec4 g_LightDirection;
};

layout(location = 0) out highp vec4 SV_Target0;

layout (early_fragment_tests) in;

void main() 
{
#if 0
	//Shadow
	highp vec3 PositionLightAverage = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < 4; ++i)
	{
		//GBuffer
		highp vec4 Position_NormalizedDevice;
		Position_NormalizedDevice.x = l_Screen_UV.x*2.0f - 1.0f;
		Position_NormalizedDevice.y = l_Screen_UV.y*2.0f - 1.0f;
		Position_NormalizedDevice.z = texelFetch(g_TexDepth, ivec2(gl_FragCoord.xy), i).x;
		Position_NormalizedDevice.w = 1.0f;

		highp vec4 Position_View = g_InvProjection * Position_NormalizedDevice;
		Position_View.xyzw /= Position_View.wwww;

		highp vec4 Position_World = g_InvView * Position_View;

		highp vec4 PositionLight = g_LightProjection * g_LightView * vec4(Position_World.xyz, 1.0f);
		//PositionLight.xyzw /= PositionLight.xyzw; //PositionLight.w == 1.0f 正交投影
		PositionLight.x = 0.5f + PositionLight.x * 0.5f;
		PositionLight.y = 0.5f + PositionLight.y * 0.5f;

		PositionLightAverage += PositionLight.xyz;
	}

	PositionLightAverage *= 0.25f;

	highp float OccluderDepth = texture(g_TexShadow, PositionLightAverage.xy).x;
	highp float SurfaceDepth = PositionLightAverage.z;
	highp float ShadowMask = (SurfaceDepth >= (OccluderDepth - 0.01f)) ? 1.0f : 0.0f;
#endif

	//Light
	highp vec3 ViewRadiance = vec3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < 4; ++i)
	{
		//GBuffer
		highp vec3 ViewDirection;
		{
            highp vec4 Position_NormalizedDevice;
            Position_NormalizedDevice.x = l_Screen_UV.x*2.0f - 1.0f;
            Position_NormalizedDevice.y = l_Screen_UV.y*2.0f - 1.0f;
            Position_NormalizedDevice.z = texelFetch(g_Texture_GBufferDepth, ivec2(gl_FragCoord.xy), i).x;
            Position_NormalizedDevice.w = 1.0f;
            
            highp vec4 Position_View = g_InvProjection * Position_NormalizedDevice;
            Position_View.xyzw /= Position_View.wwww;
            
            highp vec3 ViewDirection_View = normalize(Position_View.xyz); //- Vec3(0.0f,0.0f,0.0f)

            highp vec3 ViewDirection_World = mat3x3(g_InvView[0].xyz,g_InvView[1].xyz,g_InvView[2].xyz) * ViewDirection_View;
            ViewDirection = -ViewDirection_World;
		}
		
		highp vec3 NormalMeso = (texelFetch(g_Texture_GBufferNormalMeso, ivec2(gl_FragCoord.xy), i).xyz)*2.0f-1.0f;
		highp vec3 ColorAlbedo = texelFetch(g_Texture_GBufferAlbedo, ivec2(gl_FragCoord.xy), i).xyz;
		highp vec3 ColorSpecular = texelFetch(g_Texture_GBufferSpecular, ivec2(gl_FragCoord.xy), i).xyz;
        highp float Glossiness = 1.0f; //应当从GBuffer中获取

		highp vec3 LightDirection = -normalize(g_LightDirection.xyz);

        //E_N
        highp vec3 IrradianceNormal = g_LightIrradiance.xyz*max(0.0f, dot(NormalMeso, LightDirection));

        //Normalized Blinn-Phong
        highp vec3 HalfVector = ViewDirection + LightDirection; //即NormalMicro

        highp float Smoothness = Glossiness * 5.0f;//与Glossiness正相关 

        highp float NormalDistribution =  ((Smoothness + 2.0f)/6.28f) * pow(max(0.0f, dot(HalfVector,NormalMeso)),Smoothness);

        highp vec3 Fresnel = ColorSpecular;

        highp vec3 BRDF_Specular = NormalDistribution * Fresnel * 0.25f; //G(l,v) = max(0,cos<LightDirection,NormalMeso>)*max(0,cos<ViewDirection,NormalMeso>)

        //Lambert
        highp vec3 ColorDiffuse = ColorAlbedo * (vec3(1.0f,1.0f,1.0f) - Fresnel);

        highp vec3 BRDF_Diffuse = ColorDiffuse / 3.14f;

        //L_V
        ViewRadiance += (BRDF_Specular + BRDF_Diffuse) * IrradianceNormal;
        
        //vec3(ShadowMask, ShadowMask, ShadowMask);
    }

	ViewRadiance *= 0.25f;

	SV_Target0 = vec4(ViewRadiance,1.0f);
})";

static char const *const Light_ScreenSpaceReflection_Fragment = 
R"(#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
};

layout(binding = 12) uniform highp sampler2D g_Texture_GBufferDepth;
layout(binding = 13) uniform highp sampler2D g_Texture_GBufferNormalMacro;
layout(binding = 14) uniform highp sampler2D g_Texture_GBufferNormalMeso; //NormalMap会导致RayTrace无法得到正确的结果
layout(binding = 15) uniform highp sampler2D g_Texture_GBufferAlbedo;
layout(binding = 16) uniform highp sampler2D g_Texture_GBufferSpecular;
layout(binding = 17) uniform highp sampler2D g_Texture_RadianceDirect;

layout(location = 0) out highp vec4 SV_Target0;

layout (early_fragment_tests) in;

void main() 
{
    //GBuffer
    highp vec3 GBuffer_Specular = texelFetch(g_Texture_GBufferSpecular, ivec2(gl_FragCoord.xy), 0).xyz;
    highp vec3 GBuffer_NormalMacro = (texelFetch(g_Texture_GBufferNormalMacro, ivec2(gl_FragCoord.xy), 0).xyz)*2.0f-1.0f;
    highp float GBuffer_Depth = texelFetch(g_Texture_GBufferDepth, ivec2(gl_FragCoord.xy), 0).x;

	//SpecularFilter
    if(dot(GBuffer_Specular, vec3(0.2126f,0.7152f,0.0722f)) < 0.2f)
    {
        discard; //Alpha Blend ADD
    }

    //ReflectionDirection
    highp vec3 Position_World;
    {
        highp vec4 Position_NormalizedDevice;
        Position_NormalizedDevice.x = l_Screen_UV.x*2.0f - 1.0f;
        Position_NormalizedDevice.y = l_Screen_UV.y*2.0f - 1.0f;
        Position_NormalizedDevice.z = GBuffer_Depth;
        Position_NormalizedDevice.w = 1.0f;
            
        highp vec4 Position_View = g_InvProjection * Position_NormalizedDevice;
        Position_View.xyzw /= Position_View.wwww;

        Position_World = (g_InvView * Position_View).xyz;
    }
	
    highp vec3 ViewDirection;
    {
        highp vec3 PositionSensor_World = g_InvView[3].xyz; //g_InvView * vec4(0.0f,0.0f,0.0f,1.0f);

        ViewDirection = normalize(PositionSensor_World - Position_World);
    }

	highp vec3 NormalMacro = GBuffer_NormalMacro;
		
    highp vec3 ReflectionDirection = NormalMacro * dot(ViewDirection,NormalMacro)*2.0f - ViewDirection;

    //RayTrace
    highp vec3 Position_RayFrom_FrameBuffer = vec3(gl_FragCoord.xy, GBuffer_Depth);

    highp vec3 Position_RayTo_FrameBuffer;
    {
        highp vec3 Position_RayTo_World = Position_World + ReflectionDirection;

        highp vec4 Position_RayTo_View = g_View * vec4(Position_RayTo_World, 1.0f);
                       
        //Clip to NearZ 
        if(Position_RayTo_View.z > -0.1f) //右手系
		{
			discard; //Alpha Blend ADD
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
            discard; //Alpha Blend ADD
        }
        highp float InverseAbsLong = 1.0f / AbsLong;
        DeltaShort = (ShortTo - ShortFrom) * InverseAbsLong;
	    DeltaDepth = (Position_RayTo_FrameBuffer.z - Position_RayFrom_FrameBuffer.z) * InverseAbsLong; //不需要透视校正插值
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

        if(DepthCurrent < DepthScene) //&& DepthScene > 0.00001f) //在SkyBox之前绘制即可 //Reverse-Z //RayCastTerrain 
        {
			LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(TexelCoord.xy), 0).xyz;
            break;
        }
    }

	//Environment Map
    highp vec3 Fresnel = GBuffer_Specular;
    highp vec3 RadianceIndirect = Fresnel*LightRadiance;
	SV_Target0 = vec4(RadianceIndirect,1.0f);
})";

static char const *const PostProcess_Fragment =
R"(#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;
	
layout(binding = 17) uniform highp sampler2D g_Texture_SceneReferred;
	
layout(location = 0) out highp vec4 g_Texture_OutputReferred;

void main() 
{
	highp vec3 Radiance_SceneReferred = texelFetch(g_Texture_SceneReferred, ivec2(gl_FragCoord.xy), 0).xyz;
	highp vec3 Radiance_OutputReferred;
	Radiance_OutputReferred.x = pow(Radiance_SceneReferred.x, 0.5f);
	Radiance_OutputReferred.y = pow(Radiance_SceneReferred.y, 0.5f);
	Radiance_OutputReferred.z = pow(Radiance_SceneReferred.z, 0.5f);
	g_Texture_OutputReferred = vec4(Radiance_OutputReferred, 1.0f);
})";

#include <float.h>

unsigned __stdcall RenderMain(void *pVoid)
{
	HWND hWnd = static_cast<HWND>(pVoid);
	HDC hDC = ::GetDC(hWnd);

	EGLDisplay hDisplay = ::eglGetDisplay(hDC);

	EGLBoolean eglbResult;

	EGLint majorVersion;
	EGLint minorVersion;
	eglbResult = ::eglInitialize(hDisplay, &majorVersion, &minorVersion);
	assert(eglbResult == EGL_TRUE);

	const EGLint configAttributes[] =
	{
		EGL_RED_SIZE, 10,
		EGL_GREEN_SIZE, 10,
		EGL_BLUE_SIZE, 10,
		//EGL_ALPHA_SIZE, 2,
		//EGL_DEPTH_SIZE, 0,
		//EGL_STENCIL_SIZE, 0,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};
	EGLConfig hConfig;
	EGLint num_config;
	eglbResult = ::eglChooseConfig(hDisplay, configAttributes, &hConfig, 1, &num_config);
	assert(eglbResult == EGL_TRUE);

	EGLint attrib_list[] = {
		EGL_CONTEXT_MAJOR_VERSION_KHR,3,
		EGL_CONTEXT_MINOR_VERSION_KHR,1,
		EGL_CONTEXT_FLAGS_KHR,EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR,
		EGL_NONE
	};
	EGLContext ctx = ::eglCreateContext(hDisplay, hConfig, NULL, attrib_list);

	const EGLint surfaceAttributes[] =
	{
		EGL_NONE
	};
	EGLSurface hSurface = ::eglCreateWindowSurface(hDisplay, hConfig, hWnd, &surfaceAttributes[0]);

	eglbResult = ::eglMakeCurrent(hDisplay, hSurface, hSurface, ctx);
	assert(eglbResult == EGL_TRUE);

	::glDebugMessageCallbackKHR(
		[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)->void
	{
		char *str_source;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API_KHR: str_source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR: str_source = "Window_System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR: str_source = "Shader_Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_KHR: str_source = "ThirdParty"; break;
		case GL_DEBUG_SOURCE_APPLICATION_KHR: str_source = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER_KHR: str_source = "Other"; break;
		default: str_source = "Unknown";
		}

		char *str_type;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR_KHR: str_type = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR: str_type = "Deprecated_Behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR: str_type = "Undefined_Behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY_KHR: str_type = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE_KHR: str_type = "Peformance"; break;
		case GL_DEBUG_TYPE_MARKER_KHR: str_type = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP_KHR: str_type = "Push_Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP_KHR: str_type = "Pop_Group"; break;
		case GL_DEBUG_TYPE_OTHER_KHR: str_type = "Other"; break;
		default: str_type = "Unknown";
		}

		char *str_severity;
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH_KHR: str_severity = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM_KHR: str_severity = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW_KHR: str_severity = "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION_KHR: str_severity = "Notification"; break;
		default: str_severity = "Unknown";
		}

		char OutputString[4096];
		::sprintf_s(OutputString,"OpenGL Debug Message \n Source_%s \n Type_%s \n ID 0X%08X \n Severity_%s\n %s \n",str_source, str_type, id, str_severity, message);
		::OutputDebugStringA(OutputString);
	},
		NULL);

	::glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0U, NULL, GL_TRUE);
	::glDebugMessageControlKHR(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER_KHR, GL_DONT_CARE, 0U, NULL, GL_FALSE);

	GLuint hShader_FullScreenTriangle_Vertex;
	{
		hShader_FullScreenTriangle_Vertex = glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_FullScreenTriangle_Vertex, 1U, &::FullScreenTriangle_Vertex, NULL);
		::glCompileShader(hShader_FullScreenTriangle_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_FullScreenTriangle_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_FullScreenTriangle_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hProgram_Light_Directional;
	{
		GLuint hShader_Light_Directional_Fragment;
		{
			hShader_Light_Directional_Fragment = glCreateShader(GL_FRAGMENT_SHADER);
			::glShaderSource(hShader_Light_Directional_Fragment, 1U, &::Light_Directional_Fragment, NULL);
			::glCompileShader(hShader_Light_Directional_Fragment);

			GLint CompileStatus;
			::glGetShaderiv(hShader_Light_Directional_Fragment, GL_COMPILE_STATUS, &CompileStatus);
			assert(CompileStatus == GL_TRUE);
			if (CompileStatus != GL_TRUE)
			{
				char ShadeInfoLog[4096];
				::glGetShaderInfoLog(hShader_Light_Directional_Fragment, 4096, NULL, ShadeInfoLog);
				::OutputDebugStringA(ShadeInfoLog);
			}
		}

		hProgram_Light_Directional = glCreateProgram();
		::glAttachShader(hProgram_Light_Directional, hShader_FullScreenTriangle_Vertex);
		::glAttachShader(hProgram_Light_Directional, hShader_Light_Directional_Fragment);

		::glLinkProgram(hProgram_Light_Directional);
		GLint LinkStatus;
		::glGetProgramiv(hProgram_Light_Directional, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(hProgram_Light_Directional, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	GLuint hProgram_Light_ScreenSpaceReflection_Fragment;
	{
		GLuint hShader_Light_ScreenSpaceReflection_Fragment;
		{
			hShader_Light_ScreenSpaceReflection_Fragment = glCreateShader(GL_FRAGMENT_SHADER);
			::glShaderSource(hShader_Light_ScreenSpaceReflection_Fragment, 1U, &::Light_ScreenSpaceReflection_Fragment, NULL);
			::glCompileShader(hShader_Light_ScreenSpaceReflection_Fragment);

			GLint CompileStatus;
			::glGetShaderiv(hShader_Light_ScreenSpaceReflection_Fragment, GL_COMPILE_STATUS, &CompileStatus);
			assert(CompileStatus == GL_TRUE);
			if (CompileStatus != GL_TRUE)
			{
				char ShadeInfoLog[4096];
				::glGetShaderInfoLog(hShader_Light_ScreenSpaceReflection_Fragment, 4096, NULL, ShadeInfoLog);
				::OutputDebugStringA(ShadeInfoLog);
			}
		}

		hProgram_Light_ScreenSpaceReflection_Fragment = glCreateProgram();
		::glAttachShader(hProgram_Light_ScreenSpaceReflection_Fragment, hShader_FullScreenTriangle_Vertex);
		::glAttachShader(hProgram_Light_ScreenSpaceReflection_Fragment, hShader_Light_ScreenSpaceReflection_Fragment);

		::glLinkProgram(hProgram_Light_ScreenSpaceReflection_Fragment);
		GLint LinkStatus;
		::glGetProgramiv(hProgram_Light_ScreenSpaceReflection_Fragment, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(hProgram_Light_ScreenSpaceReflection_Fragment, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	GLuint hProgram_PostProcess;
	{
		GLuint hShader_PostProcess_Fragment;
		{
			hShader_PostProcess_Fragment = glCreateShader(GL_FRAGMENT_SHADER);
			::glShaderSource(hShader_PostProcess_Fragment, 1U, &::PostProcess_Fragment, NULL);
			::glCompileShader(hShader_PostProcess_Fragment);

			GLint CompileStatus;
			::glGetShaderiv(hShader_PostProcess_Fragment, GL_COMPILE_STATUS, &CompileStatus);
			assert(CompileStatus == GL_TRUE);
			if (CompileStatus != GL_TRUE)
			{
				char ShadeInfoLog[4096];
				::glGetShaderInfoLog(hShader_PostProcess_Fragment, 4096, NULL, ShadeInfoLog);
				::OutputDebugStringA(ShadeInfoLog);
			}
		}

		hProgram_PostProcess = glCreateProgram();
		::glAttachShader(hProgram_PostProcess, hShader_FullScreenTriangle_Vertex);
		::glAttachShader(hProgram_PostProcess, hShader_PostProcess_Fragment);

		::glLinkProgram(hProgram_PostProcess);
		GLint LinkStatus;
		::glGetProgramiv(hProgram_PostProcess, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(hProgram_PostProcess, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	//Sampler
	GLuint hSampler_Linear;
	{
		::glGenSamplers(1, &hSampler_Linear);
		::glSamplerParameteri(hSampler_Linear, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Linear, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Linear, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Linear, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glSamplerParameteri(hSampler_Linear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	GLuint hSampler_Point;
	{
		::glGenSamplers(1, &hSampler_Point);
		::glSamplerParameteri(hSampler_Point, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Point, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Point, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glSamplerParameteri(hSampler_Point, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		::glSamplerParameteri(hSampler_Point, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	//RenderTarget
	GLuint hRenderTargetTexture_ShadowPass_Depth;
	{
		::glGenTextures(1, &hRenderTargetTexture_ShadowPass_Depth);
		::glActiveTexture(GL_TEXTURE1);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_ShadowPass_Depth);
		::glActiveTexture(GL_TEXTURE1);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1024, 1024);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTarget_ShadowPass;
	{
		::glGenFramebuffers(1, &hRenderTarget_ShadowPass);
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_ShadowPass);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hRenderTargetTexture_ShadowPass_Depth, 0);
		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	GLuint hRenderTargetTexture_PlanarReflectionPass_Radiance;
	{
		::glGenTextures(1, &hRenderTargetTexture_PlanarReflectionPass_Radiance);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_PlanarReflectionPass_Radiance);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_PlanarReflectionPass_Depth;
	{
		::glGenTextures(1, &hRenderTargetTexture_PlanarReflectionPass_Depth);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_PlanarReflectionPass_Depth);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTarget_PlanarReflectionPass;
	{
		::glGenFramebuffers(1, &hRenderTarget_PlanarReflectionPass);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_PlanarReflectionPass);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hRenderTargetTexture_PlanarReflectionPass_Radiance, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hRenderTargetTexture_PlanarReflectionPass_Depth, 0);

		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	GLuint hRenderTargetTexture_GBufferPass_NormalMacro;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_NormalMacro);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMacro);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 800, 600, GL_FALSE);//OpenGL ES 不支持 GL_RGBA8_SNORM //非Color Renderable
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_NormalMeso;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_NormalMeso);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMeso);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 800, 600, GL_FALSE);//OpenGL ES 不支持 GL_RGBA8_SNORM //非Color Renderable
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Albedo;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Albedo);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Albedo);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 800, 600, GL_FALSE);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Specular;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Specular);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Specular);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 800, 600, GL_FALSE);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Depth;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Depth);
		::glActiveTexture(GL_TEXTURE1);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Depth);
		::glActiveTexture(GL_TEXTURE1);
		::glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32F, 800, 600, GL_FALSE);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	GLuint hRenderTarget_GBufferPass;
	{
		::glGenFramebuffers(1, &hRenderTarget_GBufferPass);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_GBufferPass);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMacro, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMeso, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Albedo, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Specular, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Depth, 0);

		GLenum RenderTarget[4] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
		::glDrawBuffers(4, RenderTarget);

		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	GLuint hRenderTarget_RadianceTotal;
	{
		::glGenTextures(1, &hRenderTarget_RadianceTotal);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceTotal);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTarget_LightPass;
	{
		::glGenFramebuffers(1, &hRenderTarget_LightPass);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_LightPass);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hRenderTarget_RadianceTotal, 0);

		GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
		::glDrawBuffers(1, bufs);

		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	GLuint hRenderTargetTexture_GBufferPass_NormalMacro_Resolved;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_NormalMacro_Resolved);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMacro_Resolved);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);//OpenGL ES 不支持 GL_RGBA8_SNORM //非Color Renderable
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_NormalMeso_Resolved;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_NormalMeso_Resolved);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMeso_Resolved);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);//OpenGL ES 不支持 GL_RGBA8_SNORM //非Color Renderable
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Albedo_Resolved;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Albedo_Resolved);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Albedo_Resolved);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Specular_Resolved;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Specular_Resolved);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Specular_Resolved);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_GBufferPass_Depth_Resolved;
	{
		::glGenTextures(1, &hRenderTargetTexture_GBufferPass_Depth_Resolved);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Depth_Resolved);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTarget_GBufferPass_Resoleved;
	{
		::glGenFramebuffers(1, &hRenderTarget_GBufferPass_Resoleved);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_GBufferPass_Resoleved);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMacro_Resolved, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMeso_Resolved, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Albedo_Resolved, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Specular_Resolved, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Depth_Resolved, 0);

		GLenum RenderTarget[4] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
		::glDrawBuffers(4, RenderTarget);

		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	GLuint hRenderTarget_RadianceDirect;
	{
		::glGenTextures(1, &hRenderTarget_RadianceDirect);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceDirect);
		::glActiveTexture(GL_TEXTURE0);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTargetTexture_ForwardPass_Depth;
	{
		::glGenTextures(1, &hRenderTargetTexture_ForwardPass_Depth);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_ForwardPass_Depth);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 800, 600);
		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint hRenderTarget_ForwardPass;
	{
		::glGenFramebuffers(1, &hRenderTarget_ForwardPass);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_ForwardPass);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hRenderTarget_RadianceTotal, 0);
		::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hRenderTargetTexture_ForwardPass_Depth, 0);

		GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
		::glDrawBuffers(1, bufs);

		assert(::glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		assert(::glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
	}

	//Constant
	struct BufferSensorData
	{
		PTMatrix4x4F View;
		PTMatrix4x4F Projection;
		PTMatrix4x4F InvView;
		PTMatrix4x4F InvProjection;
		PTFrustumF FrustumPlane;
	};
	GLuint hBufferConstant_Sensor;
	{
		::glGenBuffers(1, &hBufferConstant_Sensor);
		::glBindBuffer(GL_UNIFORM_BUFFER, hBufferConstant_Sensor);

		::glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferSensorData), NULL, GL_DYNAMIC_DRAW);
		::glBindBuffer(GL_UNIFORM_BUFFER, 0U);
	}

	struct BufferLightData
	{
		PTMatrix4x4F LightView;
		PTMatrix4x4F LightProjection;
		PTVector4F Irradiance;
		PTVector4F Direction;
	};
	GLuint hBufferConstant_Light;
	{
		glGenBuffers(1, &hBufferConstant_Light);
		glBindBuffer(GL_UNIFORM_BUFFER, hBufferConstant_Light);
		::glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferLightData), NULL, GL_DYNAMIC_DRAW);
		::glBindBuffer(GL_UNIFORM_BUFFER, 0U);
	}

	BufferSensorData l_BufferSensorData;

	BufferLightData l_LightData;
	{
		PTVector3F EyePosition = { -50.0f,50.0f, 50.0f };
		PTVector3F EyeDirection = { 1.0f,-1.0f,-1.0f };
		PTVector3F UpDirection = { 0.0f,1.0f,0.0f };
		PTMatrix4x4FStore(&l_LightData.LightView, PTMatrix4x4SIMDLookToRH(PTVector3FLoad(&EyePosition), PTVector3FLoad(&EyeDirection), PTVector3FLoad(&UpDirection)));
		PTMatrix4x4FStore(&l_LightData.LightProjection, PTMatrix4x4SIMDOrthographicRH(100.0f, 100.0f, 0.1f, 150.0f));
		l_LightData.Irradiance = PTVector4F{ 1.0f, 1.0f, 1.0f, 7.77777777f };//Irradiance
		l_LightData.Direction = PTVector4F{ 1.0f, -1.0f, -1.0f, 7.77777777f };//Direction
	}
	::glBindBuffer(GL_UNIFORM_BUFFER, hBufferConstant_Light);
	::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferLightData), &l_LightData);
	::glBindBuffer(GL_UNIFORM_BUFFER, 0U);	

	//Scene_Geometry_Terrain_Init
	BufferConstant_TerrainTileData TerrainTileData;
	//Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	TerrainTileData.m_TangentToWorld.r[0].x = 1.0f;
	TerrainTileData.m_TangentToWorld.r[1].x = 0.0f;
	TerrainTileData.m_TangentToWorld.r[2].x = 0.0f;
	TerrainTileData.m_TangentToWorld.r[3].x = 0.0f;
	TerrainTileData.m_TangentToWorld.r[0].y = 0.0f;
	TerrainTileData.m_TangentToWorld.r[1].y = 0.0f;
	TerrainTileData.m_TangentToWorld.r[2].y = -1.0f;
	TerrainTileData.m_TangentToWorld.r[3].y = 0.0f;
	TerrainTileData.m_TangentToWorld.r[0].z = 0.0f;
	TerrainTileData.m_TangentToWorld.r[1].z = 1.0f;
	TerrainTileData.m_TangentToWorld.r[2].z = 0.0f;
	TerrainTileData.m_TangentToWorld.r[3].z = 0.0f;
	TerrainTileData.m_TangentToWorld.r[0].w = 0.0f;
	TerrainTileData.m_TangentToWorld.r[1].w = 0.0f;
	TerrainTileData.m_TangentToWorld.r[2].w = 0.0f;
	TerrainTileData.m_TangentToWorld.r[3].w = 1.0f;

	GLuint hTexture_Terrain_Displacement;
	{
		unsigned short(*pTextureTerrain)[257] = static_cast<unsigned short (*)[257]>(::malloc(sizeof(unsigned short) * 257 * 257));
		unsigned short(*pTextureWater)[257] = static_cast<unsigned short(*)[257]>(::malloc(sizeof(unsigned short) * 257 * 257));
		float (*pTextureHeight)[256] = static_cast<float (*)[256]>(::malloc(sizeof(float) * 256 * 256));

		{
			FILE* pFile = NULL;
			::fopen_s(&pFile, "Terrain.raw", "rb");
			assert(pFile != NULL);

			unsigned short iSize;
			::fread(&iSize, sizeof(unsigned short), 1, pFile);
			assert(iSize == 257U);

			int iSquareSize = 257U * 257U;
			iSquareSize = ::fread(pTextureTerrain, sizeof(unsigned short), iSquareSize, pFile);
			assert(iSquareSize == (257U * 257U));
			::fclose(pFile);
		}

		{
			FILE* pFile = NULL;
			::fopen_s(&pFile, "Water.raw", "rb");
			assert(pFile != NULL);

			unsigned short iSize;
			::fread(&iSize, sizeof(unsigned short), 1, pFile);
			assert(iSize == 257U);

			int iSquareSize = 257U * 257U;
			iSquareSize = ::fread(pTextureWater, sizeof(unsigned short), iSquareSize, pFile);
			assert(iSquareSize == (257U * 257U));
			::fclose(pFile);
		}
		
		for (int i_x = 0; i_x < 256; ++i_x)
		{
			for (int i_y = 0; i_y < 256; ++i_y)
			{
				float fTerrain = pTextureTerrain[i_x][i_y] / 65536.0f;
				float fWater = pTextureWater[i_x][i_y] / 65536.0f;
				if (fWater < 0.99f)
				{
					pTextureHeight[i_x][i_y] = ((0.99f - fWater)*(-30.0f / 2.0f)) / 30.0f;
				}
				else
				{
					pTextureHeight[i_x][i_y] = (fTerrain * 120.0f / 2.0f) / 30.0f;
				}
			}
		}


		g_Scene_Geometry_Grass.Init(pTextureHeight);

		::glGenTextures(1, &hTexture_Terrain_Displacement);
		::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, 256, 256);
		::glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RED, GL_FLOAT, pTextureHeight);
		::glBindTexture(GL_TEXTURE_2D, 0);

		::free(pTextureHeight);
		::free(pTextureWater);
		::free(pTextureTerrain);
	}
	GLuint hTexture_Terrain_Displacement_MaxMin;
	{
		::glGenTextures(1, &hTexture_Terrain_Displacement_MaxMin);
		::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement_MaxMin);
		::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 16, 16);//ES 不支持 GL_RG32F
		::glBindTexture(GL_TEXTURE_2D, 0);

		static char const *const Source_Terrain_MaxMin_Compute =
			R"(#version 310 es
layout(binding = 4) uniform highp sampler2D g_TextureIn;
layout(binding = 3, rgba32f) writeonly uniform highp image2D g_TextureOut; //ES 不支持 rg32f

shared int s_iHeightMax_Complement;
shared int s_iHeightMin_Complement;

//Dispatch(16, 16, 1) 256×256 -> 16×16

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main()
{
    //IEEE 754

    //符号位 1位 
	//符号位 0:正数
	//符号位 1:负数
    
	//阶码 8位 尾数 23位 
	//阶码 0: 非规约形式 阶码 -126 尾数 最高有效位为0
	//阶码 1-254: 规约形式 阶码 -126-127（移码127）尾数 最高有效位为1
	//阶码 255: 特殊值 //尾数 0：无穷大 //尾数 非0： NaN(Not a Number)

	//规约形式的浮点数 尾数最高有效位为1 显然不可能为零
	//零应当为非规约形式 且尾数为0 由于符号位不同 有正零和负零之分

	//大小比较
	//根据IEEE 754标准可知
	//当符号位表示正数或负数时，都满足：阶码和尾数组成的31位定点数的值越大，所表示的浮点数的值就越大
	//符号位的行为接近定点数的“原码”表示（ 0：正数 且 1：负数 ），为方便比较大小可以转换为“补码”
	
	//算法骨架

	//浮点数->定点数“原码”
	//floatBitsToInt

	//定点数“原码”->定点数“补码”
	//符号位为0：不变
	//符号位为1：符号位不变且其余各位按位取反（相当于按位异或0X7FFFFFFF）->在以上结果的基础上加1
	//X -> ((X & 0X80000000) != 0) ? ((X^0X7FFFFFFF) + 1) : X

	//归约（基于定点数“补码”）
	//atomicMax
	//atomicMin

	//定点数“补码”->定点数“原码”
	//符号位为0：不变
	//符号位为1：符号位不变且其余各位按位取反（相当于按位异或0X7FFFFFFF）->在以上结果的基础上加1
	//X -> (X & 0X80000000) ? ((X^0X7FFFFFFF) + 1) : X

	//定点数“原码”->浮点数
	//intBitsToFloat

    //初始化
	if (gl_LocalInvocationID.x == 0U && gl_LocalInvocationID.y == 0U)
	{
		s_iHeightMax_Complement = 0X80800001;//0XFF7FFFFF = floatBitsToInt(-FLT_MAX) //0X80800001 = (0XFF7FFFFF^0X7FFFFFFF) + 1
		s_iHeightMin_Complement = 0X7F7FFFFF;//floatBitsToInt(FLT_MAX)
	}
	memoryBarrierShared();
    barrier();

    //映射
	float fHeight = texelFetch(g_TextureIn, ivec2(gl_GlobalInvocationID.xy), 0).x;
	int iHeight_Trueform = floatBitsToInt(fHeight); //浮点数->定点数“原码”
	int iHeight_Complement = ((iHeight_Trueform & 0X80000000) != 0) ? ((iHeight_Trueform ^ 0X7FFFFFFF) + 1) : iHeight_Trueform; //定点数“原码”->定点数“补码”

    //归约
	atomicMax(s_iHeightMax_Complement, iHeight_Complement);//基于定点数“补码”
	atomicMin(s_iHeightMin_Complement, iHeight_Complement);//基于定点数“补码”

    memoryBarrierShared();
    barrier();

    if (gl_LocalInvocationID.x == 0U && gl_LocalInvocationID.y == 0U)
	{
		int iHeightMax_Complement = s_iHeightMax_Complement;
		int iHeightMax_Trueform = ((iHeightMax_Complement & 0X80000000) != 0) ? ((iHeightMax_Complement ^ 0X7FFFFFFF) + 1) : iHeightMax_Complement; //定点数“补码”->定点数“原码”
		int iHeightMin_Complement = s_iHeightMin_Complement;
		int iHeightMin_Trueform = ((iHeightMin_Complement & 0X80000000) != 0) ? ((iHeightMin_Complement ^ 0X7FFFFFFF) + 1) : iHeightMin_Complement; //定点数“补码”->定点数“原码”

		float fHeightMax = intBitsToFloat(iHeightMax_Trueform); //定点数“原码”->浮点数
		float fHeightMin = intBitsToFloat(iHeightMin_Trueform); //定点数“原码”->浮点数

		imageStore(g_TextureOut, ivec2(gl_WorkGroupID.xy), vec4(fHeightMax, fHeightMin, 0.0f, 0.0f));
	}
})";

		GLuint hProgram_Terrain_MaxMin;
		{
			GLuint hShader_Terrain_MaxMin_Compute;
			{
				hShader_Terrain_MaxMin_Compute = glCreateShader(GL_COMPUTE_SHADER);
				::glShaderSource(hShader_Terrain_MaxMin_Compute, 1U, &Source_Terrain_MaxMin_Compute, NULL);
				::glCompileShader(hShader_Terrain_MaxMin_Compute);

				GLint CompileStatus;
				::glGetShaderiv(hShader_Terrain_MaxMin_Compute, GL_COMPILE_STATUS, &CompileStatus);
				assert(CompileStatus == GL_TRUE);
				if (CompileStatus != GL_TRUE)
				{
					char ShadeInfoLog[4096];
					::glGetShaderInfoLog(hShader_Terrain_MaxMin_Compute, 4096, NULL, ShadeInfoLog);
					::OutputDebugStringA(ShadeInfoLog);
				}
			}

			hProgram_Terrain_MaxMin = glCreateProgram();
			::glAttachShader(hProgram_Terrain_MaxMin, hShader_Terrain_MaxMin_Compute);

			::glLinkProgram(hProgram_Terrain_MaxMin);
			GLint LinkStatus;
			::glGetProgramiv(hProgram_Terrain_MaxMin, GL_LINK_STATUS, &LinkStatus);
			assert(LinkStatus == GL_TRUE);
			if (LinkStatus != GL_TRUE)
			{
				char ProgramInfoLog[4096];
				::glGetProgramInfoLog(hProgram_Terrain_MaxMin, 4096, NULL, ProgramInfoLog);
				::OutputDebugStringA(ProgramInfoLog);
			}
		}

		//CS
		::glUseProgram(hProgram_Terrain_MaxMin);
		//SRV
		::glBindSampler(4, hSampler_Point);
		::glActiveTexture(GL_TEXTURE4);
		::glBindTexture(GL_TEXTURE_2D, hTexture_Terrain_Displacement);
		//UAV
		::glBindImageTexture(3, hTexture_Terrain_Displacement_MaxMin, 0, FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);//ES 不支持 GL_RG32F

		::glDispatchCompute(16, 16, 1);

		//Resource Barrier
		//glBindImageTexture 3 <- 0
	}
	GLuint hTexture_Terrain_Albedo = ::glexCreateDDSTextureFromFile("Terrain_Albedo.dds");

	//Scene_Geometry_Water_Init
	BufferConstant_WaterTileData WaterTileData;
	WaterTileData.m_WaterToWorld.r[0].x = 1.0f;
	WaterTileData.m_WaterToWorld.r[0].y = 0.0f;
	WaterTileData.m_WaterToWorld.r[0].z = 0.0f;
	WaterTileData.m_WaterToWorld.r[0].w = 0.0f;
	WaterTileData.m_WaterToWorld.r[1].x = 0.0f;
	WaterTileData.m_WaterToWorld.r[1].y = 1.0f;
	WaterTileData.m_WaterToWorld.r[1].z = 0.0f;
	WaterTileData.m_WaterToWorld.r[1].w = 0.0f;
	WaterTileData.m_WaterToWorld.r[2].x = 0.0f;
	WaterTileData.m_WaterToWorld.r[2].y = 0.0f;
	WaterTileData.m_WaterToWorld.r[2].z = 1.0f;
	WaterTileData.m_WaterToWorld.r[2].w = 0.0f;
	WaterTileData.m_WaterToWorld.r[3].x = 0.0f;
	WaterTileData.m_WaterToWorld.r[3].y = 0.0f;
	WaterTileData.m_WaterToWorld.r[3].z = 0.0f;
	WaterTileData.m_WaterToWorld.r[3].w = 1.0f;
	WaterTileData.m_UVAnimated.x = 0.0f;
	WaterTileData.m_UVAnimated.y = 0.0f;
	WaterTileData.m_UVAnimated.z = 0.0f;
	WaterTileData.m_UVAnimated.w = 0.0f;

	g_Render_Geometry_Grass_GBufferPass.Init();

	g_Render_Geometry_Terrain.Init();

	g_Render_Geometry_Water.Init();

	g_Render_SkyBox_ForwardPass.Init();

	LARGE_INTEGER Frequency;
	::QueryPerformanceFrequency(&Frequency);

	std::vector<PTVector4F> Billborad_Origin;
	std::vector<PTVector4F> Wave_Translation;
	std::vector<PTVector4F> Texture_Index;

	while (true)
	{
#if 0
		//ShadowPass
		//OM
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_ShadowPass);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_ShadowPass);

		//Clear
		{
			GLfloat depth = 0.0f;
			::glClearBufferfv(GL_DEPTH, 0, &depth);
		}
#endif

#if 1
		//Sensor-PlanarReflection
		//CPU Update
		{
			//Sensor
			PTVector3F EyePosition = { g_EyePosition.x,5.5f*2.0f - g_EyePosition.y,g_EyePosition.z };//Water Position (0, 5.5, 0)
			PTVector3F EyeDirection = { g_EyeDirection.x,-g_EyeDirection.y,g_EyeDirection.z };
			PTVector3F UpDirection = { g_UpDirection.x,-g_UpDirection.y,g_UpDirection.z };

			PTMatrixSIMD View;
			PTMatrixSIMD InvView;
			View = ::PTMatrix4x4SIMDLookToRH(PTVector3FLoad(&EyePosition), PTVector3FLoad(&EyeDirection), PTVector3FLoad(&UpDirection), &InvView);
			PTMatrix4x4FStore(&l_BufferSensorData.View, View);
			PTMatrix4x4FStore(&l_BufferSensorData.InvView, InvView);
			
			PTMatrixSIMD Projection;
			PTMatrixSIMD InvProjection;
			Projection = ::PTMatrix4x4SIMDPerspectiveFovRH(1.0f, 1.0f, 0.1f, 500.0f, &InvProjection);
			PTMatrix4x4FStore(&l_BufferSensorData.Projection, Projection);
			PTMatrix4x4FStore(&l_BufferSensorData.InvProjection, InvProjection);
			
			PTFrustumSIMD Frustum = ::PTFrustumSIMDLoadRH(::PTMatrix4x4SIMDMultiply(Projection, View));
			PTFrustumFStore(&l_BufferSensorData.FrustumPlane, Frustum);
			
			::glBindBuffer(GL_UNIFORM_BUFFER, hBufferConstant_Sensor);
			::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferSensorData), &l_BufferSensorData);
			::glBindBuffer(GL_UNIFORM_BUFFER, 0U);
		}

		//PlanarReflectionPass
		//OM
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_PlanarReflectionPass);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_PlanarReflectionPass);

		//Clear
		{
			GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			::glClearBufferfv(GL_COLOR, 0, color);
			GLfloat depth = 0.0f;
			::glClearBufferfv(GL_DEPTH, 0, &depth);
		}

		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}

		//Terrain
		g_Render_Geometry_Terrain.Execute_PlanarReflectionPass(hBufferConstant_Sensor, &TerrainTileData, hTexture_Terrain_Displacement, hTexture_Terrain_Displacement_MaxMin, hTexture_Terrain_Albedo, hBufferConstant_Light);
#endif

		//Sensor
		//CPU Update
		{
			//Sensor
			PTVector3F EyePosition = { g_EyePosition.x,g_EyePosition.y,g_EyePosition.z };
			PTVector3F EyeDirection = { g_EyeDirection.x,g_EyeDirection.y,g_EyeDirection.z };
			PTVector3F UpDirection = { g_UpDirection.x,g_UpDirection.y,g_UpDirection.z };
			PTMatrixSIMD View;
			PTMatrixSIMD InvView;
			View = ::PTMatrix4x4SIMDLookToRH(PTVector3FLoad(&EyePosition), PTVector3FLoad(&EyeDirection), PTVector3FLoad(&UpDirection), &InvView);
			PTMatrix4x4FStore(&l_BufferSensorData.View, View);
			PTMatrix4x4FStore(&l_BufferSensorData.InvView, InvView);
			PTMatrixSIMD Projection;
			PTMatrixSIMD InvProjection;
			Projection = ::PTMatrix4x4SIMDPerspectiveFovRH(1.0f, 1.0f, 0.1f, 500.0f, &InvProjection);
			PTMatrix4x4FStore(&l_BufferSensorData.Projection, Projection);
			PTMatrix4x4FStore(&l_BufferSensorData.InvProjection, InvProjection);
			PTFrustumSIMD Frustum = ::PTFrustumSIMDLoadRH(::PTMatrix4x4SIMDMultiply(Projection, View));
			PTFrustumFStore(&l_BufferSensorData.FrustumPlane, Frustum);
			::glBindBuffer(GL_UNIFORM_BUFFER, hBufferConstant_Sensor);
			::glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BufferSensorData), &l_BufferSensorData);
			::glBindBuffer(GL_UNIFORM_BUFFER, 0U);

			//Animate
			LARGE_INTEGER PerformanceCount;
			::QueryPerformanceCounter(&PerformanceCount);

			float fTime = static_cast<float>(PerformanceCount.QuadPart) / static_cast<float>(Frequency.QuadPart);

			fTime *= 2.0f;

			//Scene
			//Grass
			Billborad_Origin.clear();
			Wave_Translation.clear();
			Texture_Index.clear();		
#if 1
			PTMatrixSIMD ViewProjection = ::PTMatrix4x4SIMDMultiply(::PTMatrix4x4SIMDPerspectiveFovRH(2.5f, 1.0f, 0.1f, 100.0f, NULL), View);//Distance 100.0f
			g_Scene_Geometry_Grass.CullAndAnimate(ViewProjection, fTime, Billborad_Origin, Wave_Translation, Texture_Index);
#else
			::PTMatrix4x4SIMDPerspectiveFovRH(2.5f, 1.0f, 0.1f, 50.0f, &InvProjection);//Distance 50.0f
			g_Scene_Geometry_Grass.CullAndAnimate(InvView, InvProjection, fTime, Billborad_Origin, Wave_Translation, Texture_Index);
#endif

			//Water
			WaterTileData.m_UVAnimated.x = fTime * 0.05f;
			WaterTileData.m_UVAnimated.y = fTime * 0.05f;
			WaterTileData.m_UVAnimated.z = -fTime * 0.05f;
			WaterTileData.m_UVAnimated.w = fTime * 0.05f;
		}

		//GBufferPasse
		//OM
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_GBufferPass);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_GBufferPass);

		//Clear
		{
			GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			::glClearBufferfv(GL_COLOR, 0, color);
			::glClearBufferfv(GL_COLOR, 1, color);
			::glClearBufferfv(GL_COLOR, 2, color);
			GLfloat depth = 0.0f;
			::glClearBufferfv(GL_DEPTH, 0, &depth);
		}

		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}

		//Terrain
		g_Render_Geometry_Terrain.Execute_GBufferPass(hBufferConstant_Sensor, &TerrainTileData, hTexture_Terrain_Displacement, hTexture_Terrain_Displacement_MaxMin, hTexture_Terrain_Albedo);

		//Water
		//g_Render_Geometry_Water.Execute_GBufferPass(hBufferConstant_Sensor, &WaterTileData);

		//Grass
		assert(Billborad_Origin.size() == Wave_Translation.size());
		assert(Wave_Translation.size() == Texture_Index.size());
		g_Render_Geometry_Grass_GBufferPass.Execute(hBufferConstant_Sensor, &Billborad_Origin[0], &Wave_Translation[0], &Texture_Index[0], Billborad_Origin.size());

		//LightPass
		//OM
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_LightPass);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_LightPass);

		//Clear
		{
			GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			::glClearBufferfv(GL_COLOR, 0, color);
		}

		//Light_Directional
		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}
		::glDisable(GL_CULL_FACE);

		//OM
		::glDisable(GL_DEPTH_TEST);
		::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

		//VS+PS
		::glUseProgram(hProgram_Light_Directional);
		//CBV
		::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
		::glBindBufferBase(GL_UNIFORM_BUFFER, 7, hBufferConstant_Light);

		//SRV
		::glBindSampler(12, hSampler_Point);
		::glActiveTexture(GL_TEXTURE12);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Depth);
		
		::glBindSampler(13, hSampler_Point);
		::glActiveTexture(GL_TEXTURE13);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMacro);

		::glBindSampler(14, hSampler_Point);
		::glActiveTexture(GL_TEXTURE14);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_NormalMeso);
		
		::glBindSampler(15, hSampler_Point);
		::glActiveTexture(GL_TEXTURE15);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Albedo);
		
		::glBindSampler(16, hSampler_Point);
		::glActiveTexture(GL_TEXTURE16);
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hRenderTargetTexture_GBufferPass_Specular);
		
		::glBindSampler(5, hSampler_Point);
		::glActiveTexture(GL_TEXTURE5);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_ShadowPass_Depth);

		::glDrawArrays(GL_TRIANGLES, 0U, 3U);

		//ResolvePass
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_GBufferPass_Resoleved);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_GBufferPass);
		::glBlitFramebuffer(0U, 0U, 800U, 600U, 0U, 0U, 800U, 600U, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		GLenum bufs[1];
		::glReadBuffer(GL_COLOR_ATTACHMENT0);
		bufs[0] = GL_COLOR_ATTACHMENT0;
		::glDrawBuffers(1,bufs);
		::glBlitFramebuffer(0U, 0U, 800U, 600U, 0U, 0U, 800U, 600U, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		::glReadBuffer(GL_COLOR_ATTACHMENT1);
		bufs[0] = GL_COLOR_ATTACHMENT1;
		::glDrawBuffers(1, bufs);
		::glBlitFramebuffer(0U, 0U, 800U, 600U, 0U, 0U, 800U, 600U, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		::glReadBuffer(GL_COLOR_ATTACHMENT2);
		bufs[0] = GL_COLOR_ATTACHMENT2;
		::glDrawBuffers(1, bufs);
		::glBlitFramebuffer(0U, 0U, 800U, 600U, 0U, 0U, 800U, 600U, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		::glReadBuffer(GL_COLOR_ATTACHMENT3);
		bufs[0] = GL_COLOR_ATTACHMENT3;
		::glDrawBuffers(1, bufs);
		::glBlitFramebuffer(0U, 0U, 800U, 600U, 0U, 0U, 800U, 600U, GL_COLOR_BUFFER_BIT, GL_NEAREST);


		//ForwardPass
		//OM
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hRenderTarget_ForwardPass);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, hRenderTarget_ForwardPass);

		::glCopyImageSubDataOES(
			hRenderTargetTexture_GBufferPass_Depth_Resolved, GL_TEXTURE_2D, 0U, 0U, 0U, 0U,
			hRenderTargetTexture_ForwardPass_Depth, GL_TEXTURE_2D, 0U, 0U, 0U, 0U,
			800.0f, 600.0f, 1.0f
		);

		::glCopyImageSubDataOES(
			hRenderTarget_RadianceTotal, GL_TEXTURE_2D, 0U, 0U, 0U, 0U,
			hRenderTarget_RadianceDirect, GL_TEXTURE_2D, 0U, 0U, 0U, 0U,
			800.0f, 600.0f, 1.0f
		);

#if 0
		//Light_ScreenSpaceReflection
		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}
		::glDisable(GL_CULL_FACE);

		//OM
		::glDisable(GL_DEPTH_TEST);
		::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		::glEnable(GL_BLEND);
		::glBlendFuncSeparateiOES(0, GL_ONE, GL_ONE, GL_ONE, GL_ONE);
		::glBlendEquationSeparateiOES(0, GL_FUNC_ADD, GL_FUNC_ADD);

		//VS+PS
		::glUseProgram(hProgram_Light_ScreenSpaceReflection_Fragment);
		//CBV
		::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);

		//SRV
		::glBindSampler(12, hSampler_Point);
		::glActiveTexture(GL_TEXTURE12);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Depth_Resolved);
		
		::glBindSampler(13, hSampler_Point);
		::glActiveTexture(GL_TEXTURE13);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMacro_Resolved);
		
		::glBindSampler(14, hSampler_Point);
		::glActiveTexture(GL_TEXTURE14);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_NormalMeso_Resolved);
		
		::glBindSampler(15, hSampler_Point);
		::glActiveTexture(GL_TEXTURE15);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Albedo_Resolved);
		
		::glBindSampler(16, hSampler_Point);
		::glActiveTexture(GL_TEXTURE16);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Specular_Resolved);
		
		::glBindSampler(17, hSampler_Point);
		::glActiveTexture(GL_TEXTURE17);
		::glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceDirect);

		::glDrawArrays(GL_TRIANGLES, 0U, 3U);
#endif

		//Water
		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}

#if 0
		//SRV
		::glBindSampler(12, hSampler_Point);
		::glActiveTexture(GL_TEXTURE12);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_GBufferPass_Depth_Resolved);

		::glBindSampler(17, hSampler_Point);
		::glActiveTexture(GL_TEXTURE17);
		::glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceDirect);

		g_Render_Geometry_Water.Execute_ForwardPass_ScreenSpaceReflection(hBufferConstant_Sensor, &WaterTileData, hBufferConstant_Light);
#else

		//SRV
		::glBindSampler(16, hSampler_Point);
		::glActiveTexture(GL_TEXTURE16);
		::glBindTexture(GL_TEXTURE_2D, hRenderTargetTexture_PlanarReflectionPass_Radiance);

		::glBindSampler(17, hSampler_Point);
		::glActiveTexture(GL_TEXTURE17);
		::glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceDirect);

		g_Render_Geometry_Water.Execute_ForwardPass_PlanarReflection(hBufferConstant_Sensor, &WaterTileData, hBufferConstant_Light);
#endif

		//SkyBox
		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}

		g_Render_SkyBox_ForwardPass.Execute(hBufferConstant_Sensor);

		//PostProcess
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0U);
		::glBindFramebuffer(GL_READ_FRAMEBUFFER, 0U);
	
		//RS
		::glClipControlEXT(GL_LOWER_LEFT_EXT, GL_ZERO_TO_ONE_EXT);
		{
			GLfloat Viewport[4] = { 0.0f,0.0f,800.0f,600.0f };
			::glViewportArrayvOES(0U, 1U, Viewport);
		}
		::glDisable(GL_CULL_FACE);

		//OM
		::glDisable(GL_DEPTH_TEST);
		::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		::glDisable(GL_BLEND);

		//VS+PS
		glUseProgram(hProgram_PostProcess);
		//SRV
		glBindSampler(17, hSampler_Point);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_2D, hRenderTarget_RadianceTotal);

		::glDrawArrays(GL_TRIANGLES, 0U, 3U);

		eglbResult = ::eglSwapInterval(hDisplay, 1);
		eglbResult = ::eglSwapBuffers(hDisplay, hSurface);
		assert(eglbResult == EGL_TRUE);
	}
	
	return 0U;
}