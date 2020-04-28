#version 310 es
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
	highp mat4x4 g_TangentToWorld;//基变换 Tangent Bitangent Normal 平移变换 Origin
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

//五星!!!
//在Pixel Shader中计算Normal能使三角形个数维持在较低水平，原理与NormalMap类似
#if 0
    //Normal
	vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_LMBX.x - l_Tessellation_NXRM.z) * 30.0f, 
			(l_Tessellation_LMBX.z - l_Tessellation_NXRM.x) * 30.0f, 
			1.0f //WorldSizePerTexel = 256Tile / 256Texture
			));

    RS_Tessellation_Normal = mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * l_Tessellation_NormalTangent;
#endif

    vec4 l_Tessellation_PositionWorld = g_TangentToWorld * vec4(l_Tessellation_PositionTangent, 1.0f);
	RS_PositionWorld = l_Tessellation_PositionWorld.xyz;

    gl_Position = g_Projection * g_View * l_Tessellation_PositionWorld;
    RS_Tessellation_UV_Height = l_Tessellation_UV;
}