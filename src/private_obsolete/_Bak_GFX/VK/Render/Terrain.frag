#version 310 es
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
	highp mat4x4 g_TangentToWorld;//基变换 Tangent Bitangent Normal 平移变换 Origin
};

layout(location = 0) out highp vec4 SV_Target0; //NormalMacro
layout(location = 1) out highp vec4 SV_Target1; //NormalMeso
layout(location = 2) out highp vec4 SV_Target2; //Albedo
layout(location = 3) out highp vec4 SV_Target3; //Specular

layout (early_fragment_tests) in;

void main() 
{

//在Domain Shader中计算Normal的方案已经被弃用
#if 0
	//highp vec3 Normal = normalize(RS_Tessellation_Normal);
#endif

//Gather会导致“栅格化”，使用Linear Sample能有效避免“栅格化”发生
#if 0
	//Gather
    //(0,1) (1,1) (1,0) (0,0)
    highp vec4 l_Tessellation_NXRM = textureGather(g_TextureDisplacement, RS_Tessellation_UV_Height, 0);
    //highp vec4 l_Tessellation_LMBX = textureGatherOffset(g_TextureDisplacement, RS_Tessellation_UV_Height, ivec2(-1,-1), 0);
	
	//Normal
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.z) * 30.0f, 
			(l_Tessellation_NXRM.w - l_Tessellation_NXRM.x) * 30.0f, 
			1.0f //WorldSizePerTexel = 256Tile / 256Texture
			));
#endif

	highp float l_Tessellation_M = texture(g_TextureHeight, RS_Tessellation_UV_Height).r * 30.0f;
	highp float l_Tessellation_R = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(1,0)).r * 30.0f;
	highp float l_Tessellation_N = textureOffset(g_TextureHeight, RS_Tessellation_UV_Height, ivec2(0,1)).r * 30.0f;
	
	highp vec3 l_Tessellation_NormalTangent = normalize(vec3(
			(l_Tessellation_M - l_Tessellation_R), 
			(l_Tessellation_M - l_Tessellation_N), 
			1.0f //WorldSizePerTexel = 256Tile / 256Texture
			));

	highp vec3 NormalMeso = mat3x3(g_TangentToWorld[0].xyz, g_TangentToWorld[1].xyz, g_TangentToWorld[2].xyz) * l_Tessellation_NormalTangent;
	highp vec3 NormalMacro = NormalMeso;

	//以下内容与Terrain Tessellation无关------------------------------------------------------

    //NVIDIA GameWorks Vulkan and OpenGL Samples / Texture Array Terrain Sample
    //https://developer.nvidia.com/gameworks-vulkan-and-opengl-samples
    
    //以过程式的方式根据高度确定融合因子
    //主流游戏引擎往往采用另一种完全不同的做法，融合因子由美术制作并保存在Asset中，大小与HeightField相同

    highp vec3 l_Tessellation_UVW_Albedo = vec3(RS_Tessellation_UV_Height*16.0f, l_Tessellation_M / 15.0f); //PerPatch //Sample Wrap MirroredRepeat

	highp vec3 Albedo = texture(g_TextureAlbedo,l_Tessellation_UVW_Albedo).xyz;
    
	highp vec3 Specular = vec3(0.0f,0.0f,0.0f);

	SV_Target0 = vec4((NormalMacro+1.0f)*0.5f,1.0f);	
	SV_Target1 = vec4((NormalMeso+1.0f)*0.5f,1.0f);	
	SV_Target2 = vec4(Albedo,1.0f);	
	SV_Target3 = vec4(Specular,1.0f);	
}