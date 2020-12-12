//Column DownSweep

Texture2D<float4> g_TextureInputLocalScan:register(t0);
RWTexture2D<float4> g_TextureOutputGlobalScan:register(u0);

groupshared float3 s_GlobalReduce[64];//假定gl_WorkGroupID.y的最大值小于64

[numthreads(1, 32, 1)]
void main(
	uint3 gl_LocalInvocationID : SV_GroupThreadID,
	uint3 gl_WorkGroupID : SV_GroupID,
	uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{
	//1.并行映射，获取各个之前的线程组的所对应的像素的和（即所对应的前缀和数组中的最后一个元素）//即Partial Reduction
	s_GlobalReduce[gl_LocalInvocationID.y] =
		(gl_LocalInvocationID.y < gl_WorkGroupID.y)
		?
		g_TextureInputLocalScan.mips[0][uint2(gl_GlobalInvocationID.x, 32U * (gl_LocalInvocationID.y + 1U) - 1U)].xyz
		:
		float3(0, 0, 0);

	//存在 gl_LocalInvocationID.y的最大值 小于 gl_WorkGroupID.y的最大值（假定小于64） 的可能性
	s_GlobalReduce[gl_LocalInvocationID.y + 32U] =
		((gl_LocalInvocationID.y + 32U) < gl_WorkGroupID.y)
		?
		g_TextureInputLocalScan.mips[0][uint2(gl_GlobalInvocationID.x, 32U * ((gl_LocalInvocationID.y + 32U) + 1U) - 1U)].xyz
		:
		float3(0, 0, 0);
	GroupMemoryBarrierWithGroupSync();

	//2.并行归约
	//Stride 32
	s_GlobalReduce[gl_LocalInvocationID.y] += s_GlobalReduce[gl_LocalInvocationID.y + 32];
	GroupMemoryBarrierWithGroupSync();
	//Stride 16
	s_GlobalReduce[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y < 16) ? s_GlobalReduce[gl_LocalInvocationID.y + 16] : float3(0.0f, 0.0f, 0.0f);
	GroupMemoryBarrierWithGroupSync();
	//Stride 8
	s_GlobalReduce[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y < 8) ? s_GlobalReduce[gl_LocalInvocationID.y + 8] : float3(0.0f, 0.0f, 0.0f);
	GroupMemoryBarrierWithGroupSync();
	//Stride 4
	s_GlobalReduce[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y < 4) ? s_GlobalReduce[gl_LocalInvocationID.y + 4] : float3(0.0f, 0.0f, 0.0f);
	GroupMemoryBarrierWithGroupSync();
	//Stride 2
	s_GlobalReduce[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y < 2) ? s_GlobalReduce[gl_LocalInvocationID.y + 2] : float3(0.0f, 0.0f, 0.0f);
	GroupMemoryBarrierWithGroupSync();
	//Stride 1
	s_GlobalReduce[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y < 1) ? s_GlobalReduce[gl_LocalInvocationID.y + 1] : float3(0.0f, 0.0f, 0.0f);
	GroupMemoryBarrierWithGroupSync();


	//3.输出结果
	g_TextureOutputGlobalScan[gl_GlobalInvocationID.xy] = float4(
		s_GlobalReduce[0]//所有之前的线程组的所对应的像素的和
		+
		g_TextureInputLocalScan.mips[0][gl_GlobalInvocationID.xy].xyz,//当前线程组所对应的前缀和数组
		1.0f);
}