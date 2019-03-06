//Mark Harris,Shubhabrata Sengupta,John D. Owens. "Parallel Prefix Sum (Scan) with CUDA." GPU Gems 3 Chapter 39 2007
//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch39.html
//Duane Merrill, Andrew Grimshaw. "Parallel Scan for Stream Architectures." University of Virginia, Department of Computer Science, Technical Report 2009.
//https://libraopen.lib.virginia.edu/public_view/kd17cs85f

#define ROOTSIGNATURE "\
DescriptorTable(SRV(offset=0,t0,space=0,numdescriptors=1)),\
DescriptorTable(SRV(offset=0,t0,space=1,numdescriptors=1)),\
DescriptorTable(UAV(offset=0,u0,space=0,numdescriptors=1))"

//512×NumGroup
Buffer<float> g_BufferInput : register(t0, space0);

//NumGroup
RWBuffer<float> g_BufferPartialReduce_Write :register(u0, space0);

//NumGroup
Buffer<float> g_BufferPartialReduce_Read :register(t0, space1);

//512×NumGroup
RWBuffer<float> g_BufferOutputGlobalScan : register(u0, space0);

//假定NumGroup不大于512
//SOA(Structure Of Arrays)
groupshared float s_BufferLocalShared[512];


[RootSignature(ROOTSIGNATURE)]
[numthreads(256, 1, 1)]
void Main_UpSweep(
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
		float Temp = g_BufferInput[512U * l_GroupID.x + l_LocalID.x];
		s_BufferLocalShared[l_LocalID.x] = Temp;
	}
	{
		float Temp = g_BufferInput[512U * l_GroupID.x + (256U + l_LocalID.x)];
		s_BufferLocalShared[256U + l_LocalID.x] = Temp;
	}
	GroupMemoryBarrierWithGroupSync();

	//1-2.并行归约
	//Level0
	{
		float TempA = s_BufferLocalShared[l_LocalID.x + 256U];
		float TempB = s_BufferLocalShared[l_LocalID.x];
		s_BufferLocalShared[l_LocalID.x + 256U] = TempA + TempB;
		GroupMemoryBarrierWithGroupSync();
	}
	//Level1
	if (l_LocalID.x < 128U)
	{
		float TempA = s_BufferLocalShared[256U + l_LocalID.x + 128U];
		float TempB = s_BufferLocalShared[256U + l_LocalID.x];
		s_BufferLocalShared[256U + l_LocalID.x + 128U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 64U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U];
		float TempB = s_BufferLocalShared[256U + 128U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 32U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 16U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	if (l_LocalID.x < 8U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level6
	if (l_LocalID.x < 4U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level7
	if (l_LocalID.x < 2U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level8
	if (l_LocalID.x < 1U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x];

		//1-3.将单个结果写入全局内存
		//不保存归约时的中间值而在DownSweep中重新计算
		//以减少全局内存访问
		g_BufferPartialReduce_Write[l_GroupID.x] = TempA + TempB;
	}
}

[RootSignature(ROOTSIGNATURE)]
[numthreads(256, 1, 1)]
void Main_DownSweep(
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
		float Temp = (l_LocalID.x < l_GroupID.x) ? g_BufferPartialReduce_Read[l_LocalID.x] : 0.0f;
		s_BufferLocalShared[l_LocalID.x] = Temp;
	}
	//存在LocalSize小于NumGroup（假定不大于512）的可能性
	{
		float Temp = ((256U + l_LocalID.x) < l_GroupID.x) ? g_BufferPartialReduce_Read[256U + l_LocalID.x] : 0.0f;
		s_BufferLocalShared[256U + l_LocalID.x] = Temp;
	}
	GroupMemoryBarrierWithGroupSync();

	//2-2.并行归约
	//s_BufferLocalShared[511]计算各个“之前的/*Preceding*/”线程组的局部归约的归约
	//Level0
	{
		float TempA = s_BufferLocalShared[l_LocalID.x + 256U];
		float TempB = s_BufferLocalShared[l_LocalID.x];
		s_BufferLocalShared[l_LocalID.x + 256U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 128U)
	{
		float TempA = s_BufferLocalShared[256U + l_LocalID.x + 128U];
		float TempB = s_BufferLocalShared[256U + l_LocalID.x];
		s_BufferLocalShared[256U + l_LocalID.x + 128U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 64U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U];
		float TempB = s_BufferLocalShared[256U + 128U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 32U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 16U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	if (l_LocalID.x < 8U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level6
	if (l_LocalID.x < 4U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level7
	if (l_LocalID.x < 2U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level8
	if (l_LocalID.x < 1U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] = TempA + TempB;
	}
	//GroupMemoryBarrierWithGroupSync();
	//DownSweep Bottom-Level Scan 中 只有 Thread LocalID 0 访问 s_BufferLocalShared[511]

	//3.DownSweep Bottom-Level Scan
	//Two-level Streaming Reduce-Then-Scan [Merrill 2009] 

	//Permuted-Scan [Merrill 2009] //(Bank)Conflict-Free and Padding-Free

	//倒位序（出自FFT）
	//该方法为本人原创，在论文原文中作者尚未解决
	uint PermutedIndex_Local = reversebits(l_LocalID.x) >> 23U; //23:=32-log(2,512) 512为局部共享内存维度 
	uint PermutedIndex_LocalPlus256 = reversebits(256U + l_LocalID.x) >> 23U;

	//3-1.并行映射
	//将全局内存缓存到组共享内存
	float Element_PermutedIndex_Local;
	float Element_PermutedIndex_LocalPlus256;
	{
		//Global Memory Coalescing
		//https://devblogs.nvidia.com/how-access-global-memory-efficiently-cuda-c-kernels/
		Element_PermutedIndex_Local = g_BufferInput[512U * l_GroupID.x + l_LocalID.x];
		Element_PermutedIndex_LocalPlus256 = g_BufferInput[512U * l_GroupID.x + (256U + l_LocalID.x)];

		s_BufferLocalShared[PermutedIndex_Local] = Element_PermutedIndex_Local;
		if (l_LocalID.x < 255U)
		{
			//没有必要访问s_BufferLocalShared[511]，下同
			s_BufferLocalShared[PermutedIndex_LocalPlus256] = Element_PermutedIndex_LocalPlus256;
		}
		GroupMemoryBarrierWithGroupSync();
		
		Element_PermutedIndex_Local = s_BufferLocalShared[l_LocalID.x];
		if (l_LocalID.x < 255U)
		{
			//当l_LocalID.x等于255时
			//值PermutedIndex_LocalPlus256和值256U+l_LocalID.x是相同的
			Element_PermutedIndex_LocalPlus256 = s_BufferLocalShared[256U + l_LocalID.x];
		}
	}
	//GroupMemoryBarrierWithGroupSync(); //Have Sync Above

	//3-2.UpSweep
	//Level0
	if (l_LocalID.x < 255U)
	{
		float TempA = s_BufferLocalShared[l_LocalID.x + 256U];
		float TempB = s_BufferLocalShared[l_LocalID.x];
		s_BufferLocalShared[l_LocalID.x + 256U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 127U)
	{
		float TempA = s_BufferLocalShared[256U + l_LocalID.x + 128U];
		float TempB = s_BufferLocalShared[256U + l_LocalID.x];
		s_BufferLocalShared[256U + l_LocalID.x + 128U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 63U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U];
		float TempB = s_BufferLocalShared[256U + 128U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 31U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 15U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	if (l_LocalID.x < 7U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level6
	if (l_LocalID.x < 3U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();

	//3-3.DownSweep
	//Level7(UpSweep)||Level0(DownSweep)
	if (l_LocalID.x < 1U)
	{
		//Level4(UpSweep)
		{
			float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U];
			float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x];
			s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U] = TempA + TempB;
		}

		//Level0(DownSweep)
		{
			float TempSeed = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U];//s_BufferLocalShared[511]计算各个“之前的/*Preceding*/”线程组的局部归约的归约
			float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x];
			s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x] = TempSeed;
			s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + 2U + l_LocalID.x + 1U] = TempSeed + TempB;
		}
	}
	GroupMemoryBarrierWithGroupSync();
	//Level1
	if (l_LocalID.x < 2U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + 4U + l_LocalID.x + 2U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level2
	if (l_LocalID.x < 4U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + 8U + l_LocalID.x + 4U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level3
	if (l_LocalID.x < 8U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + 64U + 32U + 16U + l_LocalID.x + 8U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level4
	if (l_LocalID.x < 16U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + 64U + 32U + l_LocalID.x + 16U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level5
	if (l_LocalID.x < 32U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U];
		float TempB = s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + 64U + l_LocalID.x + 32U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level6
	if (l_LocalID.x < 64U)
	{
		float TempA = s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U];
		float TempB = s_BufferLocalShared[256U + 128U + l_LocalID.x];
		s_BufferLocalShared[256U + 128U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + 128U + l_LocalID.x + 64U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level7
	if (l_LocalID.x < 128U)
	{
		float TempA = s_BufferLocalShared[256U + l_LocalID.x + 128U];
		float TempB = s_BufferLocalShared[256U + l_LocalID.x];
		s_BufferLocalShared[256U + l_LocalID.x] = TempA;
		s_BufferLocalShared[256U + l_LocalID.x + 128U] = TempA + TempB;
	}
	GroupMemoryBarrierWithGroupSync();
	//Level8
	{
		float TempA = s_BufferLocalShared[l_LocalID.x + 256U];
		float TempB = s_BufferLocalShared[l_LocalID.x];
		//Exclusive Scan To Inclusive Scan
		s_BufferLocalShared[l_LocalID.x] = TempA + Element_PermutedIndex_Local;
		s_BufferLocalShared[l_LocalID.x + 256U] = TempA + TempB + Element_PermutedIndex_LocalPlus256;
	}
	GroupMemoryBarrierWithGroupSync();

	//3-4.并行映射
	//将组共享内存存储到全局内存
	//Global Memory Coalescing
	//https://devblogs.nvidia.com/how-access-global-memory-efficiently-cuda-c-kernels/
	g_BufferOutputGlobalScan[512U * l_GroupID.x + l_LocalID.x] = s_BufferLocalShared[PermutedIndex_Local];
	g_BufferOutputGlobalScan[512U * l_GroupID.x + 256U + l_LocalID.x] = s_BufferLocalShared[PermutedIndex_LocalPlus256];
}