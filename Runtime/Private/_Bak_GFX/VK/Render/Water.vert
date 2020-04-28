#version 310 es

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
}