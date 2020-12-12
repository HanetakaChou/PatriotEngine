#include <math.h>

struct PTVectorSIMD
{
	float32x4_t m_Data;
};

struct PTMatrixSIMD
{
	float32x4_t m_Data[4];
};

inline PTVectorSIMD PT_VECTORCALL PTVector4FLoad(const PTVector4F *pSource)
{
	return PTVectorSIMD{ vld1q_f32(&pSource->x) };
}

inline PTVectorSIMD PT_VECTORCALL PTVector4FLoadA(const PTVector4F *pSource)
{
	return ::PTVector4FLoad(pSource);
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDPerspectiveFovRH(float FovAngleY, float AspectHByW, float NearZ, float FarZ, PTMatrixSIMD *pInverse)
{
	//投影面为z=1.0f
	float HalfViewHeight = ::tanf(0.5f * FovAngleY);
	float HalfViewWidth = HalfViewHeight * AspectHByW;

	return ::PTMatrix4x4SIMDPerspectiveRH(HalfViewWidth*2.0f, HalfViewHeight*2.0f, NearZ, FarZ, pInverse);
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDPerspectiveRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ, PTMatrixSIMD *pInverse)
{
	//Nvidia Reversed-Z Trick
	//http://developer.nvidia.com/content/depth-precision-visualized

	// _  0  0  0
	// 0  _  0  0
	// 0  0  b  a
	// 0  0 -1  0

	//Inverse
	//1/_  0   0   0
	// 0  1/_  0   0
	// 0   0   0   -1
	// 0   0  1/a b/a

	float32x4_t vZero = vdupq_n_f32(0.0f);

	if (pInverse)
	{
		float _1_Div_a = (FarZ - NearZ) / FarZ; //确保FarZ趋向于正无穷大时的正确性
		_1_Div_a /= NearZ;
		float _b_Div_a = 1.0f / FarZ;

		alignas(16) float fTempInverse[2] = { _1_Div_a,_b_Div_a };

		PTMatrixSIMD mInverse;
		//r0
		mInverse.m_Data[0] = vsetq_lane_f32(ViewWidth / 2.0f, vZero, 0);//Width/2 0 0 0
		//r1
		mInverse.m_Data[1] = vsetq_lane_f32(ViewHeight / 2.0f, vZero, 1);//0 Height/2 0 0
		//r2
		mInverse.m_Data[2] = vsetq_lane_f32(-1, vZero, 3);//0 0 0 -1
		//r3
		mInverse.m_Data[3] = vcombine_f32(vdup_n_f32(0), vld1_f32(fTempInverse));//0 0 1/a b/a

		(*pInverse) = mInverse;
	}

	float b = NearZ / (FarZ - NearZ);
	float a = FarZ / (FarZ - NearZ); //确保FarZ趋向于正无穷大时的正确性
	a *= NearZ;

	alignas(16) float fTemp[2] = { b,a };

	PTMatrixSIMD mResult;
	//r0
	mResult.m_Data[0] = vsetq_lane_f32(2.0f / ViewWidth, vZero, 0);//2/Width 0 0 0
	//r1
	mResult.m_Data[1] = vsetq_lane_f32(2.0f / ViewHeight, vZero, 1);//0 2/Height 0 0
	//r2
	mResult.m_Data[2] = vcombine_f32(vdup_n_f32(0), vld1_f32(fTemp));//0 0 b a
	//r3
	mResult.m_Data[3] = vsetq_lane_f32(-1, vZero, 2);//0 0 -1 0

	return mResult;
}