#define ROOTSIGNATURE "\
DescriptorTable(CBV(offset=0,b0,space=0,numdescriptors=1)),\
DescriptorTable(CBV(offset=0,b1,space=0,numdescriptors=1)),\
DescriptorTable(CBV(offset=0,b2,space=0,numdescriptors=1)),\
DescriptorTable(SRV(offset=0,t0,space=0,numdescriptors=4)),\
DescriptorTable(SRV(offset=0,t0,space=1,numdescriptors=1)),\
DescriptorTable(UAV(offset=0,u0,space=0,numdescriptors=1))"

cbuffer g_CBuffer_Sensor:register(b0, space0)
{
	column_major float4x4 g_Sensor_View;
	column_major float4x4 g_Sensor_InverseView;
	column_major float4x4 g_Sensor_Projection;
	column_major float4x4 g_Sensor_InverseProjection;
};

cbuffer g_CBuffer_Viewport:register(b1, space0)
{
	float2 g_Viewport_TopLeftXY;
	float2 g_Viewport_WidthHeight;
	float g_Viewport_MinDepth;
	float g_Viewport_MaxDepth;
};

cbuffer g_CBuffer_Light:register(b2, space0)
{
	float3 g_Ambient_Radiance;
	float g_Occlusion_SphereRadiusEye;//Default:0.01f
	float g_Occlusion_BiasAngle;//Default:0.2f
	float g_Occlusion_Scale;//Default:1.0f
};

Texture2D<float2> g_GBuffer_PositionEyeXY : register(t0, space0);
Texture2D<float> g_GBuffer_PositionEyeZ : register(t1, space0);
Texture2D<snorm float3> g_GBuffer_Normal : register(t2, space0);
Texture2D<unorm float3> g_GBuffer_Albedo : register(t3, space0);

Texture2D<float3> g_Radiance_Previous:register(t0, space1);

RWTexture2D<float3> g_Radiance_Now:register(u0, space0);

