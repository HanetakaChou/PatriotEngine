//Alexander Reshetov. "Morphological Antialiasing”. Intel Labs 2009.
//Timothy Lottes. "FXAA". Nvidia WhitePaper 2011.


#define ROOTSIGNATURE "\
DescriptorTable(SRV(offset=0,t0,space=0,numdescriptors=1)),\
DescriptorTable(UAV(offset=0,u0,space=0,numdescriptors=1))"

Texture2D<float4> g_TextureInput:register(t0, space0);
RWTexture2D<float4> g_TextureOutput:register(u0, space0);

[RootSignature(ROOTSIGNATURE)]
[numthreads(16, 16, 1)]
void main(uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{
	float l_EdgeThresholdMinimum = 0.021f; //(1/16)/2.96 在Nvidia的FXAA Sample中，FxaaLuma的取值范围是0到2.96
	float l_EdgeThresholdRatio = 0.125f; //1/8
	float l_SearchThreshold = 0.25f;//1.0f/4.0f
	uint l_Quality_PS = 5U;
	uint l_Quality_PN[5] = { 1U,1.5f,2U,4U,12U };

	uint2 l_TextureCoordinate_Middle = gl_GlobalInvocationID.xy;

	//BT T BR
	// L M R
	//BL B BR

	float4 l_RGBA_Middle = g_TextureInput.mips[0][l_TextureCoordinate_Middle];
	float3 l_RGB_Top = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x, l_TextureCoordinate_Middle.y - 1U)].xyz;
	float3 l_RGB_Bottom = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x, l_TextureCoordinate_Middle.y + 1U)].xyz;
	float3 l_RGB_Left = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x - 1U, l_TextureCoordinate_Middle.y)].xyz;
	float3 l_RGB_Right = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x + 1U, l_TextureCoordinate_Middle.y)].xyz;

	//光亮度转换
	float l_Luminance_Middle;
	float l_Luminance_Top;
	float l_Luminance_Bottom;
	float l_Luminance_Left;
	float l_Luminance_Right;
	{
		//在Nvidia的FXAA Sample中 
		//约定TextureInput在LinearRGB颜色空间中
		//后处理的顺序为 色调映射 -> FXAA -> 伽马校正
		
		//http://en.wikipedia.org/wiki/Relative_luminance
		//"Real Time Rendering Third Edtion" P215
		l_Luminance_Middle = l_RGBA_Middle.x*0.212671f + l_RGBA_Middle.y*0.715160f + l_RGBA_Middle.z*0.072169f;
		l_Luminance_Top = l_RGB_Top.x*0.212671f + l_RGB_Top.y*0.715160f + l_RGB_Top.z*0.072169f;
		l_Luminance_Bottom = l_RGB_Bottom.x*0.212671f + l_RGB_Bottom.y*0.715160f + l_RGB_Bottom.z*0.072169f;
		l_Luminance_Left = l_RGB_Left.x*0.212671f + l_RGB_Left.y*0.715160f + l_RGB_Left.z*0.072169f;
		l_Luminance_Right = l_RGB_Right.x*0.212671f + l_RGB_Right.y*0.715160f + l_RGB_Right.z*0.072169f;
	}

	//局部对比度检测
	float l_Luminance_LocalMin = min(l_Luminance_Middle, min(min(l_Luminance_Top, l_Luminance_Bottom), min(l_Luminance_Left, l_Luminance_Right)));
	float l_Luminance_LocalMax = max(l_Luminance_Middle, max(max(l_Luminance_Top, l_Luminance_Bottom), max(l_Luminance_Left, l_Luminance_Right)));
	float l_Luminance_LocalContrast = l_Luminance_LocalMax - l_Luminance_LocalMin;
	float l_EdgeThreshold = max(l_EdgeThresholdMinimum, l_Luminance_LocalMax*l_EdgeThresholdRatio);
	if (l_Luminance_LocalContrast < l_EdgeThreshold)
	{
		//提前退出
		g_TextureOutput[l_TextureCoordinate_Middle] = l_RGBA_Middle;
		return;
	}

	//确定主边(PrimaryEdge)的方向
	//下图中 X表示与当前处理像素颜色相近的像素 +则相反
	//当垂直对比度高时，主边水平，主边可能在T-M或M-B之间
	//+++
	//XXX
	//+++
	//当水平对比度高时，主边垂直，主边可能在L-M或M-R之间
	//+X+
	//+X+
	//+X+

	float3 l_RGB_TopLeft = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x - 1U, l_TextureCoordinate_Middle.y - 1U)].xyz;
	float3 l_RGB_TopRight = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x + 1U, l_TextureCoordinate_Middle.y - 1U)].xyz;
	float3 l_RGB_BottomLeft = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x - 1U, l_TextureCoordinate_Middle.y + 1U)].xyz;
	float3 l_RGB_BottomRight = g_TextureInput.mips[0][uint2(l_TextureCoordinate_Middle.x + 1U, l_TextureCoordinate_Middle.y + 1U)].xyz;

	//光亮度转换
	float l_Luminance_TopLeft;
	float l_Luminance_TopRight;
	float l_Luminance_BottomLeft;
	float l_Luminance_BottomRight;
	{
		l_Luminance_TopLeft = l_RGB_TopLeft.x*0.212671f + l_RGB_TopLeft.y*0.715160f + l_RGB_TopLeft.z*0.072169f;
		l_Luminance_TopRight = l_RGB_TopRight.x*0.212671f + l_RGB_TopRight.y*0.715160f + l_RGB_TopRight.z*0.072169f;
		l_Luminance_BottomLeft = l_RGB_BottomLeft.x*0.212671f + l_RGB_BottomLeft.y*0.715160f + l_RGB_BottomLeft.z*0.072169f;
		l_Luminance_BottomRight = l_RGB_BottomRight.x*0.212671f + l_RGB_BottomRight.y*0.715160f + l_RGB_BottomRight.z*0.072169f;
	}

	bool l_PrimaryEdgeHorizontal;
	{
		float PartialDerivative_Vertical = //垂直对比度高->主边水平
			abs(l_Luminance_TopLeft*0.25f + l_Luminance_BottomLeft*0.25f - l_Luminance_Left*0.5f)
			+ abs(l_Luminance_Top*0.5f + l_Luminance_Bottom*0.5f - l_Luminance_Middle*1.0f)
			+ abs(l_Luminance_TopRight*0.25f + l_Luminance_BottomRight*0.25f - l_Luminance_Right*0.5f);

		float PartialDerivative_Horizontal = //水平对比度高->主边垂直
			abs(l_Luminance_TopLeft*0.25f + l_Luminance_TopRight*0.25f - l_Luminance_Top*0.5f)
			+ abs(l_Luminance_Left*0.5f + l_Luminance_Right*0.5f - l_Luminance_Middle*1.0f)
			+ abs(l_Luminance_BottomLeft*0.25f + l_Luminance_BottomRight*0.25f - l_Luminance_Bottom*0.5f);

		l_PrimaryEdgeHorizontal = PartialDerivative_Vertical > PartialDerivative_Horizontal;
	}

	//确定主边的位置
	//当主边水平时，主边在T-M或M-B中对比度较大的两个像素之间
	//当主边垂直时，主边在L-M或M-R中对比度较大的两个像素之间

	//确定副边（SecondaryEdge）的位置
	//以主边在T-M之间为例
	//下图中 X表示与当前处理像素颜色相近的像素 +则相反
	//可能在T所在行
	//XXX+++++++T
	//XXXXXXXXXXM
	//可能在M所在行
	//++++++++++T
	//+++XXXXXXXM
	//副边与主边垂直
	//在直觉上，不断迭代，计算T所在的行或M所在行中相邻列的像素的对比度，当对比度大于某个某个阈值时，即可确定副边的位置，迭代结束
	//在Nvidia的FXAA Sample中
	//不断迭代时计算(Ti+Mi)/2（迭代列）-(T+M)/2（当前处理像素所在列而不是迭代列的相邻列）
	//阈值为abs(T-M)（主边相邻像素的对比度）*FXAA_SEARCH_THRESHOLD（1.0/4.0）
	//在计算(Ti+Mi)/2时，并没有使用lerp，而是将UV中的V分量设置为_.5并使用线性插值的采样器采样得到

	float l_PrimaryEdgeContrast;
	float l_Luminance_Reference1;
	float l_Luminance_Reference2;
	uint2 l_TextureCoordinate_Reference1;
	uint2 l_TextureCoordinate_Reference2;
	float l_TempPrimaryEdgeContrast1 = abs(l_PrimaryEdgeHorizontal ? l_Luminance_Top - l_Luminance_Middle : l_Luminance_Left - l_Luminance_Middle); //|T-M|或|L-M|
	float l_TempPrimaryEdgeContrast2 = abs(l_PrimaryEdgeHorizontal ? l_Luminance_Bottom - l_Luminance_Middle : l_Luminance_Right - l_Luminance_Middle); //|B-M|或|R-M|

																																						//主边被认为在对比度较大的两个像素之间
	l_PrimaryEdgeContrast = l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? l_TempPrimaryEdgeContrast1 : l_TempPrimaryEdgeContrast2;

	l_Luminance_Reference1 = l_Luminance_Middle;
	l_Luminance_Reference2 =
		l_PrimaryEdgeHorizontal
		? (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? l_Luminance_Top : l_Luminance_Bottom)
		: (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? l_Luminance_Left : l_Luminance_Right);

	l_TextureCoordinate_Reference1 = l_TextureCoordinate_Middle;
	l_TextureCoordinate_Reference2 =
		l_PrimaryEdgeHorizontal
		? (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? uint2(l_TextureCoordinate_Middle.x, l_TextureCoordinate_Middle.y - 1U) : uint2(l_TextureCoordinate_Middle.x, l_TextureCoordinate_Middle.y + 1U))
		: (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? uint2(l_TextureCoordinate_Middle.x - 1U, l_TextureCoordinate_Middle.y) : uint2(l_TextureCoordinate_Middle.x + 1U, l_TextureCoordinate_Middle.y));

	//在Nvidia的FXAA Sample中，在同一次迭代中同时进行正向和负向搜索，以避免分歧分支，但是这违背了纹理缓存的空间局部性
	//由于个别着色器提前break只会产生分歧分支，并不会提升性能；可以考虑去除循环体中的break语句并进行循环展开（[unroll]）
	uint2 l_TextureCoordinate_SecondaryEdgePositive1 = l_TextureCoordinate_Reference1;
	uint2 l_TextureCoordinate_SecondaryEdgePositive2 = l_TextureCoordinate_Reference2;
	uint2 l_TextureCoordinate_SecondaryEdgeNegative1 = l_TextureCoordinate_Reference1;
	uint2 l_TextureCoordinate_SecondaryEdgeNegative2 = l_TextureCoordinate_Reference2;
	uint2 l_Position_Step = l_PrimaryEdgeHorizontal ? uint2(1U, 0U) : uint2(0U, 1U);
	bool l_Done_Positive1 = false;
	bool l_Done_Positive2 = false;
	bool l_Done_Negative1 = false;
	bool l_Done_Negative2 = false;
	float l_PrimaryEdgeContrastTuned = l_PrimaryEdgeContrast*l_SearchThreshold;

	//[unroll]
	for (uint i_p = 0U; i_p < l_Quality_PS; ++i_p)
	{
		if (!l_Done_Positive1 && !l_Done_Positive2)
		{
			l_TextureCoordinate_SecondaryEdgePositive1 += (l_Position_Step*l_Quality_PN[i_p]);
			l_TextureCoordinate_SecondaryEdgePositive2 += (l_Position_Step*l_Quality_PN[i_p]);
			float3 l_RGB_Iterate1 = g_TextureInput.mips[0][l_TextureCoordinate_SecondaryEdgePositive1].xyz;
			float3 l_RGB_Iterate2 = g_TextureInput.mips[0][l_TextureCoordinate_SecondaryEdgePositive2].xyz;
			float l_Luminance_Iterate1;
			float l_Luminance_Iterate2;
			{

				l_Luminance_Iterate1 = l_RGB_Iterate1.x*0.212671f + l_RGB_Iterate1.y*0.715160f + l_RGB_Iterate1.z*0.072169f;
				l_Luminance_Iterate2 = l_RGB_Iterate2.x*0.212671f + l_RGB_Iterate2.y*0.715160f + l_RGB_Iterate2.z*0.072169f;
			}
			l_Done_Positive1 = l_Done_Positive1 || abs(l_Luminance_Iterate1 - l_Luminance_Reference1) >= l_PrimaryEdgeContrastTuned;
			l_Done_Positive2 = l_Done_Positive2 || abs(l_Luminance_Iterate2 - l_Luminance_Reference2) >= l_PrimaryEdgeContrastTuned;
		}
	}

	//[unroll]
	for (uint i_n = 0U; i_n < l_Quality_PS; ++i_n)
	{
		if (!l_Done_Negative1 && !l_Done_Negative2)
		{
			l_TextureCoordinate_SecondaryEdgeNegative1 -= (l_Position_Step*l_Quality_PN[i_n]);
			l_TextureCoordinate_SecondaryEdgeNegative2 -= (l_Position_Step*l_Quality_PN[i_n]);
			float3 l_RGB_Iterate1 = g_TextureInput.mips[0][l_TextureCoordinate_SecondaryEdgeNegative1].xyz;
			float3 l_RGB_Iterate2 = g_TextureInput.mips[0][l_TextureCoordinate_SecondaryEdgeNegative2].xyz;
			float l_Luminance_Iterate1;
			float l_Luminance_Iterate2;
			{

				l_Luminance_Iterate1 = l_RGB_Iterate1.x*0.212671f + l_RGB_Iterate1.y*0.715160f + l_RGB_Iterate1.z*0.072169f;
				l_Luminance_Iterate2 = l_RGB_Iterate2.x*0.212671f + l_RGB_Iterate2.y*0.715160f + l_RGB_Iterate2.z*0.072169f;
			}
			l_Done_Negative1 = l_Done_Negative1 || abs(l_Luminance_Iterate1 - l_Luminance_Reference1) >= l_PrimaryEdgeContrastTuned;
			l_Done_Negative2 = l_Done_Negative2 || abs(l_Luminance_Iterate2 - l_Luminance_Reference2) >= l_PrimaryEdgeContrastTuned;
		}
	}


	float l_PrimaryEdgeLength_Positive = l_PrimaryEdgeHorizontal ? l_TextureCoordinate_SecondaryEdgePositive1.x - l_TextureCoordinate_Reference1.x : l_TextureCoordinate_SecondaryEdgePositive1.y - l_TextureCoordinate_Reference1.y;
	float l_PrimaryEdgeLength_Negative = l_PrimaryEdgeHorizontal ? l_TextureCoordinate_Reference1.x - l_TextureCoordinate_SecondaryEdgeNegative1.x : l_TextureCoordinate_Reference1.y - l_TextureCoordinate_SecondaryEdgeNegative1.y;


	//副边有两条，在正负方向各有一条
	//在Nvidia的FXAA Sample中
	//只有当距离主边较近一侧的副边与M在同一行/列时，才需要融合
	//以主边在T-M之间为例
	//当副边在T所在行时，不需要融合
	//当副边在M所在行时，需要融合

	//！！！
	//这一步非常关键
	//如果不这么做，那么会产生非常明显的锯齿
	//产生上述现象的原因目前尚不明确，但时可能与以下事实存在着一定联系，留给今后的某位大神来解决
	//在Intel的MLAA中，在处理每一个像素时，需要处理4条主边，分别在L-M T-M R-M B-M之间
	//在AMD的MLAA中，在处理每一个像素时，需要处理2条主边
	//在Nvidia的FXAA中，在处理每一个像素时，只需要处理1条主边
	//！！！

	bool l_NoFilter = (l_PrimaryEdgeLength_Positive < l_PrimaryEdgeLength_Negative) ? l_Done_Positive2 : l_Done_Negative2;
	if (l_NoFilter)
	{
		//提前退出
		g_TextureOutput[l_TextureCoordinate_Middle] = l_RGBA_Middle;
		return;
	}

	//L模式（Z或U模式都可以被看作2个L模式）
	//下图中 X表示与当前处理像素颜色相近的像素 +则相反
	//以主边在T-M之间为例（Z模式）
	//++++++++T++++++++++|XXXX
	//   -----------------
	//+++|XXXxMxXXXXXXXXXXXXXX
	//在Nvidia的FXAA Sample中
	//lerp并没有被使用，而是将UV设置为ColorOld和ColorOpposite之间的某个值，并使用线性插值的采样器采样得到

	float l_PrimaryEdgeLength_LittleEndian = min(l_PrimaryEdgeLength_Positive, l_PrimaryEdgeLength_Negative);
	float l_PrimaryEdgeLength = (l_PrimaryEdgeLength_Positive + 1.0f + l_PrimaryEdgeLength_Negative)*0.5f;
	float l_SecendaryEdge = 1.0f;
	float l_UpBase = (l_PrimaryEdgeLength - 1.0f - l_PrimaryEdgeLength_LittleEndian) / l_PrimaryEdgeLength*l_SecendaryEdge;
	float l_DownBase = (l_PrimaryEdgeLength - l_PrimaryEdgeLength_LittleEndian) / l_PrimaryEdgeLength*l_SecendaryEdge;
	float l_Height = 0.5f;
	float l_Trapezoid_Area = (l_UpBase + l_DownBase)*l_Height*0.5f;

	float3 l_RGB_Opposite =
		l_PrimaryEdgeHorizontal
		? (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? l_RGB_Top : l_RGB_Bottom)
		: (l_TempPrimaryEdgeContrast1 > l_TempPrimaryEdgeContrast2 ? l_RGB_Left : l_RGB_Right);

	g_TextureOutput[l_TextureCoordinate_Middle] = float4(lerp(l_RGBA_Middle.xyz, l_RGB_Opposite, l_Trapezoid_Area), l_RGBA_Middle.w);
}