//Mark Harris,Shubhabrata Sengupta,John D. Owens. "Parallel Prefix Sum (Scan) with CUDA." GPU Gems 3 Chapter 39 2007
//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch39.html
//Duane Merrill, Andrew Grimshaw. "Parallel Scan for Stream Architectures." University of Virginia, Department of Computer Science, Technical Report 2009.
//https://libraopen.lib.virginia.edu/public_view/kd17cs85f


//NumGroup×WorkDim
Texture2D<float4> g_TextureInput : register(t0);
Texture2D<float4> g_TextureInputPartialReduce:register(t1);

//64*NumGroup×WorkDim
RWTexture2D<float4> g_TextureOutputScan : register(u0);

//假定NumGroup不大于64
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
	//2.DownSweep Top-Level Scan //原文中为Scan，本人认为应当为Reduce
	//Two-level Streaming Reduce-Then-Scan [Merrill 2009]

	//2-1.并行映射
	//将各个“之前的/*Preceding*/”线程组的局部归约缓存到局部内存
	{
		float4 Texel = (l_LocalID.x < l_GroupID.x) ? g_TextureInputPartialReduce.mips[0][uint2(l_LocalID.x, l_GlobalID.y)].xyzw : float4(0.0f, 0.0f, 0.0f, 0.0f);
		s_LocalArray_X[l_LocalID.x] = Texel.x;
		s_LocalArray_Y[l_LocalID.x] = Texel.y;
		s_LocalArray_Z[l_LocalID.x] = Texel.z;
		s_LocalArray_W[l_LocalID.x] = Texel.w;
	}
	//存在LocalSize小于NumGroup（假定不大于64）的可能性
	{
		float4 Texel = ((32U + l_LocalID.x) < l_GroupID.x) ? g_TextureInputPartialReduce.mips[0][uint2(32U + l_LocalID.x, l_GlobalID.y)].xyzw : float4(0.0f, 0.0f, 0.0f, 0.0f);
		s_LocalArray_X[32U + l_LocalID.x] = Texel.x;
		s_LocalArray_Y[32U + l_LocalID.x] = Texel.y;
		s_LocalArray_Z[32U + l_LocalID.x] = Texel.z;
		s_LocalArray_W[32U + l_LocalID.x] = Texel.w;
	}
	GroupMemoryBarrierWithGroupSync();


	//2-2.并行归约
	//s_LocalArray_*[63]计算各个“之前的/*Preceding*/”线程组的局部归约的归约
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
		s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
	}
	//GroupMemoryBarrierWithGroupSync();
	//DownSweep Bottom-Level Scan 中 只有 Thread LocalID 0 访问 s_LocalArray_*[63]

	//3.DownSweep Bottom-Level Scan
	//Two-level Streaming Reduce-Then-Scan [Merrill 2009] 

	//Permuted-Scan [Merrill 2009] //(Bank)Conflict-Free and Padding-Free

	//倒位序（出自FFT）
	//该方法为本人原创，在论文原文中作者尚未解决
	uint PermutedIndex_Local = reversebits(l_LocalID.x) >> 26U; //26:=32-log(2,64) 64为局部共享内存维度 
	uint PermutedIndex_LocalPlus32 = reversebits(32U + l_LocalID.x) >> 26U;

	//3-1.并行映射
	//将全局内存缓存到组共享内存
	{
		float4 Texel = g_TextureInput.mips[0][uint2(64U * l_GroupID.x + l_LocalID.x, l_GlobalID.y)].xyzw;
		s_LocalArray_X[PermutedIndex_Local] = Texel.x;
		s_LocalArray_Y[PermutedIndex_Local] = Texel.y;
		s_LocalArray_Z[PermutedIndex_Local] = Texel.z;
		s_LocalArray_W[PermutedIndex_Local] = Texel.w;
	}
	//没有必要访问s_LocalArray_*[63]，下同
	if (l_LocalID.x < 31U)
	{
		float4 Texel = g_TextureInput.mips[0][uint2(64U * l_GroupID.x + 32U + l_LocalID.x, l_GlobalID.y)].xyzw;
		s_LocalArray_X[PermutedIndex_LocalPlus32] = Texel.x;
		s_LocalArray_Y[PermutedIndex_LocalPlus32] = Texel.y;
		s_LocalArray_Z[PermutedIndex_LocalPlus32] = Texel.z;
		s_LocalArray_W[PermutedIndex_LocalPlus32] = Texel.w;
	}
	GroupMemoryBarrierWithGroupSync();

	//3-2.UpSweep
	//Level0
	if (l_LocalID.x < 31U)
	{
		s_LocalArray_X[l_LocalID.x + 32U] += s_LocalArray_X[l_LocalID.x];
		s_LocalArray_Y[l_LocalID.x + 32U] += s_LocalArray_Y[l_LocalID.x];
		s_LocalArray_Z[l_LocalID.x + 32U] += s_LocalArray_Z[l_LocalID.x];
		s_LocalArray_W[l_LocalID.x + 32U] += s_LocalArray_W[l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 15U)
	{
		s_LocalArray_X[32U + l_LocalID.x + 16U] += s_LocalArray_X[32U + l_LocalID.x];
		s_LocalArray_Y[32U + l_LocalID.x + 16U] += s_LocalArray_Y[32U + l_LocalID.x];
		s_LocalArray_Z[32U + l_LocalID.x + 16U] += s_LocalArray_Z[32U + l_LocalID.x];
		s_LocalArray_W[32U + l_LocalID.x + 16U] += s_LocalArray_W[32U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 7U)
	{
		s_LocalArray_X[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_X[32U + 16U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_Y[32U + 16U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_Z[32U + 16U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + l_LocalID.x + 8U] += s_LocalArray_W[32U + 16U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 3U)
	{
		s_LocalArray_X[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_X[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + 8U + l_LocalID.x + 4U] += s_LocalArray_W[32U + 16U + 8U + l_LocalID.x];
	}
	GroupMemoryBarrierWithGroupSync();
	
	//3-3.DownSweep
	//Level4(UpSweep)||Level0(DownSweep)
	if (l_LocalID.x < 1U)
	{
		//Level4(UpSweep)
		s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x];
		s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x];
	
		//Level0(DownSweep)
		float Temp_X = s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		float Temp_W = s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		
		//s_LocalArray_*[63]计算各个“之前的/*Preceding*/”线程组的局部归约的归约
		s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x] = s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];
		s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x] = s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];
		s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x] = s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];
		s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x] = s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];

		s_LocalArray_X[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += Temp_X;
		s_LocalArray_Y[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += Temp_Y;
		s_LocalArray_Z[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += Temp_Z;
		s_LocalArray_W[32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] += Temp_W;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 2U)
	{
		float Temp_X = s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x];
		float Temp_W = s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x];

		s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x] = s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x] = s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x] = s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x] = s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x + 2U];

		s_LocalArray_X[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += Temp_X;
		s_LocalArray_Y[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += Temp_Y;
		s_LocalArray_Z[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += Temp_Z;
		s_LocalArray_W[32U + 16U + 8U + 4U + l_LocalID.x + 2U] += Temp_W;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 4U)
	{
		float Temp_X = s_LocalArray_X[32U + 16U + 8U + l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x];
		float Temp_W = s_LocalArray_W[32U + 16U + 8U + l_LocalID.x];

		s_LocalArray_X[32U + 16U + 8U + l_LocalID.x] = s_LocalArray_X[32U + 16U + 8U + l_LocalID.x + 4U];
		s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x] = s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x + 4U];
		s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x] = s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x + 4U];
		s_LocalArray_W[32U + 16U + 8U + l_LocalID.x] = s_LocalArray_W[32U + 16U + 8U + l_LocalID.x + 4U];

		s_LocalArray_X[32U + 16U + 8U + l_LocalID.x + 4U] += Temp_X;
		s_LocalArray_Y[32U + 16U + 8U + l_LocalID.x + 4U] += Temp_Y;
		s_LocalArray_Z[32U + 16U + 8U + l_LocalID.x + 4U] += Temp_Z;
		s_LocalArray_W[32U + 16U + 8U + l_LocalID.x + 4U] += Temp_W;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 8U)
	{
		float Temp_X = s_LocalArray_X[32U + 16U + l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[32U + 16U + l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[32U + 16U + l_LocalID.x];
		float Temp_W = s_LocalArray_W[32U + 16U + l_LocalID.x];

		s_LocalArray_X[32U + 16U + l_LocalID.x] = s_LocalArray_X[32U + 16U + l_LocalID.x + 8U];
		s_LocalArray_Y[32U + 16U + l_LocalID.x] = s_LocalArray_Y[32U + 16U + l_LocalID.x + 8U];
		s_LocalArray_Z[32U + 16U + l_LocalID.x] = s_LocalArray_Z[32U + 16U + l_LocalID.x + 8U];
		s_LocalArray_W[32U + 16U + l_LocalID.x] = s_LocalArray_W[32U + 16U + l_LocalID.x + 8U];

		s_LocalArray_X[32U + 16U + l_LocalID.x + 8U] += Temp_X; 
		s_LocalArray_Y[32U + 16U + l_LocalID.x + 8U] += Temp_Y; 
		s_LocalArray_Z[32U + 16U + l_LocalID.x + 8U] += Temp_Z; 
		s_LocalArray_W[32U + 16U + l_LocalID.x + 8U] += Temp_W; 
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 16U)
	{
		float Temp_X = s_LocalArray_X[32U + l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[32U + l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[32U + l_LocalID.x];
		float Temp_W = s_LocalArray_W[32U + l_LocalID.x];

		s_LocalArray_X[32U + l_LocalID.x] = s_LocalArray_X[32U + l_LocalID.x + 16U];
		s_LocalArray_Y[32U + l_LocalID.x] = s_LocalArray_Y[32U + l_LocalID.x + 16U];
		s_LocalArray_Z[32U + l_LocalID.x] = s_LocalArray_Z[32U + l_LocalID.x + 16U];
		s_LocalArray_W[32U + l_LocalID.x] = s_LocalArray_W[32U + l_LocalID.x + 16U];

		s_LocalArray_X[32U + l_LocalID.x + 16U] += Temp_X;
		s_LocalArray_Y[32U + l_LocalID.x + 16U] += Temp_Y;
		s_LocalArray_Z[32U + l_LocalID.x + 16U] += Temp_Z;
		s_LocalArray_W[32U + l_LocalID.x + 16U] += Temp_W;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	{
		float Temp_X = s_LocalArray_X[l_LocalID.x];
		float Temp_Y = s_LocalArray_Y[l_LocalID.x];
		float Temp_Z = s_LocalArray_Z[l_LocalID.x];
		float Temp_W = s_LocalArray_W[l_LocalID.x];

		s_LocalArray_X[l_LocalID.x] = s_LocalArray_X[l_LocalID.x + 32U];
		s_LocalArray_Y[l_LocalID.x] = s_LocalArray_Y[l_LocalID.x + 32U];
		s_LocalArray_Z[l_LocalID.x] = s_LocalArray_Z[l_LocalID.x + 32U];
		s_LocalArray_W[l_LocalID.x] = s_LocalArray_W[l_LocalID.x + 32U];

		s_LocalArray_X[l_LocalID.x + 32U] += Temp_X;
		s_LocalArray_Y[l_LocalID.x + 32U] += Temp_Y;
		s_LocalArray_Z[l_LocalID.x + 32U] += Temp_Z;
		s_LocalArray_W[l_LocalID.x + 32U] += Temp_W;
	}
	GroupMemoryBarrierWithGroupSync();

	//3-4.并行映射
	//将组共享内存存储到全局内存
	{
		float4 Texel;
		Texel.x = s_LocalArray_X[PermutedIndex_Local];
		Texel.y = s_LocalArray_Y[PermutedIndex_Local];
		Texel.z = s_LocalArray_Z[PermutedIndex_Local];
		Texel.w = s_LocalArray_W[PermutedIndex_Local];
		g_TextureOutputScan[uint2(64U * l_GroupID.x + l_LocalID.x, l_GlobalID.y)].xyzw = Texel;
	}
	{
		float4 Texel;
		Texel.x = s_LocalArray_X[PermutedIndex_LocalPlus32];
		Texel.y = s_LocalArray_Y[PermutedIndex_LocalPlus32];
		Texel.z = s_LocalArray_Z[PermutedIndex_LocalPlus32];
		Texel.w = s_LocalArray_W[PermutedIndex_LocalPlus32];
		g_TextureOutputScan[uint2(64U * l_GroupID.x + 32U + l_LocalID.x, l_GlobalID.y)].xyzw = Texel;
	}
}