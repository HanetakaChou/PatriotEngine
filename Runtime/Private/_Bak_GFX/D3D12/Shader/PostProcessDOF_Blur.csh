//Blur

cbuffer g_CBuffer_Viewport:register(b0)
{
	float g_Viewport_TopLeftX;
	float g_Viewport_TopLeftY;
	float g_Viewport_Width;
	float g_Viewport_Height;
	float g_Viewport_MinDepth;
	float g_Viewport_MaxDepth;
};

cbuffer g_CBuffer_Perspective:register(b1)
{
	float g_Perspective_Width;
	float g_Perspective_Height;
	float g_Perspective_NearZ;
	float g_Perspective_FarZ;
};

cbuffer g_CBuffer_DepthOfField:register(b2)
{
	float g_DepthOfField_NearPercent;//默认值 0.28f
	float g_DepthOfField_FarPercent;//默认值 0.49f
	float g_DepthOfField_Aperture;//默认值 0.25f
};

Texture2D<float4> g_TextureRadiancePrevious:register(t0);
Texture2D<float> g_TextureGBufferPositionEyeZ:register(t1);
Texture2D<float4> g_TextureRadiancePreviousSAT:register(t2);

RWTexture2D<float4> g_TextureRadianceNow:register(u0);

inline float3 SATAverage(uint X1, uint Y1, uint X2, uint Y2)
{
	//SAT
	//X1Y1---->X2Y1
	// |        |
	// V        V
	//X1Y2---->X2Y2
	//Average = (S(X2,Y2) - S(X2,Y1) - S(X1,Y2) + S(X1,Y1)) / ((X2 - X1) * (Y2 - Y1))
	float3 Accumulator = float3(0.0f, 0.0f, 0.0f);
	Accumulator += g_TextureRadiancePreviousSAT.mips[0][uint2(X2, Y2)].xyz;
	Accumulator -= g_TextureRadiancePreviousSAT.mips[0][uint2(X2, Y1)].xyz;
	Accumulator -= g_TextureRadiancePreviousSAT.mips[0][uint2(X1, Y2)].xyz;
	Accumulator += g_TextureRadiancePreviousSAT.mips[0][uint2(X1, Y1)].xyz;
	return Accumulator / ((X2 - X1) * (Y2 - Y1));
}

inline float3 PseudoGaussianBlur(uint2 l_Center_PositionTexture, float l_CircleOfConfusion_Radius)
{
	float3 Accumulator = float3(0.0f, 0.0f, 0.0f);

	uint l_R = uint(l_CircleOfConfusion_Radius);
	uint l_RDIV2 = max(1U, uint(l_CircleOfConfusion_Radius*0.5f));
	uint l_3RDIV4 = max(1U, uint(l_CircleOfConfusion_Radius*0.75f));

	//R*(R*0.5f)
	Accumulator += SATAverage(l_Center_PositionTexture.x - l_R, l_Center_PositionTexture.y - l_RDIV2, l_Center_PositionTexture.x + l_R, l_Center_PositionTexture.y + l_RDIV2);
	//(R*0.75f)*(R*0.75f)
	Accumulator += SATAverage(l_Center_PositionTexture.x - l_3RDIV4, l_Center_PositionTexture.y - l_3RDIV4, l_Center_PositionTexture.x + l_3RDIV4, l_Center_PositionTexture.y + l_3RDIV4);
	//(R*0.5f)*R
	Accumulator += SATAverage(l_Center_PositionTexture.x - l_RDIV2, l_Center_PositionTexture.y - l_R, l_Center_PositionTexture.x + l_RDIV2, l_Center_PositionTexture.y + l_R);

	return Accumulator*0.333333333f;
}

[numthreads(8, 4, 1)]
void main(uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{

	float l_Near_PositionEyeZ = g_Perspective_NearZ + (g_Perspective_FarZ - g_Perspective_NearZ)*g_DepthOfField_NearPercent;//近点
	float l_Far_PositionEyeZ = g_Perspective_NearZ + (g_Perspective_FarZ - g_Perspective_NearZ)*g_DepthOfField_FarPercent;//远点
	float l_BlurSizeScale = ((g_Viewport_Width + g_Viewport_Height)*0.02f)*g_DepthOfField_Aperture;//模糊大小缩放

	float l_Center_PositionEyeZ = g_TextureGBufferPositionEyeZ.mips[0][gl_GlobalInvocationID.xy];

	g_TextureRadianceNow[gl_GlobalInvocationID.xy] =
		((l_Center_PositionEyeZ >= l_Near_PositionEyeZ) && (l_Center_PositionEyeZ <= l_Far_PositionEyeZ))
		?
		g_TextureRadiancePrevious.mips[0][gl_GlobalInvocationID.xy]
		:
		(
		(l_Center_PositionEyeZ<l_Near_PositionEyeZ)
			?
			float4(PseudoGaussianBlur(gl_GlobalInvocationID.xy, max(2.0f, (1.0f - l_Center_PositionEyeZ / l_Near_PositionEyeZ)*l_BlurSizeScale)), 1.0f)
			:
			float4(PseudoGaussianBlur(gl_GlobalInvocationID.xy, max(2.0f, (l_Center_PositionEyeZ / l_Far_PositionEyeZ - 1.0f)*l_BlurSizeScale)), 1.0f)
			);
}