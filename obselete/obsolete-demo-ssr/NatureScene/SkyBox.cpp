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

#include "SkyBox.h"
#include <assert.h>

static char const *const Render_SkyBox_ForwardPass_Vertex =
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
		gl_Position = vec4(-1.0f,-1.0f,0.00001f,1.0f);//Reverse-Z
		l_Screen_UV = vec2(0.0f,0.0f);
	break;
	case 1:
		gl_Position = vec4(3.0f,-1.0f,0.00001f,1.0f);
		l_Screen_UV = vec2(2.0f,0.0f);
	break;
	case 2:
		gl_Position = vec4(-1.0f,3.0f,0.00001f,1.0f);
		l_Screen_UV = vec2(0.0f,2.0f);
	break;
	}
})";

static char const *const Render_SkyBox_ForwardPass_Fragment =
R"(#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
};

layout(binding = 13) uniform highp samplerCube g_TexRadiance;

layout(location = 0) out highp vec4 SV_Target0;

layout (early_fragment_tests) in;

void main() 
{
	highp vec4 Position_NormalizedDevice;
	Position_NormalizedDevice.x = l_Screen_UV.x*2.0f - 1.0f;
	Position_NormalizedDevice.y = l_Screen_UV.y*2.0f - 1.0f;
	Position_NormalizedDevice.z = 0.5f; //����
	Position_NormalizedDevice.w = 1.0f;

	highp vec4 Position_View = g_InvProjection * Position_NormalizedDevice;
	Position_View.xyzw /= Position_View.wwww;

	highp vec3 Direction_View = normalize(Position_View.xyz); //- Vec3(0.0f,0.0f,0.0f)

	highp vec3 Direction_World = mat3x3(g_InvView[0].xyz,g_InvView[1].xyz,g_InvView[2].xyz) * Direction_View;

	SV_Target0 = texture(g_TexRadiance,Direction_World);
})";

void Render_SkyBox_ForwardPass::Init()
{
	GLuint hShader_Render_SkyBox_ForwardPass_Vertex;
	{
		hShader_Render_SkyBox_ForwardPass_Vertex = ::glCreateShader(GL_VERTEX_SHADER);
		::glShaderSource(hShader_Render_SkyBox_ForwardPass_Vertex, 1U, &::Render_SkyBox_ForwardPass_Vertex, NULL);
		::glCompileShader(hShader_Render_SkyBox_ForwardPass_Vertex);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Render_SkyBox_ForwardPass_Vertex, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Render_SkyBox_ForwardPass_Vertex, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	GLuint hShader_Render_SkyBox_ForwardPass_Fragment;
	{
		hShader_Render_SkyBox_ForwardPass_Fragment = ::glCreateShader(GL_FRAGMENT_SHADER);
		::glShaderSource(hShader_Render_SkyBox_ForwardPass_Fragment, 1U, &::Render_SkyBox_ForwardPass_Fragment, NULL);
		::glCompileShader(hShader_Render_SkyBox_ForwardPass_Fragment);

		GLint CompileStatus;
		::glGetShaderiv(hShader_Render_SkyBox_ForwardPass_Fragment, GL_COMPILE_STATUS, &CompileStatus);
		assert(CompileStatus == GL_TRUE);
		if (CompileStatus != GL_TRUE)
		{
			char ShadeInfoLog[4096];
			::glGetShaderInfoLog(hShader_Render_SkyBox_ForwardPass_Fragment, 4096, NULL, ShadeInfoLog);
			::OutputDebugStringA(ShadeInfoLog);
		}
	}

	//m_hProgram_SkyBox
	{
		m_hProgram_ForwardPass_SkyBox = ::glCreateProgram();
		::glAttachShader(m_hProgram_ForwardPass_SkyBox, hShader_Render_SkyBox_ForwardPass_Vertex);
		::glAttachShader(m_hProgram_ForwardPass_SkyBox, hShader_Render_SkyBox_ForwardPass_Fragment);

		::glLinkProgram(m_hProgram_ForwardPass_SkyBox);
		GLint LinkStatus;
		::glGetProgramiv(m_hProgram_ForwardPass_SkyBox, GL_LINK_STATUS, &LinkStatus);
		assert(LinkStatus == GL_TRUE);
		if (LinkStatus != GL_TRUE)
		{
			char ProgramInfoLog[4096];
			::glGetProgramInfoLog(m_hProgram_ForwardPass_SkyBox, 4096, NULL, ProgramInfoLog);
			::OutputDebugStringA(ProgramInfoLog);
		}
	}

	m_hTextureRadiance_SkyBox = ::glexCreateDDSTextureFromFile("SkyBox.dds");
}

void Render_SkyBox_ForwardPass::Execute(GLuint hBufferConstant_Sensor)
{
	//RS
	::glDisable(GL_CULL_FACE);

	//OM
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_POLYGON_OFFSET_FILL);
	::glDepthFunc(GL_GREATER);
	::glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//VS+PS
	::glUseProgram(m_hProgram_ForwardPass_SkyBox);
	//CBV
	::glBindBufferBase(GL_UNIFORM_BUFFER, 3, hBufferConstant_Sensor);
	//SRV
	//::glBindSampler(13, hSampler_Linear);
	::glActiveTexture(GL_TEXTURE13);
	::glBindTexture(GL_TEXTURE_CUBE_MAP, m_hTextureRadiance_SkyBox);

	//IA
	//::glBindVertexArray(0U);
	//::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
	::glDrawArrays(GL_TRIANGLES, 0U, 3U);
}

Render_SkyBox_ForwardPass g_Render_SkyBox_ForwardPass;