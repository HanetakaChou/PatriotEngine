#include "DirectLight.rs.hlsli"

inline float PCFSampleCompare(Texture2D TextureShadowMap, SamplerComparisonState SamplerShadowMap, float3 UVDShadowMap, float2 TexelSizeUV, float DepthConstantBias)
{
	//偏导数深度偏移
	//Calculating a Per-Texel Depth Bias with DDX and DDY for Large PCFs
	//http://msdn.microsoft.com/en-us/library/ee416307
	float3 DDX_UVDShadowMap = ddx(UVDShadowMap.xyz);
	float3 DDY_UVDShadowMap = ddy(UVDShadowMap.xyz);

	//[GBufferOffsetU GbufferOffsetV] * [ddx(ShadowMapU) ddx(ShadowMapV)] = [ShadowMapOffsetU ShadowMapOffsetV]
	//	                                [ddy(ShadowMapU) ddy(ShadowMapV)]

	//[ShadowMapOffsetU ShadowMapOffsetV] * Inverse([ddx(ShadowMapU) ddx(ShadowMapV)]) = [GBufferOffsetU GbufferOffsetV]
	//	                                           ([ddy(ShadowMapU) ddy(ShadowMapV)])

	//[GBufferOffsetU GbufferOffsetV] * [ddx(ShadowMapDepth)] =  [ShadowMapOffsetDepth]
	//                                  [ddy(ShadowMapDepth)]

	float2x2 mOffsetUVGBufferToShadowMap = float2x2(DDX_UVDShadowMap.xy, DDY_UVDShadowMap.xy);

	//Inverse
	float2x2 mOffsetUVShadowMapToGBuffer;
	{
		float fDeterminant = (mOffsetUVGBufferToShadowMap._11*mOffsetUVGBufferToShadowMap._22 - mOffsetUVGBufferToShadowMap._12*mOffsetUVGBufferToShadowMap._21);
		float2x2 mAdjugate = float2x2(
			mOffsetUVGBufferToShadowMap._22, -mOffsetUVGBufferToShadowMap._12,
			-mOffsetUVGBufferToShadowMap._21, mOffsetUVGBufferToShadowMap._11
			);
		mOffsetUVShadowMapToGBuffer = mAdjugate*((1.0f / fDeterminant).xxxx);
	}

	float fShadowFactor = 0.0f;

	fShadowFactor += TextureShadowMap.SampleCmpLevelZero(SamplerShadowMap, UVDShadowMap.xy, UVDShadowMap.z - DepthConstantBias);

	fShadowFactor += TextureShadowMap.SampleCmpLevelZero(SamplerShadowMap, UVDShadowMap.xy + float2(TexelSizeUV.x, 0.0f), UVDShadowMap.z + dot(mul(float2(TexelSizeUV.x, 0.0f), mOffsetUVShadowMapToGBuffer), float2(DDX_UVDShadowMap.z, DDY_UVDShadowMap.z)) - DepthConstantBias);

	fShadowFactor += TextureShadowMap.SampleCmpLevelZero(SamplerShadowMap, UVDShadowMap.xy + float2(0.0f, TexelSizeUV.y), UVDShadowMap.z + dot(mul(float2(0.0f, TexelSizeUV.y), mOffsetUVShadowMapToGBuffer), float2(DDX_UVDShadowMap.z, DDY_UVDShadowMap.z)) - DepthConstantBias);

	fShadowFactor += TextureShadowMap.SampleCmpLevelZero(SamplerShadowMap, UVDShadowMap.xy + float2(-TexelSizeUV.x, 0.0f), UVDShadowMap.z + dot(mul(float2(-TexelSizeUV.x, 0.0f), mOffsetUVShadowMapToGBuffer), float2(DDX_UVDShadowMap.z, DDY_UVDShadowMap.z)) - DepthConstantBias);

	fShadowFactor += TextureShadowMap.SampleCmpLevelZero(SamplerShadowMap, UVDShadowMap.xy + float2(0.0f, -TexelSizeUV.y), UVDShadowMap.z + dot(mul(float2(0.0f, -TexelSizeUV.y), mOffsetUVShadowMapToGBuffer), float2(DDX_UVDShadowMap.z, DDY_UVDShadowMap.z)) - DepthConstantBias);

	fShadowFactor /= 5.0f;

	return fShadowFactor;
}