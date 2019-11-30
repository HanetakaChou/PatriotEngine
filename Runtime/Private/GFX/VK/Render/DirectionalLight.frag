#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
};

layout(binding = 12) uniform highp sampler2DMS g_Texture_GBufferDepth;
layout(binding = 13) uniform highp sampler2DMS g_Texture_GBufferNormal;
layout(binding = 14) uniform highp sampler2DMS g_Texture_GBufferAlbedo;
layout(binding = 15) uniform highp sampler2DMS g_Texture_GBufferSpecular;

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
		
		highp vec3 NormalMacro = (texelFetch(g_Texture_GBufferNormal, ivec2(gl_FragCoord.xy), i).xyz)*2.0f-1.0f;
		highp vec3 ColorAlbedo = texelFetch(g_Texture_GBufferAlbedo, ivec2(gl_FragCoord.xy), i).xyz;
		highp vec3 ColorSpecular = texelFetch(g_Texture_GBufferSpecular, ivec2(gl_FragCoord.xy), i).xyz;
        highp float Glossiness = 1.0f; //应当从GBuffer中获取

		highp vec3 LightDirection = -normalize(g_LightDirection.xyz);

        //E_N
        highp vec3 IrradianceNormal = g_LightIrradiance.xyz*max(0.0f, dot(NormalMacro, LightDirection));

        //Normalized Blinn-Phong
        highp vec3 HalfVector = ViewDirection + LightDirection; //即NormalMicro

        highp float Smoothness = Glossiness * 5.0f;//与Glossiness正相关 

        highp float NormalDistribution =  ((Smoothness + 2.0f)/6.28f) * pow(max(0.0f, dot(HalfVector,NormalMacro)),Smoothness);

        highp vec3 Fresnel = ColorSpecular;

        highp vec3 BRDF_Specular = NormalDistribution * Fresnel * 0.25f; //G(l,v) = max(0,cos<LightDirection,NormalMacro>)*max(0,cos<ViewDirection,NormalMacro>)

        //Lambert
        highp vec3 ColorDiffuse = ColorAlbedo * (vec3(1.0f,1.0f,1.0f) - Fresnel);

        highp vec3 BRDF_Diffuse = ColorDiffuse / 3.14f;

        //L_V
        ViewRadiance += (BRDF_Specular + BRDF_Diffuse) * IrradianceNormal;
        
        //vec3(ShadowMask, ShadowMask, ShadowMask);
    }

	ViewRadiance *= 0.25f;

	SV_Target0 = vec4(ViewRadiance,1.0f);
}