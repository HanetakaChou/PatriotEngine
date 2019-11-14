//Column UpSweep

Texture2D<float4> g_TextureInput:register(t0);
RWTexture2D<float4> g_TextureOutputLocalScan :register(u0);

groupshared float3 s_LocalScan[32];

[numthreads(1, 32, 1)]
void main(
	uint3 gl_LocalInvocationID : SV_GroupThreadID,
	uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{
	//1.并行映射，将全局内存缓存到组共享内存
	s_LocalScan[gl_LocalInvocationID.y] = g_TextureInput.mips[0][gl_GlobalInvocationID.xy].xyz;
	GroupMemoryBarrierWithGroupSync();

	//2.并行扫描
	//Stride 1
	s_LocalScan[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y >= 1) ? s_LocalScan[(gl_LocalInvocationID.y - 1)] : 0.0f;
	GroupMemoryBarrierWithGroupSync();
	//Stride 2
	s_LocalScan[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y >= 2) ? s_LocalScan[(gl_LocalInvocationID.y - 2)] : 0.0f;
	GroupMemoryBarrierWithGroupSync();
	//Stride 4
	s_LocalScan[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y >= 4) ? s_LocalScan[(gl_LocalInvocationID.y - 4)] : 0.0f;
	GroupMemoryBarrierWithGroupSync();
	//Stride 8
	s_LocalScan[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y >= 8) ? s_LocalScan[(gl_LocalInvocationID.y - 8)] : 0.0f;
	GroupMemoryBarrierWithGroupSync();
	//Stride 16
	s_LocalScan[gl_LocalInvocationID.y] += (gl_LocalInvocationID.y >= 16) ? s_LocalScan[(gl_LocalInvocationID.y - 16)] : 0.0f;
	GroupMemoryBarrierWithGroupSync();

	//注：Chunk中的最后一个元素即为PartialReduction

	//3.输出结果
	g_TextureOutputLocalScan[gl_GlobalInvocationID.xy] = float4(s_LocalScan[gl_LocalInvocationID.y], 1.0f);
}