#define ROOTSIGNATURE "\
DescriptorTable(CBV(offset=0,b0,space=0,numdescriptors=1)),\
DescriptorTable(CBV(offset=0,b1,space=0,numdescriptors=1)),\
DescriptorTable(SRV(offset=0,t0,space=0,numdescriptors=1)),\
DescriptorTable(UAV(offset=0,u0,space=0,numdescriptors=2))"

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

Texture2D<float> g_GBuffer_Depth:register(t0, space0);

RWTexture2D<float2> g_GBuffer_PositionEyeXY:register(u0, space0);
RWTexture2D<float> g_GBuffer_PositionEyeZ:register(u1, space0);


[RootSignature(ROOTSIGNATURE)]
[numthreads(16, 16, 1)]
void main(uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{
	//直接在GBuffer中存放PositionEye 可能的无效写入为3个float
	//根据Depth重建PositionEye 可能的无效写入为1个float

	uint2 l_PositionTexture = uint2(g_Viewport_TopLeftXY)+gl_GlobalInvocationID.xy;

	float3 l_PositionFramebuffer;
	l_PositionFramebuffer.xy = g_Viewport_TopLeftXY + float2(gl_GlobalInvocationID.xy);
	l_PositionFramebuffer.z = g_GBuffer_Depth.mips[0][l_PositionTexture];

	float3 l_PositionNormalizedDevice;
	l_PositionNormalizedDevice.xy = (l_PositionFramebuffer.xy - g_Viewport_TopLeftXY) / g_Viewport_WidthHeight;
	l_PositionNormalizedDevice.xy = (l_PositionNormalizedDevice.xy - float2(0.5f, 0.5f))*float2(2.0f, -2.0f);
	l_PositionNormalizedDevice.z = (l_PositionFramebuffer.z - g_Viewport_MinDepth) / (g_Viewport_MaxDepth - g_Viewport_MinDepth);

	float4 l_PositionEye1x4 = mul(float4(l_PositionNormalizedDevice, 1.0f), g_Sensor_InverseProjection);

	float3 l_PositionEye = l_PositionEye1x4.xyz / l_PositionEye1x4.www;

	//解决precise关键字的问题
	g_GBuffer_PositionEyeXY[l_PositionTexture] = l_PositionEye.xy;
	g_GBuffer_PositionEyeZ[l_PositionTexture] = l_PositionEye.z;
}