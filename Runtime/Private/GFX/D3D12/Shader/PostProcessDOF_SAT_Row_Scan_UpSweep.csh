//Mark Harris,Shubhabrata Sengupta,John D. Owens. "Parallel Prefix Sum (Scan) with CUDA." GPU Gems 3 Chapter 39 2007
//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch39.html
//Duane Merrill, Andrew Grimshaw. "Parallel Scan for Stream Architectures." University of Virginia, Department of Computer Science, Technical Report 2009.
//https://libraopen.lib.virginia.edu/public_view/kd17cs85f

//64*NumGroup×WorkDim
Texture2D<float4> g_TextureInput : register(t0);
//NumGroup×WorkDim
RWTexture2D<float4> g_TextureOutputPartialReduce :register(u0);

//SOA(Structure Of Arrays)
groupshared float s_LocalArray_X[64U];
groupshared float s_LocalArray_Y[64U];
groupshared float s_LocalArray_Z[64U];
groupshared float s_LocalArray_W[64U];

[numthreads(32, 1, 1)]
void main(
	uint3 l_LocalID : SV_GroupThreadID,
	uint3 l_GroupID : SV_GroupID,
	uint3 l_GlobalID : SV_DispatchThreadID
)
{
	//1.UpSweep Reduction
	//Two-level Streaming Reduce-Then-Scan [Merrill 2009]
		
	//1-1.并行映射
	//将全局内存缓存到组共享内存
	{
		float4 Texel = g_TextureInput.mips[0][uint2(64U * l_GroupID.x + l_LocalID.x, l_GlobalID.y)].xyzw;
		s_LocalArray_X[l_LocalID.x] = Texel.x;
		s_LocalArray_Y[l_LocalID.x] = Texel.y;
		s_LocalArray_Z[l_LocalID.x] = Texel.z;
		s_LocalArray_W[l_LocalID.x] = Texel.w;
	}
	{
		float4 Texel = g_TextureInput.mips[0][uint2(64U * l_GroupID.x + (32U + l_LocalID.x), l_GlobalID.y)].xyzw;
		s_LocalArray_X[32U + l_LocalID.x] = Texel.x;
		s_LocalArray_Y[32U + l_LocalID.x] = Texel.y;
		s_LocalArray_Z[32U + l_LocalID.x] = Texel.z;
		s_LocalArray_W[32U + l_LocalID.x] = Texel.w;
	}
	GroupMemoryBarrierWithGroupSync();

	//1-2.并行归约
	//Level0
	s_LocalArray_X[l_LocalID.x + 32U] += s_LocalArray_X[l_LocalID.x];
	s_LocalArray_Y[l_LocalID.x + 32U] += s_LocalArray_Y[l_LocalID.x];
	s_LocalArray_Z[l_LocalID.x + 32U] += s_LocalArray_Z[l_LocalID.x];
	s_LocalArray_W[l_LocalID.x + 32U] += s_LocalArray_W[l_LocalID.x];
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 16U)
	{
		s_LocalArray_X[32U + l_LocalID.x + 16U] += s_LocalArray_X[32U + l_LocalID.x];
		s_LocalArray_Y[32U + l_LocalID.x + 16U] += s_LocalArray_Y[32U + l_LocalID.x];
		s_LocalArray_Z[32U + l_LocalID.x + 16U] += s_LocalArray_Z[32U + l_LocalID.x];
		s_LocalArray_W[32U + l_LocalID.x + 16U] += s_LocalArray_W[32U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 8U)
	{
		s_LocalArray_X[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_X[32U + 16U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_Y[32U + 16U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_Z[32U + 16U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_W[32U + 16U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 4U)
	{
		s_LocalArray_X[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_X[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_W[32U + 16U + 8U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 2U)
	{
		s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	if (l_LocalID.x < 1U)
	{
		float4 Texel;
		Texel.x = (s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] + s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x]);
		Texel.y = (s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] + s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x]);
		Texel.z = (s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] + s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x]);
		Texel.w = (s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] + s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x]);
		//将单个结果写入全局内存
		//不保存归约时的中间值而在DownSweep中重新计算
		//以减少全局内存访问
		g_TextureOutputPartialReduce[uint2(l_GroupID.x, l_GlobalID.y)].xyzw = Texel;
	}
}