[RootSignature(ROOTSIGNATURE)]
[numthreads(16, 16, 1)]
void ComputeMain(uint3 gl_GlobalInvocationID : SV_DispatchThreadID, uint3 gl_LocalInvocationID : SV_GroupThreadID)
{
	//1. Occlusion
	uint2 l_Random4x4_Index = uint2(gl_LocalInvocationID.x % 4U, gl_LocalInvocationID.y % 4U);

	uint2 l_SphereCenter_PositionTexture = uint2(g_Viewport_TopLeftXY)+gl_GlobalInvocationID.xy;

	float3 l_SphereCenter_PositionEye = float3(g_GBuffer_PositionEyeXY.mips[0][l_SphereCenter_PositionTexture], g_GBuffer_PositionEyeZ.mips[0][l_SphereCenter_PositionTexture]);

	float3 l_SphereCenter_NormalEyeNormalized = g_GBuffer_Normal.mips[0][l_SphereCenter_PositionTexture];

	float l_SphereRadius_Eye = g_Occlusion_SphereRadiusEye;

	float l_SphereRadius_Texture;
	{
		float2 l_SphereRadius_Clip_XY = l_SphereRadius_Eye.xx*float2(g_Sensor_Projection._m00, g_Sensor_Projection._m11);
		float l_SphereRadius_Clip_W = l_SphereCenter_PositionEye.z*g_Sensor_Projection._m23;

		float2 l_SphereRadius_FrameBuffer_XY = (l_SphereRadius_Clip_XY.xy / l_SphereRadius_Clip_W.xx)*g_Viewport_WidthHeight.xy;

		l_SphereRadius_Texture = (l_SphereRadius_FrameBuffer_XY.x + l_SphereRadius_FrameBuffer_XY.y) * 0.5f;
	}

	float l_Occlusion_BiasAngleSin = sin(g_Occlusion_BiasAngle);

	float l_Occlusion_Scale = g_Occlusion_Scale;

	float4x4 l_Random4x4_Angle = float4x4(
		0.367931575f, 0.0249740928f, 0.304209858f, 0.141995281f,
		0.185859770f, 0.573927343f, 0.461712837f, 0.185075939f,
		0.192656606f, 0.559165776f, 0.182406366f, 0.255917281f,
		0.507797599f, 0.476437449f, 0.678749740f, 0.101216622f
		);

	float4x4 l_Random4x4_Ratio = float4x4(
		0.979269028f, 0.778312981f, 0.179841995f, 0.101347998f,
		0.981091022f, 0.00856199954f, 0.437979996f, 0.0444369987f,
		0.786985993f, 0.368645996f, 0.366948009f, 0.955089986f,
		0.228856996f, 0.757256985f, 0.877119005f, 0.591566980f
		);

	float l_RayDirection_RotationAngleRandom = l_Random4x4_Angle[l_Random4x4_Index.x][l_Random4x4_Index.y];

	float l_RayLength_TranslateRatioRandom = l_Random4x4_Ratio[l_Random4x4_Index.x][l_Random4x4_Index.y];

	float l_OcclusionInverse;
	{
		float l_RayDirection_RotationAngleStep = 0.785398163f;//2PI/8
		float l_RayLength_TranslateStepTexture = l_SphereRadius_Texture / (4.0f + 1.0f);//+1.0f for the l_RayLength_TranslateRatioRandom

		float l_OcclusionInverseAccumulator = 0.0f;

		//[unroll]
		for (uint i_PerDirection = 0U; i_PerDirection < 8U; ++i_PerDirection)
		{
			//NumDirection:8
			float l_RayDirection_RotationAngle = l_RayDirection_RotationAngleRandom + l_RayDirection_RotationAngleStep*i_PerDirection;

			float2 l_RayDirection_Texture;
			sincos(l_RayDirection_RotationAngle, l_RayDirection_Texture.y, l_RayDirection_Texture.x);

			//[unroll]
			for (uint i_PerStep = 0U; i_PerStep < 4U; ++i_PerStep)
			{
				//NumStep:4
				float l_RayLength_Texture = 1.0f + l_RayLength_TranslateStepTexture*(l_RayLength_TranslateRatioRandom + i_PerStep);

				uint2 l_Sample_PositionTexture = l_SphereCenter_PositionTexture + uint2(l_RayDirection_Texture*l_RayLength_Texture.xx);

				float3 l_Sample_PositionEye = float3(g_GBuffer_PositionEyeXY.mips[0][l_Sample_PositionTexture.xy], g_GBuffer_PositionEyeZ.mips[0][l_Sample_PositionTexture.xy]);

				float3 l_LightDirectionEye = l_Sample_PositionEye - l_SphereCenter_PositionEye;

				float AngularFalloff = max(dot(normalize(l_LightDirectionEye), l_SphereCenter_NormalEyeNormalized) - l_Occlusion_BiasAngleSin, 0.0f);

				float DistanceFalloff = max((1.0f - length(l_LightDirectionEye) / l_SphereRadius_Eye), 0.0f);

				l_OcclusionInverseAccumulator += AngularFalloff*DistanceFalloff;
			}
		}

		l_OcclusionInverse = l_OcclusionInverseAccumulator / 32.0f;
	}

	float l_Occlusion = 1.0f - l_OcclusionInverse*2.0f*l_Occlusion_Scale;

	//2. Ambient 
	float3 l_Ambient_Radiance = g_Ambient_Radiance.xyz;

	float3 l_ColorDiffuse = g_GBuffer_Albedo.mips[0][l_SphereCenter_PositionTexture.xy];//²»¿¼ÂÇ³ËÒÔ(1-Fresnel)

	float3 l_LightIncident_Radiance = l_Ambient_Radiance;

	float3 l_OutputRadiance_Accumulator = l_ColorDiffuse.xyz*(l_LightIncident_Radiance.xyz*l_Occlusion.xxx);//BRDF Lambert

	g_Radiance_Now[l_SphereCenter_PositionTexture.xy] = g_Radiance_Previous.mips[0][l_SphereCenter_PositionTexture.xy] + l_OutputRadiance_Accumulator;
}