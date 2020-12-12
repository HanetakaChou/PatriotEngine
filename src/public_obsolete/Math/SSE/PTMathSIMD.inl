//--------------------------------------------------
// Author: 张羽乔
// License: LGPL 3
//--------------------------------------------------

#include <math.h>
#include <float.h>
#include <stdint.h>
#include <assert.h>

//线性代数

struct PTVectorSIMD
{
	__m128 m_Data;
};

struct PTQuaternionSIMD
{
	__m128 m_Data;
};

struct PTDualQuaternionSIMD
{
	__m128 m_Data[2];
};

struct PTMatrixSIMD
{
	__m128 m_Data[4];
};

inline PTVectorSIMD PT_VECTORCALL PTVector2FLoad(const PTVector2F *pSource)
{
	__m128 vResult = _mm_loadl_pi/*x y _ _*/(_mm_setzero_ps(), reinterpret_cast<__m64 const*>(&pSource->x));
	return  PTVectorSIMD{ vResult };
}

inline void PT_VECTORCALL PTVector2FStore(PTVector2F* pDestination, PTVectorSIMD V)
{
	_mm_storel_pi(reinterpret_cast<__m64*>(&pDestination->x), V.m_Data);
}

inline PTVectorSIMD PT_VECTORCALL PTVector3FLoad(const PTVector3F *pSource)
{
	__m128 vResult = _mm_loadl_pi/*x y z _*/(_mm_load_ps1/*_ _ z _*/(&pSource->z), reinterpret_cast<__m64 const*>(&pSource->x));
	return PTVectorSIMD{ vResult };
}

inline void PT_VECTORCALL PTVector3FStore(PTVector3F* pDestination, PTVectorSIMD V)
{
	_mm_storel_pi(reinterpret_cast<__m64*>(&pDestination->x), V.m_Data);
	_mm_store_ss(&pDestination->z, _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(2, 2, 2, 2)));
}

inline PTVectorSIMD PT_VECTORCALL PTVector4FLoad(const PTVector4F *pSource)
{
	__m128 vResult = ((reinterpret_cast<uintptr_t>(pSource) & 0XFU) != 0U) ? _mm_loadu_ps(&pSource->x) : _mm_load_ps(&pSource->x);
	return PTVectorSIMD{ vResult };
}

inline void PT_VECTORCALL PTVector4FStore(PTVector4F* pDestination, PTVectorSIMD V)
{
	((reinterpret_cast<uintptr_t>(pDestination) & 0XFU) != 0U) ? _mm_storeu_ps(&pDestination->x, V.m_Data) : _mm_store_ps(&pDestination->x, V.m_Data);
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDReplicate(float Value)
{
	return PTVectorSIMD{ _mm_load_ps1(&Value) };
}

inline float PT_VECTORCALL PTVectorSIMDGetX(PTVectorSIMD V)
{
	return _mm_cvtss_f32(V.m_Data);
}

inline float PT_VECTORCALL PTVectorSIMDGetY(PTVectorSIMD V)
{
	__m128 vTemp = _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(1, 1, 1, 1));
	return _mm_cvtss_f32(vTemp);
}

inline float PT_VECTORCALL PTVectorSIMDGetZ(PTVectorSIMD V)
{
	__m128 vTemp = _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(2, 2, 2, 2));
	return _mm_cvtss_f32(vTemp);
}

inline float PT_VECTORCALL PTVectorSIMDGetW(PTVectorSIMD V)
{
	__m128 vTemp = _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(3, 3, 3, 3));
	return _mm_cvtss_f32(vTemp);
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDNegate(PTVectorSIMD V)
{
	__m128 vZero = ::_mm_setzero_ps();
	__m128 vResult = ::_mm_sub_ps(vZero, V.m_Data);
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDAdd(PTVectorSIMD V1, PTVectorSIMD V2)
{
	return PTVectorSIMD{ _mm_add_ps(V1.m_Data,V2.m_Data) };
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDSubtract(PTVectorSIMD V1, PTVectorSIMD V2)
{
	return PTVectorSIMD{ _mm_sub_ps(V1.m_Data,V2.m_Data) };
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDMultiply(PTVectorSIMD V1, PTVectorSIMD V2)
{
	return PTVectorSIMD{ _mm_mul_ps(V1.m_Data,V2.m_Data) };
}

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDLess(PTVectorSIMD V1, PTVectorSIMD V2)
{
	return PTVectorSIMD{ _mm_cmplt_ps(V1.m_Data,V2.m_Data) };
}

inline bool PT_VECTORCALL PTVector2SIMDToBool(PTVectorSIMD V)
{
	return (_mm_movemask_ps(V.m_Data) & 3) == 3;
}

inline bool PT_VECTORCALL PTVector3SIMDToBool(PTVectorSIMD V)
{
	return (_mm_movemask_ps(V.m_Data) & 7) == 7;
}

inline bool PT_VECTORCALL PTVector4SIMDToBool(PTVectorSIMD V)
{
	return _mm_movemask_ps(V.m_Data) == 15;
}

inline PTVectorSIMD PT_VECTORCALL PTVector2SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2)
{
	//映射
	__m128 vTemp1 = _mm_mul_ps(V1.m_Data, V2.m_Data);
	//归约
	__m128 vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 1));//y _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+y _ _ _ 注意是ss而不是ps!!!

	__m128 vResult = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 0));//x+z+y+w x+z+y+w x+z+y+w x+z+y+w
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2)
{
	//映射
	__m128 vTemp1 = _mm_mul_ps(V1.m_Data, V2.m_Data);
	//归约
	__m128 vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 2));//z _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+z y _ _ 注意是ss而不是ps!!!
	vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 1));//y _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+z+y _ _ _
	
	__m128 vResult = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 0));//x+z+y+w x+z+y+w x+z+y+w x+z+y+w
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVector4SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2)
{
	//映射
	__m128 vTemp1 = _mm_mul_ps(V1.m_Data, V2.m_Data);
	//归约
	__m128 vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 3, 2));//z w _ _
	vTemp1 = ::_mm_add_ps(vTemp1, vTemp2);//x+z y+w _ _ 
	vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 1));//y+w _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+z+y+w _ _ _ 注意是ss而不是ps!!!

	__m128 vResult = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 0));//x+z+y+w x+z+y+w x+z+y+w x+z+y+w
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDCross(PTVectorSIMD V1, PTVectorSIMD V2)
{
	__m128 vResult = _mm_sub_ps(//y1z2-z1y2 z1x2-x1z2 x1y2-y1x2
		::_mm_mul_ps(//y1z2 z1x2 x1y2
			_mm_shuffle_ps(V1.m_Data, V1.m_Data, _MM_SHUFFLE(0, 0, 2, 1)),//y1 z1 x1 _
			_mm_shuffle_ps(V2.m_Data, V2.m_Data, _MM_SHUFFLE(0, 1, 0, 2))//z2 x2 y2 _
		),
		::_mm_mul_ps(//z1y2 x1z2 y1x2
			_mm_shuffle_ps(V1.m_Data, V1.m_Data, _MM_SHUFFLE(0, 1, 0, 2)),//z1 x1 y1 _
			_mm_shuffle_ps(V2.m_Data, V2.m_Data, _MM_SHUFFLE(0, 0, 2, 1))//y2 z2 x2 _
		)
	);
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDNormalize(PTVectorSIMD V)
{
	__m128 vLength = ::_mm_sqrt_ps((::PTVector3SIMDDot(V, V)).m_Data);
	//零向量的方向是任意的！！！
	__m128 vResult = ::_mm_div_ps(V.m_Data, vLength);
	return PTVectorSIMD{ vResult };
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransformCoord(PTMatrixSIMD M, PTVectorSIMD V)
{
	PTMatrixSIMD mTranspose = ::PTMatrix4x4SIMDTranspose(M);

	return PTVectorSIMD{
		::_mm_add_ps(
			::_mm_add_ps(
				::_mm_mul_ps(mTranspose.m_Data[0], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(0, 0, 0, 0))), //C0*X
				::_mm_mul_ps(mTranspose.m_Data[1], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(1, 1, 1, 1))) //C1*Y
			),
			::_mm_add_ps(
				::_mm_mul_ps(mTranspose.m_Data[2], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(2, 2, 2, 2))), //C2*Z
				mTranspose.m_Data[3] //C3*W(1.0f)
			)
		)
	};
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransformTangent(PTMatrixSIMD M, PTVectorSIMD V)
{
	PTMatrixSIMD mTranspose = ::PTMatrix4x4SIMDTranspose(M);

	return PTVectorSIMD{
		::_mm_add_ps(
			::_mm_add_ps(
				::_mm_mul_ps(mTranspose.m_Data[0], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(0, 0, 0, 0))), //C0*X
				::_mm_mul_ps(mTranspose.m_Data[1], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(1, 1, 1, 1))) //C1*Y
			),
			::_mm_mul_ps(mTranspose.m_Data[2], _mm_shuffle_ps(V.m_Data, V.m_Data, _MM_SHUFFLE(2, 2, 2, 2))) //C2*Z
		)
	};
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransform(PTQuaternionSIMD qR, PTVectorSIMD vPosition)
{
	//vP = vP + cross(qR.xyz, cross(qR.xyz, vP.xyz) + vP.xyz*qR.www) *2 
	alignas(16) float f1110[4] = { 1.0f,1.0f,1.0f,0.0f };
	float f2 = 2.0f;
	return PTVectorSIMD{ ::_mm_add_ps(
		vPosition.m_Data, //vP + cross(qR.xyz, cross(qR.xyz, vP.xyz) + vP.xyz*qR.www) * 2
		_mm_mul_ps( //cross(qR.xyz, cross(qR.xyz, vP.xyz) + vP.xyz*qR.www) *2
			::PTVector3SIMDCross( //cross(qR.xyz, vP.xyz) + vP.xyz*qR.www
				PTVectorSIMD{ qR.m_Data }, //qR.xyz
				PTVectorSIMD{ _mm_add_ps( //cross(qR.xyz, vP.xyz) + vP.xyz*qR.www
					::PTVector3SIMDCross(PTVectorSIMD{ qR.m_Data }, vPosition).m_Data, //cross(qR.xyz, vP.xyz)
					_mm_mul_ps(vPosition.m_Data, _mm_shuffle_ps(qR.m_Data, qR.m_Data, _MM_SHUFFLE(3, 3, 3, 3))) //vP.xyz*qR.www
				) }
			).m_Data,
			_mm_load_ps1(&f2) //2
		)
	) };
}

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransform(PTDualQuaternionSIMD dqRT, PTVectorSIMD vPosition)
{
	//^q = q0 + q1ε 
	PTQuaternionSIMD q0{ dqRT.m_Data[0] };
	PTQuaternionSIMD q1{ dqRT.m_Data[1] };
	
	//r = q0
	PTQuaternionSIMD qRotation = q0;

	//t = 2 * { Quat_Mul(q1,Quat_Con(q0)).xyz }
	alignas(16) float const f_1_1_11[4] = { -1.0f,-1.0f,-1.0f,1.0f };
	float f2 = 2.0f;
	__m128 vTranslation = ::_mm_mul_ps(
		::PTQuaternionSIMDMultiply(q1, PTQuaternionSIMD{ _mm_mul_ps(q0.m_Data,_mm_load_ps(f_1_1_11)) }).m_Data,
		_mm_load_ps1(&f2)
	);
	assert(::fabsf(::PTVectorSIMDGetW(PTVectorSIMD{ vTranslation })) < 0.0000005f);
	
	PTVectorSIMD vResult{ _mm_add_ps(::PTVector3SIMDTransform(qRotation, vPosition).m_Data,vTranslation) };
	return vResult;
}

inline PTVectorSIMD PT_VECTORCALL PTVector4SIMDTransform(PTMatrixSIMD M, PTVectorSIMD V)
{
	PTMatrixSIMD mTemp;
	//映射
	mTemp.m_Data[0] = ::_mm_mul_ps(M.m_Data[0], V.m_Data);
	mTemp.m_Data[1] = ::_mm_mul_ps(M.m_Data[1], V.m_Data);
	mTemp.m_Data[2] = ::_mm_mul_ps(M.m_Data[2], V.m_Data);
	mTemp.m_Data[3] = ::_mm_mul_ps(M.m_Data[3], V.m_Data);

	//归约
	mTemp = ::PTMatrix4x4SIMDTranspose(mTemp);
	__m128 vResult = ::_mm_add_ps(
		::_mm_add_ps(mTemp.m_Data[0], mTemp.m_Data[1]),
		::_mm_add_ps(mTemp.m_Data[2], mTemp.m_Data[3]));

	return PTVectorSIMD{ vResult };
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationMatrix(PTMatrixSIMD mRotation)
{
	//|m00 m01 m02|
	//|m10 m11 m12|
	//|m20 m21 m22|

	//m00 + m11 + m22 = 4*(q_w^2) -1
	__m128 m00m00m00m00 = _mm_shuffle_ps(mRotation.m_Data[0], mRotation.m_Data[0], _MM_SHUFFLE(0, 0, 0, 0));
	__m128 m11m11m11m11 = _mm_shuffle_ps(mRotation.m_Data[1], mRotation.m_Data[1], _MM_SHUFFLE(1, 1, 1, 1));
	__m128 m22m22m22m22 = _mm_shuffle_ps(mRotation.m_Data[2], mRotation.m_Data[2], _MM_SHUFFLE(2, 2, 2, 2));

	//(m21 m02 m10) - (m12 m20 m01) = 4*q_w*(q_x q_y q_z)
	__m128 m21m02m10 = _mm_shuffle_ps(
		_mm_shuffle_ps(mRotation.m_Data[2], mRotation.m_Data[0], _MM_SHUFFLE(2, 2, 1, 1)), //m21 m21 m02 m02
		mRotation.m_Data[1], //m10 m11 m12
		_MM_SHUFFLE(0, 0, 2, 0) //m02 m21 m10 _
	);
	__m128 m12m20m01 = _mm_shuffle_ps(
		_mm_shuffle_ps(mRotation.m_Data[1], mRotation.m_Data[2], _MM_SHUFFLE(0, 0, 2, 2)), //m12 m12 m20 m20
		mRotation.m_Data[0], //m00 m01 m02
		_MM_SHUFFLE(1, 1, 2, 0) //m12 m20 m01 _
	);

	float f1 = 1.0f;
	float f1div4 = 0.25f;
	__m128 v1div4 = _mm_load_ps1(&f1div4);	
	__m128 q_wwww = _mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(_mm_add_ps(m00m00m00m00, m22m22m22m22), _mm_add_ps(m11m11m11m11, _mm_load_ps1(&f1))), v1div4));
	__m128 q_xyz = _mm_div_ps(_mm_mul_ps(_mm_sub_ps(m21m02m10, m12m20m01), v1div4), q_wwww);

	alignas(16) const uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float const*>(uSelect1110));

	return PTQuaternionSIMD{
		::_mm_or_ps(
			::_mm_and_ps(vSelect1110, q_xyz),//x y z _
			::_mm_andnot_ps(vSelect1110, q_wwww)//_ _ _ w
		)
	};
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationX(float Angle)
{
	alignas(16) float fAxisX[4] = { 1.0f,0.0f,0.0f };
	return ::PTQuaternionSIMDRotationNormalAxis(PTVectorSIMD{ ::_mm_load_ps(fAxisX) }, Angle);
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationY(float Angle)
{
	alignas(16) float fAxisY[4] = { 0.0f,1.0f,0.0f };
	return ::PTQuaternionSIMDRotationNormalAxis(PTVectorSIMD{ ::_mm_load_ps(fAxisY) }, Angle);
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationZ(float Angle)
{
	alignas(16) float fAxisZ[4] = { 0.0f,0.0f,1.0f };
	return ::PTQuaternionSIMDRotationNormalAxis(PTVectorSIMD{ ::_mm_load_ps(fAxisZ) }, Angle);
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationXYZ(float AngleX, float AngleY, float AngleZ)
{
	return ::PTQuaternionSIMDMultiply(PTQuaternionSIMDRotationZ(AngleZ), ::PTQuaternionSIMDMultiply(::PTQuaternionSIMDRotationY(AngleY), ::PTQuaternionSIMDRotationX(AngleX)));
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationAxis(PTVectorSIMD Axis, float Angle)
{
	PTVectorSIMD NormalAxis = ::PTVector3SIMDNormalize(Axis);
	return ::PTQuaternionSIMDRotationNormalAxis(NormalAxis, Angle);
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationNormalAxis(PTVectorSIMD NormalAxis, float Angle)
{
	float const fHalfAngle = Angle * 0.5f;
	float fSin = ::sinf(fHalfAngle);
	float fCos = ::cosf(fHalfAngle);

	__m128 vSin = ::_mm_load_ps1(&fSin);
	__m128 vCos = ::_mm_load_ps1(&fCos);

	alignas(16) const uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float const*>(uSelect1110));

	return PTQuaternionSIMD{
		::_mm_or_ps(
			::_mm_and_ps(vSelect1110, ::_mm_mul_ps(NormalAxis.m_Data, vSin)),//CinU/2*(Nx,Ny,Nz) _
			::_mm_andnot_ps(vSelect1110, vCos)//_ _ _ CosU/2
		)
	};
}

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDMultiply(PTQuaternionSIMD q0, PTQuaternionSIMD q1)
{
	//(q0_w,q0_z·k,q0_y·j,q0_x·i)*q1_x·i = (q0_w·q1_x·i, q0_z·q1_x·j, q0_y·q1_x·-k,q0_x·q1_x·-1)
	alignas(16) float const f11_1_1[4] = { 1.0f,1.0f,-1.0f,-1.0f };
	__m128 q0_mul_q1_x = _mm_mul_ps(
		_mm_mul_ps(
			_mm_shuffle_ps(q0.m_Data, q0.m_Data, _MM_SHUFFLE(0, 1, 2, 3)),
			_mm_shuffle_ps(q1.m_Data, q1.m_Data, _MM_SHUFFLE(0, 0, 0, 0))
		), 
		_mm_load_ps(f11_1_1)
	);
	
	//(q0_z·k,q0_w,q0_x·i,q0_y·j)*q1_y·j = (q0_z·q1_y·-i, q0_w·q1_y·j, q0_x·q1_y·k,q0_x·q1_x·-1)
	alignas(16) float const f_111_1[4] = { -1.0f,1.0f,1.0f,-1.0f };
	__m128 q0_mul_q1_y = _mm_mul_ps(
		_mm_mul_ps(
			_mm_shuffle_ps(q0.m_Data, q0.m_Data, _MM_SHUFFLE(1, 0, 3, 2)), 
			_mm_shuffle_ps(q1.m_Data, q1.m_Data, _MM_SHUFFLE(1, 1, 1, 1))
		), 
		_mm_load_ps(f_111_1)
	);

	//(q0_y·j,q0_x·i,q0_w,q0_z·k)*q1_z·k = (q0_y·q1_z·i, q0_x·q1_z·-j, q0_w·q1_z·k,q0_z·q1_z·-1)
	alignas(16) float const f1_11_1[4] = { 1.0f,-1.0f,1.0f,-1.0f };
	__m128 q0_mul_q1_z = _mm_mul_ps(
		_mm_mul_ps(
			_mm_shuffle_ps(q0.m_Data, q0.m_Data, _MM_SHUFFLE(2, 3, 0, 1)),
			_mm_shuffle_ps(q1.m_Data, q1.m_Data, _MM_SHUFFLE(2, 2, 2, 2))
		), 
		_mm_load_ps(f1_11_1)
	);

	//(q0_x·i,q0_y·j,q0_z·k,q0_w)*q1_w = (q0_x·q1_w·i, q0_y·q1_w·k, q0_z·q1_w·k, q0_w·q1_w)
	__m128 q0_mul_q1_w = _mm_mul_ps(q0.m_Data, _mm_shuffle_ps(q1.m_Data, q1.m_Data, _MM_SHUFFLE(3, 3, 3, 3)));

	PTQuaternionSIMD qResult;
	qResult.m_Data = _mm_add_ps(
		_mm_add_ps(q0_mul_q1_x, q0_mul_q1_z), 
		_mm_add_ps(q0_mul_q1_y, q0_mul_q1_w)
	);
	return qResult;
}

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDRotationTranslationMatrix(PTMatrixSIMD mRotationTranslation)
{
	PTQuaternionSIMD qRotation = ::PTQuaternionSIMDRotationMatrix(mRotationTranslation);

	float fTranslationX = ::PTVectorSIMDGetW(PTVectorSIMD{ mRotationTranslation.m_Data[0] });
	float fTranslationY = ::PTVectorSIMDGetW(PTVectorSIMD{ mRotationTranslation.m_Data[1] });
	float fTranslationZ = ::PTVectorSIMDGetW(PTVectorSIMD{ mRotationTranslation.m_Data[2] });

	return ::PTDualQuaternionSIMDRotationTranslation(qRotation, fTranslationX, fTranslationY, fTranslationZ);
}

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDRotationTranslation(PTQuaternionSIMD qRotation, float fTranslationX, float fTranslationY, float fTranslationZ)
{
	//r
	PTQuaternionSIMD q0 = qRotation;
	
	//[0,(1/2)×t]×r
	//[w, xyz] //注意写法上的差异
	float const f1div2 = 0.5f;
	PTQuaternionSIMD qTranslation{ _mm_mul_ps(_mm_set_ps(0.0f,fTranslationZ,fTranslationY,fTranslationX), _mm_load_ps1(&f1div2)) };
	PTQuaternionSIMD q1 = ::PTQuaternionSIMDMultiply(qTranslation, qRotation);

	PTDualQuaternionSIMD dqResult;
	dqResult.m_Data[0] = q0.m_Data;
	dqResult.m_Data[1] = q1.m_Data;
	return dqResult;
}

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDMultiply(PTDualQuaternionSIMD dq0, PTDualQuaternionSIMD dq1)
{
	PTDualQuaternionSIMD dqResult;
	dqResult.m_Data[0] = ::PTQuaternionSIMDMultiply(PTQuaternionSIMD{ dq0.m_Data[0] }, PTQuaternionSIMD{ dq1.m_Data[0] }).m_Data;
	dqResult.m_Data[1] = ::_mm_add_ps(
		::PTQuaternionSIMDMultiply(PTQuaternionSIMD{ dq0.m_Data[0] }, PTQuaternionSIMD{ dq1.m_Data[1] }).m_Data,
		::PTQuaternionSIMDMultiply(PTQuaternionSIMD{ dq0.m_Data[1] }, PTQuaternionSIMD{ dq1.m_Data[0] }).m_Data
	);
	return dqResult;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4FLoad(const PTMatrix4x4F* pSource)
{
	PTMatrixSIMD mTemp;
	mTemp.m_Data[0] = ::_mm_loadu_ps(&pSource->r[0].x);
	mTemp.m_Data[1] = ::_mm_loadu_ps(&pSource->r[1].x);
	mTemp.m_Data[2] = ::_mm_loadu_ps(&pSource->r[2].x);
	mTemp.m_Data[3] = ::_mm_loadu_ps(&pSource->r[3].x);
	return mTemp;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4FLoadA(const PTMatrix4x4F* pSource)
{
	assert((reinterpret_cast<uintptr_t>(&pSource->r[0].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pSource->r[1].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pSource->r[2].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pSource->r[3].x) % 16U) == 0U);

	PTMatrixSIMD mTemp;
	mTemp.m_Data[0] = ::_mm_load_ps(&pSource->r[0].x);
	mTemp.m_Data[1] = ::_mm_load_ps(&pSource->r[1].x);
	mTemp.m_Data[2] = ::_mm_load_ps(&pSource->r[2].x);
	mTemp.m_Data[3] = ::_mm_load_ps(&pSource->r[3].x);
	return mTemp;
}

inline void PT_VECTORCALL PTMatrix4x4FStore(PTMatrix4x4F* pDestination, PTMatrixSIMD M)
{
	::_mm_storeu_ps(&pDestination->r[0].x, M.m_Data[0]);
	::_mm_storeu_ps(&pDestination->r[1].x, M.m_Data[1]);
	::_mm_storeu_ps(&pDestination->r[2].x, M.m_Data[2]);
	::_mm_storeu_ps(&pDestination->r[3].x, M.m_Data[3]);
}

inline void PT_VECTORCALL PTMatrix4x4FStoreA(PTMatrix4x4F* pDestination, PTMatrixSIMD M)
{
	assert((reinterpret_cast<uintptr_t>(&pDestination->r[0].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->r[1].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->r[2].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->r[3].x) % 16U) == 0U);

	::_mm_store_ps(&pDestination->r[0].x, M.m_Data[0]);
	::_mm_store_ps(&pDestination->r[1].x, M.m_Data[1]);
	::_mm_store_ps(&pDestination->r[2].x, M.m_Data[2]);
	::_mm_store_ps(&pDestination->r[3].x, M.m_Data[3]);
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDTranspose(PTMatrixSIMD M)
{
	__m128 vTemp1 = _mm_shuffle_ps(M.m_Data[0], M.m_Data[1], _MM_SHUFFLE(1, 0, 1, 0));//m00 m01 m10 m11
	__m128 vTemp2 = _mm_shuffle_ps(M.m_Data[2], M.m_Data[3], _MM_SHUFFLE(1, 0, 1, 0));//m20 m21 m30 m31
	__m128 vTemp3 = _mm_shuffle_ps(M.m_Data[0], M.m_Data[1], _MM_SHUFFLE(3, 2, 3, 2));//m02 m03 m12 m13
	__m128 vTemp4 = _mm_shuffle_ps(M.m_Data[2], M.m_Data[3], _MM_SHUFFLE(3, 2, 3, 2));//m22 m23 m32 m33

	PTMatrixSIMD mResult;
	mResult.m_Data[0] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));//m00 m10 m20 m30
	mResult.m_Data[1] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));//m01 m11 m21 m31
	mResult.m_Data[2] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));//m02 m12 m22 m32
	mResult.m_Data[3] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(3, 1, 3, 1));//m03 m13 m23 m33
	return mResult;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDTranslation(float OffsetX, float OffsetY, float OffsetZ)
{
	PTMatrixSIMD mResult;
	alignas(16) float f1000[4] = { 1.0f ,0.0f ,0.0f ,OffsetX };
	alignas(16) float f0100[4] = { 0.0f ,1.0f ,0.0f ,OffsetY };
	alignas(16) float f0010[4] = { 0.0f ,0.0f ,1.0f ,OffsetZ };
	alignas(16) float f0001[4] = { 0.0f ,0.0f, 0.0f, 1.0f };
	mResult.m_Data[0] = ::_mm_load_ps(f1000);
	mResult.m_Data[1] = ::_mm_load_ps(f0100);
	mResult.m_Data[2] = ::_mm_load_ps(f0010);
	mResult.m_Data[3] = ::_mm_load_ps(f0001);
	return mResult;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDRotationQuaternion(PTQuaternionSIMD Q)
{
	//QaQa-QbQb-QcQc+QdQd	2QaQb-QcQd				2QaQc+QbQd
	//2QaQb+2QcQd			-QaQa+QbQb-QcQc+QdQd	-2QaQd+2QbQc
	//2QaQc-2QbQd			2QaQd+2QbQc				-QaQa-QbQb+QcQc+QdQd

	//由于Qa*Qa+Qb*Qb+Qc*Qc+Qd*Qd=1
	//1-2QbQb-2QcQc		2QaQb-QcQd			2QaQc+QbQd
	//2QaQb+2QcQd		1-2QaQa-2QcQc		-2QaQd+2QbQc
	//2QaQc-2QbQd		2QaQd+2QbQc			1-2QaQa-2QbQb


	__m128 v2q = ::_mm_add_ps(Q.m_Data, Q.m_Data);//2Qa 2Qb 2Qc 2Qd

	__m128 vTemp1 = ::_mm_mul_ps(v2q, Q.m_Data);//2QaQa 2QbQb 2QcQc 2QdQd
	__m128 vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 1));//2QbQb 2QaQa 2QaQa _
	vTemp1 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 1, 2, 2));//2QcQc 2QcQc 2QbQb _

	alignas(16) float f1110[4] = { 1.0f ,1.0f ,1.0f ,0.0f };
	__m128 v1110 = ::_mm_load_ps(f1110);

	__m128 vSquare = ::_mm_sub_ps(v1110, ::_mm_add_ps(vTemp2, vTemp1));//1-2QbQb-2QcQc 1-2QaQa2QcQc 1-2QaQa-2QbQb _

	vTemp1 = _mm_shuffle_ps(v2q, v2q, _MM_SHUFFLE(0, 1, 0, 0));//2Qa 2Qa 2Qb _
	vTemp2 = _mm_shuffle_ps(Q.m_Data, Q.m_Data, _MM_SHUFFLE(0, 2, 2, 1));//Qb Qc Qc _
	__m128 vABACBC = ::_mm_mul_ps(vTemp1, vTemp2);//2QaQb 2QaQc 2QbQc _

	vTemp1 = _mm_shuffle_ps(v2q, v2q, _MM_SHUFFLE(0, 0, 1, 2));//2Qc 2Qb 2Qa _
	vTemp2 = _mm_shuffle_ps(Q.m_Data, Q.m_Data, _MM_SHUFFLE(0, 3, 3, 3));//Qd Qd Qd _
	__m128 vQAQBQC = ::_mm_mul_ps(vTemp1, vTemp2);//2QcQd 2QbQd 2QaQd _

	__m128 vAdd = ::_mm_add_ps(vABACBC, vQAQBQC);//2QaQb+2QcQd 2QaQc+2QbQd 2QbQc+2QaQd _
	__m128 vSub = ::_mm_sub_ps(vABACBC, vQAQBQC);//2QaQb-2QcQd 2QaQc-2QbQd 2QbQc-2QaQd _

	__m128 v12 = _mm_shuffle_ps(vAdd, vSub, _MM_SHUFFLE(2, 0, 0, 1));//2QaQc+2QbQd 2QaQb+2QcQd 2QaQb-2QcQd 2QbQc-2QaQd
	__m128 v34 = _mm_shuffle_ps(vAdd, vSub, _MM_SHUFFLE(0, 1, 0, 2));//2QbQc+2QaQd _           2QaQc-2QbQd _

	PTMatrixSIMD mResult;
	mResult.m_Data[0] = _mm_shuffle_ps(vSquare, v12, _MM_SHUFFLE(0, 2, 0, 0));//1-2QbQb-2QcQc _ 2QaQb-2QcQd 2QaQc+2QbQd
	mResult.m_Data[0] = _mm_shuffle_ps(mResult.m_Data[0], mResult.m_Data[0], _MM_SHUFFLE(0, 3, 2, 0));//1-2QbQb-2QcQc 2QaQb-2QcQd 2QaQc+2QbQd _
	mResult.m_Data[1] = _mm_shuffle_ps(vSquare, v12, _MM_SHUFFLE(3, 1, 0, 1));//1-2QaQa-2QcQc _ 2QaQb+2QcQd 2QbQc-2QaQd
	mResult.m_Data[1] = _mm_shuffle_ps(mResult.m_Data[1], mResult.m_Data[1], _MM_SHUFFLE(0, 3, 0, 2));//2QaQb+2QcQd 1-2QaQa-2QcQc 2QbQc-2QaQd _
	mResult.m_Data[2] = _mm_shuffle_ps(vSquare, v34, _MM_SHUFFLE(0, 2, 0, 2));//1-2QaQa-2QbQb _ 2QaQc-2QbQd 2QbQc+2QaQd
	mResult.m_Data[2] = _mm_shuffle_ps(mResult.m_Data[2], mResult.m_Data[2], _MM_SHUFFLE(0, 0, 3, 2));//2QaQc-2QbQd 2QbQc+2QaQd 1-2QaQa-2QbQb _

	alignas(16) uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect1110));

	mResult.m_Data[0] = ::_mm_and_ps(vSelect1110, mResult.m_Data[0]);
	mResult.m_Data[1] = ::_mm_and_ps(vSelect1110, mResult.m_Data[1]);
	mResult.m_Data[2] = ::_mm_and_ps(vSelect1110, mResult.m_Data[2]);

	alignas(16) float f0001[4] = { 0.0f ,0.0f ,0.0f ,1.0f };
	mResult.m_Data[3] = ::_mm_load_ps(f0001);

	return mResult;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDLookToRH(PTVectorSIMD EyePosition, PTVectorSIMD EyeDirection, PTVectorSIMD UpDirection, PTMatrixSIMD *pInverse)
{
	alignas(16) uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect1110));

	alignas(16) float f0001[4] = { 0.0f ,0.0f ,0.0f ,1.0f };
	__m128 v0001 = ::_mm_load_ps(f0001);

	PTVectorSIMD NegEyeDirection = ::PTVectorSIMDNegate(EyeDirection);
	PTVectorSIMD AZ = ::PTVector3SIMDNormalize(NegEyeDirection);
	PTVectorSIMD AX = ::PTVector3SIMDNormalize(::PTVector3SIMDCross(UpDirection, AZ));
	PTVectorSIMD AY = ::PTVector3SIMDCross(AZ, AX); //垂直而不需要Normalize

	if (pInverse != NULL)
	{
		//基变换
		//|AXx AYx AZx 0|
		//|AXy AYy AZy 0|
		//|AXz AYz AYz 0|
		//|0   0   0   1|

		//平移
		//|1 0 0 Px|
		//|0 1 0 Py|
		//|0 0 1 Pz|
		//|0 0 0 1 |

		//平移×基变换
		//|AXx AYx AZx Px|
		//|AXy AYy AZy Py|
		//|AXz AYz AYz Pz|
		//|0   0   0   1 |

		PTMatrixSIMD mInverse;
		mInverse.m_Data[0] = ::_mm_and_ps(vSelect1110, AX.m_Data);//AXx AXy AXz 0
		mInverse.m_Data[1] = ::_mm_and_ps(vSelect1110, AY.m_Data);//AYx AYy AYz 0
		mInverse.m_Data[2] = ::_mm_and_ps(vSelect1110, AZ.m_Data);//AZx AZy AZz 0
		mInverse.m_Data[3] = ::_mm_or_ps(
			::_mm_and_ps(vSelect1110, EyePosition.m_Data),//Px Py Pz _
			::_mm_andnot_ps(vSelect1110, v0001)//_ _ _ 1
		);

		(*pInverse) = PTMatrix4x4SIMDTranspose(mInverse);
	}

	//正交阵的逆为正交阵的转置
	//|AXx AXy AXz 0|
	//|AYx AYy AYz 0|
	//|AZx AZy AZz 0|
	//|0   0   0   1|

	//平移
	//|1 0 0 -Px|
	//|0 1 0 -Py|
	//|0 0 1 -Pz|
	//|0 0 0 1 |
	PTVectorSIMD NegEyePosition = ::PTVectorSIMDNegate(EyePosition);
	
	//相乘
	PTVectorSIMD DX = ::PTVector3SIMDDot(AX, NegEyePosition);
	PTVectorSIMD DY = ::PTVector3SIMDDot(AY, NegEyePosition);
	PTVectorSIMD DZ = ::PTVector3SIMDDot(AZ, NegEyePosition);

	PTMatrixSIMD mResult;
	mResult.m_Data[0] = ::_mm_or_ps(
		::_mm_and_ps(vSelect1110, AX.m_Data),//RX x y z _
		::_mm_andnot_ps(vSelect1110, DX.m_Data)//DX _ _ _ w
	);
	mResult.m_Data[1] = ::_mm_or_ps(
		::_mm_and_ps(vSelect1110, AY.m_Data),//RY x y z _
		::_mm_andnot_ps(vSelect1110, DY.m_Data)//DY _ _ _ w
	);
	mResult.m_Data[2] = ::_mm_or_ps(
		::_mm_and_ps(vSelect1110, AZ.m_Data),//RZ x y z _
		::_mm_andnot_ps(vSelect1110, DZ.m_Data)//DZ _ _ _ w
	);
	mResult.m_Data[3] = v0001;

	return mResult;
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

	alignas(16) uint32_t uSelect0100[4] = { 0x00000000 ,0xFFFFFFFF ,0x00000000 ,0x00000000 };
	__m128 vSelect0100 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect0100));
	
	alignas(16) float f000_1[4] = { 0.0f, 0.0f, 0.0f , -1.0f };
	__m128 v000_1 = ::_mm_load_ps(f000_1);

	if (pInverse)
	{
		float _1Da = (FarZ - NearZ) / FarZ;//确保FarZ趋向于正无穷大时的正确性
		_1Da /= NearZ;
		float _bDa = 1.0f / FarZ;

		alignas(16) float fTempInverse[4] = { ViewWidth / 2.0f,ViewHeight / 2.0f ,_1Da,_bDa };
		__m128 vTempInverse = ::_mm_load_ps(fTempInverse);//Width/2 Height/2 1/a b/a

		PTMatrixSIMD mInverse;
		//r0
		mInverse.m_Data[0] = ::_mm_setzero_ps();
		mInverse.m_Data[0] = ::_mm_move_ss(mInverse.m_Data[0], vTempInverse);//Width/2 0 0 0
		//r1
		mInverse.m_Data[1] = ::_mm_and_ps(vSelect0100, vTempInverse);//0 Height/2 0 0
		//r2
		mInverse.m_Data[2] = v000_1;
		//r3
		mInverse.m_Data[3] = ::_mm_setzero_ps();
		mInverse.m_Data[3] = _mm_shuffle_ps(mInverse.m_Data[3], vTempInverse, _MM_SHUFFLE(3, 2, 0, 0));//0 0 1/a b/a

		(*pInverse) = mInverse;
	}

	float b = NearZ / (FarZ - NearZ);
	float a = FarZ / (FarZ - NearZ);//确保FarZ趋向于正无穷大时的正确性
	a *= NearZ;

	alignas(16) float fTemp[4] = { 2.0f / ViewWidth,2.0f / ViewHeight,b,a };
	__m128 vTemp = ::_mm_load_ps(fTemp);//2/Width 2/Height b a

	PTMatrixSIMD mResult;
	//r0
	mResult.m_Data[0] = ::_mm_setzero_ps();
	mResult.m_Data[0] = ::_mm_move_ss(mResult.m_Data[0], vTemp);//2/Width 0 0 0
	//r1
	mResult.m_Data[1] = ::_mm_and_ps(vSelect0100, vTemp);//0 2/Height 0 0
	//r2
	mResult.m_Data[2] = ::_mm_setzero_ps();
	mResult.m_Data[2] = _mm_shuffle_ps(mResult.m_Data[2], vTemp, _MM_SHUFFLE(3, 2, 0, 0));//0 0 b a
	//r3
	mResult.m_Data[3] = _mm_shuffle_ps(v000_1, v000_1, _MM_SHUFFLE(2, 3, 1, 0));//0 0 -1 0

	return mResult;
}

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDOrthographicRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
	//Nvidia Reversed-Z Trick
	//http://developer.nvidia.com/content/depth-precision-visualized

	// _ 0 0 0
	// 0 _ 0 0
	// 0 0 b a
	// 0 0 0 1
	float b = 1.0f / (FarZ - NearZ);
	float a = FarZ / (FarZ - NearZ);//确保FarZ趋向于正无穷大时的正确性
	alignas(16) float fValues[4] = { 2.0f / ViewWidth,2.0f / ViewHeight,b,a };
	__m128 vValues = ::_mm_load_ps(fValues);//2/Width 2/Height b a

	PTMatrixSIMD mResult;
	//r0
	mResult.m_Data[0] = ::_mm_setzero_ps();
	mResult.m_Data[0] = ::_mm_move_ss(mResult.m_Data[0], vValues);//2/Width 0 0 0
	//r1
	alignas(16) uint32_t uSelect0100[4] = { 0x00000000 ,0xFFFFFFFF ,0x00000000 ,0x00000000 };
	__m128 vSelect0100 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect0100));
	mResult.m_Data[1] = ::_mm_and_ps(vSelect0100, vValues);//0 2/Height 0 0
	//r2
	mResult.m_Data[2] = ::_mm_setzero_ps();
	mResult.m_Data[2] = _mm_shuffle_ps(mResult.m_Data[2], vValues, _MM_SHUFFLE(3, 2, 0, 0));//0 0 b a
	//r3
	alignas(16) float fSelect0010[4] = { 0.0f, 0.0f, 0.0f , 1.0f };
	mResult.m_Data[3] = ::_mm_load_ps(fSelect0010);//0 0 0 1
	return mResult;
}


inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDMultiply(PTMatrixSIMD M1, PTMatrixSIMD M2)
{
	PTMatrixSIMD mResult;
	__m128 vR = M1.m_Data[0];//a00 a01 a02 a03
	__m128 vX = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(0, 0, 0, 0));//a00 a00 a00 a00
	__m128 vY = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(1, 1, 1, 1));//a01 a01 a01 a01
	__m128 vZ = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(2, 2, 2, 2));//a02 a02 a02 a02
	__m128 vW = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(3, 3, 3, 3));//a03 a03 a03 a03
	
	//计算第一行
	//映射
	vX = ::_mm_mul_ps(vX, M2.m_Data[0]);//a00*b00 a00*b01 a00*b02 a00*b03
	vY = ::_mm_mul_ps(vY, M2.m_Data[1]);//a01*b10 a01*b11 a01*b12 a01*b13
	vZ = ::_mm_mul_ps(vZ, M2.m_Data[2]);//a02*b20 a02*b21 a02*b22 a02*b23
	vW = ::_mm_mul_ps(vW, M2.m_Data[3]);//a03*b30 a03*b31 a03*b32 a03*b33
	//归约
	mResult.m_Data[0] = ::_mm_add_ps( //a00*b00+a01*b10+a02*b20+a03*b30 a00*b01+a01*b11+a02*b21+a03*b31 a00*b02+a01*b12+a02*b22+a03*b32 a00*b03+a01*b13+a02*b23+a03*b33
		::_mm_add_ps(vX, vZ),
		::_mm_add_ps(vY, vW)
	);
	
	//计算其它三行
	vR = M1.m_Data[1];
	vX = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(0, 0, 0, 0));
	vY = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(2, 2, 2, 2));
	vW = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(3, 3, 3, 3));
	vX = ::_mm_mul_ps(vX, M2.m_Data[0]);
	vY = ::_mm_mul_ps(vY, M2.m_Data[1]);
	vZ = ::_mm_mul_ps(vZ, M2.m_Data[2]);
	vW = ::_mm_mul_ps(vW, M2.m_Data[3]);
	mResult.m_Data[1] = ::_mm_add_ps(::_mm_add_ps(vX, vZ), ::_mm_add_ps(vY, vW));
	
	vR = M1.m_Data[2];
	vX = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(0, 0, 0, 0));
	vY = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(2, 2, 2, 2));
	vW = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(3, 3, 3, 3));
	vX = ::_mm_mul_ps(vX, M2.m_Data[0]);
	vY = ::_mm_mul_ps(vY, M2.m_Data[1]);
	vZ = ::_mm_mul_ps(vZ, M2.m_Data[2]);
	vW = ::_mm_mul_ps(vW, M2.m_Data[3]);
	mResult.m_Data[2] = ::_mm_add_ps(::_mm_add_ps(vX, vZ), ::_mm_add_ps(vY, vW));

	vR = M1.m_Data[3];
	vX = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(0, 0, 0, 0));
	vY = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(2, 2, 2, 2));
	vW = _mm_shuffle_ps(vR, vR, _MM_SHUFFLE(3, 3, 3, 3));
	vX = ::_mm_mul_ps(vX, M2.m_Data[0]);
	vY = ::_mm_mul_ps(vY, M2.m_Data[1]);
	vZ = ::_mm_mul_ps(vZ, M2.m_Data[2]);
	vW = ::_mm_mul_ps(vW, M2.m_Data[3]);
	mResult.m_Data[3] = ::_mm_add_ps(::_mm_add_ps(vX, vZ), ::_mm_add_ps(vY, vW));

	return mResult;
}

//相交测试

struct PTFrustumSIMD //视锥体（四棱台）
{
	__m128 Plane[6];
};

struct PTSphereSIMD
{

	__m128 Center;
	__m128 Radius;
};

struct PTAABSIMD
{

	__m128 Center;
	__m128 HalfDiagonal;
};

struct PTOBSIMD
{

	__m128 Center;
	__m128 HalfDiagonal;
	__m128 Axis[3];
};

//向量(a,b,c,d)表示平面ax + by + cz + d = 0
//点P(px,py,pc)到平面上任意一点在平面法向量(a,b,c)上的投影为(a * xp + b * yp + c * pc + d) / 根号(a^2 + b^2 + c^2)，特别地，当平面法向量为单位向量时，根号(a^2 + b^2 + c^2)为1，上式即为(px,py,pc,1)点积(a,b,c,d)
//可根据投影与0的大小关系确定点P在平面的哪一侧

inline __m128 PT_VECTORCALL PTSIMDInternalPlaneNormalize(__m128 P)
{
	//计算(P.x,P.y,P.z)的长度
	__m128 vLength = _mm_sqrt_ps((::PTVector3SIMDDot(PTVectorSIMD{ P }, PTVectorSIMD{ P })).m_Data);
	//零向量的方向是任意的！！！
	__m128 vResult = _mm_div_ps(P, vLength);
	return vResult;
}

inline PTFrustumSIMD PT_VECTORCALL PTFrustumSIMDLoadRH(PTMatrixSIMD ViewProjection)
{
	//Gil Gribb  Klaus Hartmann,"Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix", 06/15/2001
	//http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

	PTFrustumSIMD F;

	//Near
	F.Plane[0] = ::_mm_sub_ps(ViewProjection.m_Data[3], ViewProjection.m_Data[2]);
	F.Plane[0] = ::PTSIMDInternalPlaneNormalize(F.Plane[0]);

	//Far
	F.Plane[1] = ViewProjection.m_Data[2];
	F.Plane[1] = ::PTSIMDInternalPlaneNormalize(F.Plane[1]);

	//Right
	F.Plane[2]= ::_mm_sub_ps(ViewProjection.m_Data[3], ViewProjection.m_Data[0]);
	F.Plane[2] = ::PTSIMDInternalPlaneNormalize(F.Plane[2]);

	//Left
	F.Plane[3] = ::_mm_add_ps(ViewProjection.m_Data[3], ViewProjection.m_Data[0]);
	F.Plane[3] = ::PTSIMDInternalPlaneNormalize(F.Plane[3]);

	//Top
	F.Plane[4] = ::_mm_sub_ps(ViewProjection.m_Data[3], ViewProjection.m_Data[1]);
	F.Plane[4] = ::PTSIMDInternalPlaneNormalize(F.Plane[4]);

	//Bottom
	F.Plane[5] = ::_mm_add_ps(ViewProjection.m_Data[3], ViewProjection.m_Data[1]);
	F.Plane[5] = ::PTSIMDInternalPlaneNormalize(F.Plane[5]);

	return F;
}

inline void PT_VECTORCALL PTFrustumFStore(PTFrustumF *pDestination, PTFrustumSIMD F)
{
	::_mm_storeu_ps(&pDestination->Plane[0].x, F.Plane[0]);
	::_mm_storeu_ps(&pDestination->Plane[1].x, F.Plane[1]);
	::_mm_storeu_ps(&pDestination->Plane[2].x, F.Plane[2]);
	::_mm_storeu_ps(&pDestination->Plane[3].x, F.Plane[3]);
	::_mm_storeu_ps(&pDestination->Plane[4].x, F.Plane[4]);
	::_mm_storeu_ps(&pDestination->Plane[5].x, F.Plane[5]);
}

inline void PT_VECTORCALL PTFrustumFStoreA(PTFrustumF *pDestination, PTFrustumSIMD F)
{
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[0].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[1].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[2].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[3].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[4].x) % 16U) == 0U);
	assert((reinterpret_cast<uintptr_t>(&pDestination->Plane[5].x) % 16U) == 0U);

	::_mm_store_ps(&pDestination->Plane[0].x, F.Plane[0]);
	::_mm_store_ps(&pDestination->Plane[1].x, F.Plane[1]);
	::_mm_store_ps(&pDestination->Plane[2].x, F.Plane[2]);
	::_mm_store_ps(&pDestination->Plane[3].x, F.Plane[3]);
	::_mm_store_ps(&pDestination->Plane[4].x, F.Plane[4]);
	::_mm_store_ps(&pDestination->Plane[5].x, F.Plane[5]);
}

inline PTSphereSIMD PT_VECTORCALL PTSphereSIMDLoadFromPoint(uint32_t Count, PTVector4F const *pPointArray, uint32_t Stride)
{
	//Jack Ritter. "An efficient bounding sphere." Graphics Gems 1 1990.

	//The first pass finds two points that are close to maximally spaced.
	//This pair describes the initial guess for the sphere.

	//Make one(quick) pass through the N points.
	//Find these six points :
	//The point with minimum x, the point with maximum x,
	//The point with minimum y, the point with maximum y,
	//The point with minimum z, the point with maximum z.
	PTVectorSIMD PointMinX;
	PTVectorSIMD PointMaxX;
	PTVectorSIMD PointMinY;
	PTVectorSIMD PointMaxY;
	PTVectorSIMD PointMinZ;
	PTVectorSIMD PointMaxZ;

	PointMinX = PointMaxX = PointMinY = PointMaxY = PointMinZ = PointMaxZ = ::PTVector4FLoad(pPointArray);

	for (uint32_t i = 1U; i < Count; ++i)
	{
		PTVectorSIMD vPoint = ::PTVector4FLoad(reinterpret_cast<PTVector4F const*>(reinterpret_cast<uintptr_t>(pPointArray) + Stride * i));

		float X = PTVectorSIMDGetX(vPoint);
		float Y = PTVectorSIMDGetY(vPoint);
		float Z = PTVectorSIMDGetZ(vPoint);

		if (X < PTVectorSIMDGetX(PointMinX)) { PointMinX = vPoint; }
		if (X > PTVectorSIMDGetX(PointMaxX)) { PointMaxX = vPoint; }
		if (Y < PTVectorSIMDGetY(PointMinY)) { PointMinY = vPoint; }
		if (Y > PTVectorSIMDGetY(PointMaxY)) { PointMaxY = vPoint; }
		if (Z < PTVectorSIMDGetZ(PointMinZ)) { PointMinZ = vPoint; }
		if (Z > PTVectorSIMDGetZ(PointMaxZ)) { PointMaxZ = vPoint; }
	}

	//This gives three pairs of points.
	//Each pair has the maximum span for its dimension.
	//Pick the pair with the maximum point-to-point separation(which could be greater than the maximum dimensional span).
	//Calculate the initial sphere, using this pair of points as a diameter.
	PTVectorSIMD vCenter;
	float fRadius_Square;
	{
		float fSpanX_Square;
		{
			PTVectorSIMD DeltaX = ::PTVectorSIMDSubtract(PointMaxX, PointMinX);
			fSpanX_Square = ::PTVectorSIMDGetX(::PTVector3SIMDDot(DeltaX, DeltaX));
		}
		float fSpanY_Square;
		{
			PTVectorSIMD DeltaY = ::PTVectorSIMDSubtract(PointMaxY, PointMinY);
			fSpanY_Square = ::PTVectorSIMDGetX(::PTVector3SIMDDot(DeltaY, DeltaY));
		}
		float fSpanZ_Square;
		{
			PTVectorSIMD DeltaZ = ::PTVectorSIMDSubtract(PointMaxZ, PointMinZ);
			fSpanZ_Square = ::PTVectorSIMDGetX(::PTVector3SIMDDot(DeltaZ, DeltaZ));
		}

		PTVectorSIMD vHalf = ::PTVectorSIMDReplicate(0.5f);

		//X>Y
		if (fSpanX_Square > fSpanY_Square)
		{
			//X>Z X>Y
			if (fSpanX_Square > fSpanZ_Square)
			{
				vCenter = ::PTVectorSIMDMultiply(::PTVectorSIMDAdd(PointMaxX, PointMinX), vHalf);
				fRadius_Square = fSpanX_Square * 0.25f;
			}
			//Z>X>Y
			else
			{
				vCenter = ::PTVectorSIMDMultiply(::PTVectorSIMDAdd(PointMaxZ, PointMinZ), vHalf);
				fRadius_Square = fSpanZ_Square * 0.25f;
			}
		}
		//Y>X
		else
		{
			//Y>X Y>Z
			if (fSpanY_Square > fSpanZ_Square)
			{
				vCenter = ::PTVectorSIMDMultiply(::PTVectorSIMDAdd(PointMaxY, PointMaxY), vHalf);
				fRadius_Square = fSpanY_Square * 0.25f;
			}
			//Z>Y>X
			else
			{
				vCenter = ::PTVectorSIMDMultiply(::PTVectorSIMDAdd(PointMaxZ, PointMinZ), vHalf);
				fRadius_Square = fSpanZ_Square * 0.25f;
			}
		}
	}

	//The second pass compares each point to the current sphere, and enlarges the sphere if the point is outside.
	
	//Make a second pass through the N points: 
	//for each point outside the current sphere, 
	//update the current sphere to the larger sphere passing through the point on one side, 
	//and the back side of the old sphere on the other side.
	
	//一定包含SphereOld，反向延长CenterOldToPoint即可证明
	//RadiusNew -> Average Of RadiusOld And DistanceCenterOldToPoint
	//CenterNew ->CenterOld + (Point - CenterOld) * （1- (RadiusNew/DistanceCenterOldToPoint))
	//

	//Each new sphere will (barely) contain the old sphere, plus the new point, and usually some other outsiders as well.
	//The number of updates needed will be a tiny fraction of N.
	//In testing each point against the current sphere, 
	//the square of its distance from the current sphere’s center is compared to the square of the current sphere’s radius, 
	//to avoid doing a sqrt() calculation.

	for (uint32_t i = 0U; i < Count; ++i)
	{
		PTVectorSIMD vPoint = ::PTVector4FLoad(reinterpret_cast<PTVector4F const*>(reinterpret_cast<uintptr_t>(pPointArray) + Stride * i));

		PTVectorSIMD vDelta = ::PTVectorSIMDSubtract(vPoint, vCenter);

		float fCenterOldToPoint_Square = ::PTVectorSIMDGetX(::PTVector3SIMDDot(vDelta, vDelta));

		if (fCenterOldToPoint_Square > fRadius_Square)
		{
			float fRadiusOld = ::sqrtf(fRadius_Square);
			float fCenterOldToPoint = ::sqrtf(fCenterOldToPoint_Square);
			float fRadiusNew = (fRadiusOld + fCenterOldToPoint)*0.5f;
			fRadius_Square = fRadiusNew * fRadiusNew;

			float Lambda = 1.0f - fRadiusNew / fCenterOldToPoint;
			vCenter = ::PTVectorSIMDAdd(vCenter, ::PTVectorSIMDMultiply(vDelta, ::PTVectorSIMDReplicate(Lambda)));
		}
	}

	return PTSphereSIMD{ vCenter.m_Data,(::PTVectorSIMDReplicate(::sqrtf(fRadius_Square))).m_Data };
}

inline PTSphereSIMD PT_VECTORCALL PTSphereSIMDLoad(PTVector3F *pCenter, float Radius)
{
	PTSphereSIMD S;
	S.Center = ::_mm_set_ps(1.0f, pCenter->z, pCenter->y, pCenter->x);
	S.Radius = ::_mm_set_ps1(Radius);
	return S;
}

inline void PT_VECTORCALL PTSIMDInternalPlaneSphereIntersect(__m128 P, PTSphereSIMD S, __m128 &Inside, __m128 &OutSide)
{
	//球心到平面上任意一点在平面法向量上的投影
	alignas(16) uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect1110));
	float f1 = 1.0f;
	__m128 vf1111 = ::_mm_load_ps1(&f1);
	__m128 vCenter4 = ::_mm_or_ps(
		::_mm_and_ps(vSelect1110, S.Center),//x y z _
		::_mm_andnot_ps(vSelect1110, vf1111)//_ _ _ 1
	);
	__m128 vD = (::PTVector4SIMDDot(PTVectorSIMD{ P }, PTVectorSIMD{ vCenter4 })).m_Data;

	//与PTSIMDFrustumLoadRH一致
	Inside = ::_mm_cmpgt_ps(vD, S.Radius);
	OutSide = ::_mm_cmplt_ps(vD, (::PTVectorSIMDNegate(PTVectorSIMD{ S.Radius })).m_Data);
}

inline PTIntersectionType PT_VECTORCALL PTFrustumSIMDSphereSIMDIntersect(PTFrustumSIMD F, PTSphereSIMD S)
{
	__m128 AnyOutside;
	__m128 AllInside;

	__m128 OutSide;
	__m128 Inside;

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[0], S, Inside, OutSide);
	AnyOutside = OutSide;
	AllInside = Inside;

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[1], S, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[2], S, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[3], S, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[4], S, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneSphereIntersect(F.Plane[5], S, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	if (::_mm_movemask_ps(AnyOutside))//==0XF
	{
		return PTDISJOINT;
	}
	else if (::_mm_movemask_ps(AllInside))//==0XF
	{
		return PTCONTAIN;
	}
	else
	{
		return PTINTERSECT;
	}
}

inline PTAABSIMD PT_VECTORCALL PTAABSIMDLoadFromPoint(uint32_t Count, const PTVector4F* pPointArray, uint32_t Stride)
{
	assert(Count > 0);
	assert(pPointArray);

	PTVectorSIMD vMin;
	PTVectorSIMD vMax;
	vMin = vMax = ::PTVector4FLoad(pPointArray);

	//是否应当使用Map模式并行？？？
	for (uint32_t i = 1U; i < Count; ++i)
	{
		PTVectorSIMD vPoint = ::PTVector4FLoad(reinterpret_cast<PTVector4F const*>(reinterpret_cast<uintptr_t>(pPointArray) + Stride * i));

		vMin.m_Data = ::_mm_min_ps(vMin.m_Data, vPoint.m_Data);
		vMax.m_Data = ::_mm_max_ps(vMax.m_Data, vPoint.m_Data);
	}

	PTVectorSIMD vHalf = ::PTVectorSIMDReplicate(0.5f);
	//(Max+Min)*0.5
	PTVectorSIMD vCenter = ::PTVectorSIMDMultiply(::PTVectorSIMDAdd(vMax, vMin), vHalf);
	//(Max-Min)*0.5
	PTVectorSIMD vHalfDiagonal = ::PTVectorSIMDMultiply(::PTVectorSIMDSubtract(vMax, vMin), vHalf);
	
	PTAABSIMD AAB = { vCenter.m_Data,vHalfDiagonal.m_Data };
	return AAB;
}

inline PTAABSIMD PT_VECTORCALL PTAABSIMDLoad(PTAABF *pSource)
{
	__m128 vCenter = (::PTVector3FLoad(&pSource->Center)).m_Data;
	__m128 vHalfDiagonal = (::PTVector3FLoad(&pSource->HalfDiagonal)).m_Data;
	PTAABSIMD AAB = { vCenter,vHalfDiagonal };
	return AAB;
}

inline void PT_VECTORCALL PTAABFStore(PTAABF *pDestination, PTAABSIMD AAB)
{
	::PTVector3FStore(&pDestination->Center, PTVectorSIMD{ AAB.Center });
	::PTVector3FStore(&pDestination->HalfDiagonal, PTVectorSIMD{ AAB.HalfDiagonal });
}

inline void PT_VECTORCALL PTSIMDInternalPlaneAABIntersect(__m128 P, PTAABSIMD AAB, __m128 &Inside, __m128 &OutSide)
{
	//长方体中心到平面上任意一点在平面法向量上的投影
	alignas(16) uint32_t uSelect1110[4] = { 0XFFFFFFFFU ,0XFFFFFFFFU ,0XFFFFFFFFU ,0X00000000U };
	__m128 vSelect1110 = ::_mm_load_ps(reinterpret_cast<float *>(uSelect1110));
	float f1 = 1.0f;
	__m128 vf1111 = ::_mm_load_ps1(&f1);
	__m128 vCenter4 = ::_mm_or_ps(
		::_mm_and_ps(vSelect1110, AAB.Center),//x y z _
		::_mm_andnot_ps(vSelect1110, vf1111)//_ _ _ 1
	);
	__m128 vD = (::PTVector4SIMDDot(PTVectorSIMD{ P }, PTVectorSIMD{ vCenter4 })).m_Data;

	//长方体8个半对角线在平面法向量上的投影的最大值
	//映射
	__m128 vTemp1 = _mm_mul_ps(P, AAB.HalfDiagonal);//逐分量相乘
	__m128 vTemp2 = _mm_setzero_ps();
	vTemp2 = _mm_sub_ps(vTemp2, vTemp1);
	vTemp1 = _mm_max_ps(vTemp1, vTemp2);//取绝对值
	//归约
	vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 2));//z _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+z y _ _ 注意是ss而不是ps!!!
	vTemp2 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 1));//y _ _ _
	vTemp1 = ::_mm_add_ss(vTemp1, vTemp2);//x+z+y _ _ _
	__m128 vMaxHalfDiagonal = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(0, 0, 0, 0));//x+z+y x+z+y x+z+y x+z+y

	//与PTSIMDFrustumLoadRH一致
	Inside = ::_mm_cmpgt_ps(vD, vMaxHalfDiagonal);
	OutSide = ::_mm_cmplt_ps(vD, (::PTVectorSIMDNegate(PTVectorSIMD{ vMaxHalfDiagonal })).m_Data);
}

inline PTIntersectionType PT_VECTORCALL PTFrustumSIMDAABSIMDIntersect(PTFrustumSIMD F, PTAABSIMD AAB)
{
	__m128 AnyOutside;
	__m128 AllInside;

	__m128 OutSide;
	__m128 Inside;

	::PTSIMDInternalPlaneAABIntersect(F.Plane[0], AAB, Inside, OutSide);
	AnyOutside = OutSide;
	AllInside = Inside;

	::PTSIMDInternalPlaneAABIntersect(F.Plane[1], AAB, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneAABIntersect(F.Plane[2], AAB, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneAABIntersect(F.Plane[3], AAB, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneAABIntersect(F.Plane[4], AAB, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	::PTSIMDInternalPlaneAABIntersect(F.Plane[5], AAB, Inside, OutSide);
	AnyOutside = ::_mm_or_ps(AnyOutside, OutSide);
	AllInside = ::_mm_and_ps(AllInside, Inside);

	if (::_mm_movemask_ps(AnyOutside))
	{
		return PTDISJOINT;
	}
	else if (::_mm_movemask_ps(AllInside))
	{
		return PTCONTAIN;
	}
	else
	{
		return PTINTERSECT;
	}
}

struct PTRaySIMD
{
	__m128 Origin;
	__m128 Direction;
};

struct PTTriangleSIMD
{
	__m128 V0;
	__m128 V1;
	__m128 V2;
};

inline PTRaySIMD PT_VECTORCALL PTRaySIMDLoad(PTVector3F *pOrigin, PTVector3F *pDirection)
{
	__m128 vOrigin = (::PTVector3FLoad(pOrigin)).m_Data;
	__m128 vDirection = (::PTVector3FLoad(pDirection)).m_Data;
	PTRaySIMD Ray = { vOrigin,vDirection };
	return Ray;
}

inline bool PT_VECTORCALL PTRaySIMDSphereSIMDIntersect(PTRaySIMD R, PTSphereSIMD S, float *pt)
{
	PTVectorSIMD vL = ::PTVectorSIMDSubtract(PTVectorSIMD{ S.Center }, PTVectorSIMD{ R.Origin });

	float L_2 = ::PTVectorSIMDGetX(::PTVector3SIMDDot(vL, vL));

	float R_2 = S.Radius*S.Radius;
	PTVectorSIMD vR_Square = ::PTVectorSIMDMultiply(PTVectorSIMD{ S.Radius }, PTVectorSIMD{ S.Radius });

	if (_mm_movemask_ps(_mm_cmple_ps(vL_Square.m_Data, vR_Square.m_Data)))
	{
		if (pt != NULL)
		{
			PTVectorSIMD vS = ::PTVector3SIMDDot(vL, PTVectorSIMD{ R.Direction });

		}

		return true;
	}

	PTVectorSIMD vS = ::PTVector3SIMDDot(vL, PTVectorSIMD{ R.Direction });

	PTVectorSIMD vS_Square = ::PTVectorSIMDMultiply(vS, vS);

	if (::PTVectorSIMDGetX(vS) < 0.0f && ::PTVector3SIMDToBool(PTVectorSIMD{ _mm_cmpgt_ps(vL_Square.m_Data, vR_Square.m_Data) }))
	{
		return false;
	}

	PTVectorSIMD vM_Square = ::PTVectorSIMDSubtract(vL_Square, vS_Square);

	if (_mm_movemask_ps(_mm_cmpgt_ps(vM_Square.m_Data, vR_Square.m_Data)))
	{
		return false;
	}

	if (pt != NULL)
	{
		PTVectorSIMD vQ = PTVectorSIMD{ _mm_sqrt_ps(::PTVectorSIMDSubtract(vR_Square, vM_Square).m_Data) };
		PTVectorSIMD vT = ::PTVectorSIMDSubtract(vS, vQ);
		(*pt) = PTVectorSIMDGetX(vT);
	}

	return true;
}

inline PTTriangleSIMD PT_VECTORCALL PTTriangleSIMDLoad(PTVector3F *pV0, PTVector3F *pV1, PTVector3F *pV2)
{
	__m128 vV0 = (::PTVector3FLoad(pV0)).m_Data;
	__m128 vV1 = (::PTVector3FLoad(pV1)).m_Data;
	__m128 vV2 = (::PTVector3FLoad(pV2)).m_Data;
	PTTriangleSIMD Triangle = { vV0,vV1,vV2 };
	return Triangle;
}

inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_CullBack(PTRaySIMD R, PTTriangleSIMD T, float *pt)
{
	//Tomas Moller, Ben Trumbore. "Fast, Minimum Storage Ray/Triangle Intersection." Journal of Graphics Tools, Volume 2, 1997.

	//射线：R = O + tD
	//三角形：T = V0 + u(V1-V0) + v(V2-V0) //设 e1 = V1 - V0 //e2 = V2 - V0

	//[ ]：表示矩阵
	//| |：表示行列式

	//构建线性方程组：R = T
	//=> O + tD = V0 + u(V1-V0) + v(V2-V0)
	//                   [t]
	//=>[-D V1-V0 V2-V0] [u] = O - V0
	//                   [v]

	//克拉默法则
	//[t]           1         | O - V0 V1 - V0 V2 - V0 |
	//[u] = ----------------- |  -D    O - V0  V2 - V0 |
	//[v]   |-D V1-V0 V2-V0|  |  -D    V1 - V0  O - V0 |

	//标量三重积
	//[-D V1-V0 V2-V0] = -D•((V1-V0)×(V2-V0)) = D•(-(V1-V0)×(V2-V0)) 反映了射线方向与法线方向之间的夹角

	//在右手系且逆时针绕序时，-(V1-V0)×(V2-V0)为△V0V1V2的法线方向的相反方向，[-D V1-V0 V2-V0]＞0即表示射线从三角形正面射入

	//向量积D×(V2-V0)的计算结果可以复用：
	//|-D V1-V0 V2-V0| = -|V1-V0 -D V2-V0| = -(-|V1-V0 D V2-V0|) = |V1-V0 D V2-V0| = (V1-V0)•(D×(V2-V0))
	//|-D  O-V0 V2-V0| = -| O-V0 -D V2-V0| = -(-| O-V0 D V2-V0|) = | O-V0 D V2-V0| =  (O-V0)•(D×(V2-V0))

	__m128 vE1 = _mm_sub_ps(T.V1, T.V0);
	__m128 vE2 = _mm_sub_ps(T.V2, T.V0);

	__m128 vCross_D_E2 = ::PTVector3SIMDCross(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vE2 }).m_Data; //D×(V2-V0)
	
	__m128 det_NegD_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE1 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D V1-V0 V2-V0|

	float fEpsilon = 1e-20f;
	__m128 vEpsilon = _mm_load_ps1(&fEpsilon);

	if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_E1_E2, vEpsilon))) //|-D V1-V0 V2-V0|<Epsilon
	{
		//平行或背面射入
		return false;
	}

	//Division Delay //除以|-D V1-V0 V2-V0|并不一定是必要的

	__m128 vSubOV0 = _mm_sub_ps(R.Origin, T.V0);

	__m128 det_NegD_SubOV0_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D  O-V0 V2-V0|

	//    |-D  O-V0 V2-V0|
	//u = -----------------
	//    |-D V1-V0 V2-V0|

	//u>=0
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>|-D  O-V0 V2-V0|>0

	if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_SubOV0_E2, _mm_setzero_ps()))) //|-D  O-V0 V2-V0|<0
	{
		//不满足u>=0
		return false;
	}

	//u<=1
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>|-D  O-V0 V2-V0|<=|-D V1-V0 V2-V0|

	if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_SubOV0_E2, det_NegD_E1_E2))) //|-D  O-V0 V2-V0|>|-D V1-V0 V2-V0|
	{
		//不满足u<=1

		//根据线性规划，u>=0且v>=0且(u+v)<=1一定有u<=1 
		//判定u<=1是一种EarlyOut

		return false;
	}


	//向量积(O-V0)×(V1-V0)的计算结果可以复用：
	//|O-V0 V1-V0 V2-V0| = -|O-V0 V2-V0 V1-V0| = -(-|V2-V0 O-V0 V1-V0|) = |V2-V0 O-V0 V1-V0| = (V2-V0)•((O-V0)×(V1-V0))
	//| -D  V1-V0  O-V0| = -| -D   O-V0 V1-V0| = -(-|  D   O-V0 V1-V0|) = |  D   O-V0 V1-V0| =       D•((O-V0)×(V1-V0))

	__m128 vCross_SubOV0_E1 = ::PTVector3SIMDCross(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vE1 }).m_Data; //(O-V0)×(V1-V0)

	__m128 det_NegD_E1_SubOV0 = ::PTVector3SIMDDot(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //| -D  V1-V0  O-V0|

	//    |-D V1-V0  O-V0|
	//v = -----------------
	//    |-D V1-V0 V2-V0|

	//v>=0
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>|-D V1-V0 O-V0|>=0

	if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_E1_SubOV0, _mm_setzero_ps()))) //|-D V1-V0  O-V0|<0
	{
		//不满足v>=0
		return false;
	}

	//(u+v)<=1 
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)<=|-D V1-V0 V2-V0| 

	if (_mm_movemask_ps(_mm_cmpgt_ps(_mm_add_ps(det_NegD_SubOV0_E2, det_NegD_E1_SubOV0), det_NegD_E1_E2))) //(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)>|-D V1-V0 V2-V0| 
	{
		//不满足(u+v)<=1 
		return false;
	}

	__m128 det_SubOV0_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE2 }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //|O-V0 V1-V0 V2-V0|

	//    |O-V0 V1-V0 V2-V0|
	//t = -----------------
	//    | -D  V1-V0 V2-V0|

	//t>=0
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>|O-V0 V1-V0 V2-V0|>=0
	if (_mm_movemask_ps(_mm_cmplt_ps(det_SubOV0_E1_E2, _mm_setzero_ps()))) //|O-V0 V1-V0 V2-V0|<0
	{
		//不满足t>=0
		return false;
	}

	if (pt != NULL)
	{
		__m128 vt = _mm_div_ps(det_SubOV0_E1_E2, det_NegD_E1_E2);

		(*pt) = _mm_cvtss_f32(vt);
	}

	return true;
}

inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_CullFront(PTRaySIMD R, PTTriangleSIMD T, float *pt)
{
	//Tomas Moller, Ben Trumbore. "Fast, Minimum Storage Ray/Triangle Intersection." Journal of Graphics Tools, Volume 2, 1997.

	//射线：R = O + tD
	//三角形：T = V0 + u(V1-V0) + v(V2-V0) //设 e1 = V1 - V0 //e2 = V2 - V0

	//[ ]：表示矩阵
	//| |：表示行列式

	//构建线性方程组：R = T
	//=> O + tD = V0 + u(V1-V0) + v(V2-V0)
	//                   [t]
	//=>[-D V1-V0 V2-V0] [u] = O - V0
	//                   [v]

	//克拉默法则
	//[t]           1         | O - V0 V1 - V0 V2 - V0 |
	//[u] = ----------------- |  -D    O - V0  V2 - V0 |
	//[v]   |-D V1-V0 V2-V0|  |  -D    V1 - V0  O - V0 |

	//标量三重积
	//[-D V1-V0 V2-V0] = -D•((V1-V0)×(V2-V0)) = D•(-(V1-V0)×(V2-V0)) 反映了射线方向与法线方向之间的夹角

	//在右手系且逆时针绕序时，-(V1-V0)×(V2-V0)为△V0V1V2的法线方向的相反方向，[-D V1-V0 V2-V0]＞0即表示射线从三角形正面射入

	//向量积D×(V2-V0)的计算结果可以复用：
	//|-D V1-V0 V2-V0| = -|V1-V0 -D V2-V0| = -(-|V1-V0 D V2-V0|) = |V1-V0 D V2-V0| = (V1-V0)•(D×(V2-V0))
	//|-D  O-V0 V2-V0| = -| O-V0 -D V2-V0| = -(-| O-V0 D V2-V0|) = | O-V0 D V2-V0| =  (O-V0)•(D×(V2-V0))

	__m128 vE1 = _mm_sub_ps(T.V1, T.V0);
	__m128 vE2 = _mm_sub_ps(T.V2, T.V0);

	__m128 vCross_D_E2 = ::PTVector3SIMDCross(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vE2 }).m_Data; //D×(V2-V0)

	__m128 det_NegD_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE1 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D V1-V0 V2-V0|

	float fNegEpsilon = -1e-20f;
	__m128 vNegEpsilon = _mm_load_ps1(&fNegEpsilon);

	if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_E1_E2, vNegEpsilon))) //|-D V1-V0 V2-V0|>-Epsilon
	{
		//平行或正面射入
		return false;
	}

	//Division Delay //除以|-D V1-V0 V2-V0|并不一定是必要的

	__m128 vSubOV0 = _mm_sub_ps(R.Origin, T.V0);

	__m128 det_NegD_SubOV0_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D  O-V0 V2-V0|

	//    |-D  O-V0 V2-V0|
	//u = -----------------
	//    |-D V1-V0 V2-V0|

	//u>=0
	//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
	//<=>|-D  O-V0 V2-V0|<=0

	if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_SubOV0_E2, _mm_setzero_ps()))) //|-D  O-V0 V2-V0|>0
	{
		//不满足u>=0
		return false;
	}

	//u<=1
	//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
	//<=>|-D  O-V0 V2-V0|>=|-D V1-V0 V2-V0|

	if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_SubOV0_E2, det_NegD_E1_E2))) //|-D  O-V0 V2-V0|<|-D V1-V0 V2-V0|
	{
		//不满足u<=1

		//根据线性规划，u>=0且v>=0且(u+v)<=1一定有u<=1 
		//判定u<=1是一种EarlyOut

		return false;
	}


	//向量积(O-V0)×(V1-V0)的计算结果可以复用：
	//|O-V0 V1-V0 V2-V0| = -|O-V0 V2-V0 V1-V0| = -(-|V2-V0 O-V0 V1-V0|) = |V2-V0 O-V0 V1-V0| = (V2-V0)•((O-V0)×(V1-V0))
	//| -D  V1-V0  O-V0| = -| -D   O-V0 V1-V0| = -(-|  D   O-V0 V1-V0|) = |  D   O-V0 V1-V0| =       D•((O-V0)×(V1-V0))

	__m128 vCross_SubOV0_E1 = ::PTVector3SIMDCross(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vE1 }).m_Data; //(O-V0)×(V1-V0)

	__m128 det_NegD_E1_SubOV0 = ::PTVector3SIMDDot(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //| -D  V1-V0  O-V0|

	//    |-D V1-V0  O-V0|
	//v = -----------------
	//    |-D V1-V0 V2-V0|

	//v>=0
	//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
	//<=>|-D V1-V0 O-V0|<=0

	if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_E1_SubOV0, _mm_setzero_ps()))) //|-D V1-V0  O-V0|>0
	{
		//不满足v>=0
		return false;
	}

	//(u+v)<=1 
	//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
	//<=>(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)>=|-D V1-V0 V2-V0| 

	if (_mm_movemask_ps(_mm_cmplt_ps(_mm_add_ps(det_NegD_SubOV0_E2, det_NegD_E1_SubOV0), det_NegD_E1_E2))) //(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)<|-D V1-V0 V2-V0| 
	{
		//不满足(u+v)<=1 
		return false;
	}

	__m128 det_SubOV0_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE2 }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //|O-V0 V1-V0 V2-V0|

	//    |O-V0 V1-V0 V2-V0|
	//t = -----------------
	//    | -D  V1-V0 V2-V0|

	//t>=0
	//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
	//<=>|O-V0 V1-V0 V2-V0|<=0
	if (_mm_movemask_ps(_mm_cmpgt_ps(det_SubOV0_E1_E2, _mm_setzero_ps()))) //|O-V0 V1-V0 V2-V0|>0
	{
		//不满足t>=0
		return false;
	}

	if (pt != NULL)
	{
		__m128 vt = _mm_div_ps(det_SubOV0_E1_E2, det_NegD_E1_E2);

		(*pt) = _mm_cvtss_f32(vt);
	}

	return true;
}

inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_NoCull(PTRaySIMD R, PTTriangleSIMD T, float *pt)
{
	//Tomas Moller, Ben Trumbore. "Fast, Minimum Storage Ray/Triangle Intersection." Journal of Graphics Tools, Volume 2, 1997.

	//射线：R = O + tD
	//三角形：T = V0 + u(V1-V0) + v(V2-V0) //设 e1 = V1 - V0 //e2 = V2 - V0

	//[ ]：表示矩阵
	//| |：表示行列式

	//构建线性方程组：R = T
	//=> O + tD = V0 + u(V1-V0) + v(V2-V0)
	//                   [t]
	//=>[-D V1-V0 V2-V0] [u] = O - V0
	//                   [v]

	//克拉默法则
	//[t]           1         | O - V0 V1 - V0 V2 - V0 |
	//[u] = ----------------- |  -D    O - V0  V2 - V0 |
	//[v]   |-D V1-V0 V2-V0|  |  -D    V1 - V0  O - V0 |

	//标量三重积
	//[-D V1-V0 V2-V0] = -D•((V1-V0)×(V2-V0)) = D•(-(V1-V0)×(V2-V0)) 反映了射线方向与法线方向之间的夹角

	//在右手系且逆时针绕序时，-(V1-V0)×(V2-V0)为△V0V1V2的法线方向的相反方向，[-D V1-V0 V2-V0]＞0即表示射线从三角形正面射入

	//向量积D×(V2-V0)的计算结果可以复用：
	//|-D V1-V0 V2-V0| = -|V1-V0 -D V2-V0| = -(-|V1-V0 D V2-V0|) = |V1-V0 D V2-V0| = (V1-V0)•(D×(V2-V0))
	//|-D  O-V0 V2-V0| = -| O-V0 -D V2-V0| = -(-| O-V0 D V2-V0|) = | O-V0 D V2-V0| =  (O-V0)•(D×(V2-V0))

	__m128 vE1 = _mm_sub_ps(T.V1, T.V0);
	__m128 vE2 = _mm_sub_ps(T.V2, T.V0);

	__m128 vCross_D_E2 = ::PTVector3SIMDCross(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vE2 }).m_Data; //D×(V2-V0)

	__m128 det_NegD_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE1 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D V1-V0 V2-V0|

	float fEpsilon = 1e-20f;
	float fNegEpsilon = -1e-20f;
	
	__m128 vEpsilon = _mm_load_ps1(&fEpsilon);
	__m128 vNegEpsilon = _mm_load_ps1(&fNegEpsilon);

	int bIsFront = _mm_movemask_ps(_mm_cmpge_ps(det_NegD_E1_E2, vEpsilon)); //|-D V1-V0 V2-V0|>=Epsilon
	int bIsBack = _mm_movemask_ps(_mm_cmple_ps(det_NegD_E1_E2, vNegEpsilon)); //|-D V1-V0 V2-V0|<=-Epsilon

	//Division Delay //除以|-D V1-V0 V2-V0|并不一定是必要的

	if (bIsFront)
	{
		__m128 vSubOV0 = _mm_sub_ps(R.Origin, T.V0);

		__m128 det_NegD_SubOV0_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D  O-V0 V2-V0|

		//    |-D  O-V0 V2-V0|
		//u = -----------------
		//    |-D V1-V0 V2-V0|

		//u>=0
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>|-D  O-V0 V2-V0|>0

		if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_SubOV0_E2, _mm_setzero_ps()))) //|-D  O-V0 V2-V0|<0
		{
			//不满足u>=0
			return false;
		}

		//u<=1
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>|-D  O-V0 V2-V0|<=|-D V1-V0 V2-V0|

		if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_SubOV0_E2, det_NegD_E1_E2))) //|-D  O-V0 V2-V0|>|-D V1-V0 V2-V0|
		{
			//不满足u<=1

			//根据线性规划，u>=0且v>=0且(u+v)<=1一定有u<=1 
			//判定u<=1是一种EarlyOut

			return false;
		}


		//向量积(O-V0)×(V1-V0)的计算结果可以复用：
		//|O-V0 V1-V0 V2-V0| = -|O-V0 V2-V0 V1-V0| = -(-|V2-V0 O-V0 V1-V0|) = |V2-V0 O-V0 V1-V0| = (V2-V0)•((O-V0)×(V1-V0))
		//| -D  V1-V0  O-V0| = -| -D   O-V0 V1-V0| = -(-|  D   O-V0 V1-V0|) = |  D   O-V0 V1-V0| =       D•((O-V0)×(V1-V0))

		__m128 vCross_SubOV0_E1 = ::PTVector3SIMDCross(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vE1 }).m_Data; //(O-V0)×(V1-V0)

		__m128 det_NegD_E1_SubOV0 = ::PTVector3SIMDDot(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //| -D  V1-V0  O-V0|

		//    |-D V1-V0  O-V0|
		//v = -----------------
		//    |-D V1-V0 V2-V0|

		//v>=0
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>|-D V1-V0 O-V0|>=0

		if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_E1_SubOV0, _mm_setzero_ps()))) //|-D V1-V0  O-V0|<0
		{
			//不满足v>=0
			return false;
		}

		//(u+v)<=1 
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)<=|-D V1-V0 V2-V0| 

		if (_mm_movemask_ps(_mm_cmpgt_ps(_mm_add_ps(det_NegD_SubOV0_E2, det_NegD_E1_SubOV0), det_NegD_E1_E2))) //(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)>|-D V1-V0 V2-V0| 
		{
			//不满足(u+v)<=1 
			return false;
		}

		__m128 det_SubOV0_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE2 }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //|O-V0 V1-V0 V2-V0|

		//    |O-V0 V1-V0 V2-V0|
		//t = -----------------
		//    | -D  V1-V0 V2-V0|

		//t>=0
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>|O-V0 V1-V0 V2-V0|>=0
		if (_mm_movemask_ps(_mm_cmplt_ps(det_SubOV0_E1_E2, _mm_setzero_ps()))) //|O-V0 V1-V0 V2-V0|<0
		{
			//不满足t>=0
			return false;
		}

		if (pt != NULL)
		{
			__m128 vt = _mm_div_ps(det_SubOV0_E1_E2, det_NegD_E1_E2);

			(*pt) = _mm_cvtss_f32(vt);
		}

		return true;
	}
	else if (bIsBack)
	{
		__m128 vSubOV0 = _mm_sub_ps(R.Origin, T.V0);

		__m128 det_NegD_SubOV0_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vCross_D_E2 }).m_Data; //|-D  O-V0 V2-V0|

		//    |-D  O-V0 V2-V0|
		//u = -----------------
		//    |-D V1-V0 V2-V0|

		//u>=0
		//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
		//<=>|-D  O-V0 V2-V0|<=0

		if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_SubOV0_E2, _mm_setzero_ps()))) //|-D  O-V0 V2-V0|>0
		{
			//不满足u>=0
			return false;
		}

		//u<=1
		//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
		//<=>|-D  O-V0 V2-V0|>=|-D V1-V0 V2-V0|

		if (_mm_movemask_ps(_mm_cmplt_ps(det_NegD_SubOV0_E2, det_NegD_E1_E2))) //|-D  O-V0 V2-V0|<|-D V1-V0 V2-V0|
		{
			//不满足u<=1

			//根据线性规划，u>=0且v>=0且(u+v)<=1一定有u<=1 
			//判定u<=1是一种EarlyOut

			return false;
		}


		//向量积(O-V0)×(V1-V0)的计算结果可以复用：
		//|O-V0 V1-V0 V2-V0| = -|O-V0 V2-V0 V1-V0| = -(-|V2-V0 O-V0 V1-V0|) = |V2-V0 O-V0 V1-V0| = (V2-V0)•((O-V0)×(V1-V0))
		//| -D  V1-V0  O-V0| = -| -D   O-V0 V1-V0| = -(-|  D   O-V0 V1-V0|) = |  D   O-V0 V1-V0| =       D•((O-V0)×(V1-V0))

		__m128 vCross_SubOV0_E1 = ::PTVector3SIMDCross(PTVectorSIMD{ vSubOV0 }, PTVectorSIMD{ vE1 }).m_Data; //(O-V0)×(V1-V0)

		__m128 det_NegD_E1_SubOV0 = ::PTVector3SIMDDot(PTVectorSIMD{ R.Direction }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //| -D  V1-V0  O-V0|

		//    |-D V1-V0  O-V0|
		//v = -----------------
		//    |-D V1-V0 V2-V0|

		//v>=0
		//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
		//<=>|-D V1-V0 O-V0|<=0

		if (_mm_movemask_ps(_mm_cmpgt_ps(det_NegD_E1_SubOV0, _mm_setzero_ps()))) //|-D V1-V0  O-V0|>0
		{
			//不满足v>=0
			return false;
		}

		//(u+v)<=1 
		//又|-D V1-V0 V2-V0|<=-Epsilon<0（见上文）
		//<=>(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)>=|-D V1-V0 V2-V0| 

		if (_mm_movemask_ps(_mm_cmplt_ps(_mm_add_ps(det_NegD_SubOV0_E2, det_NegD_E1_SubOV0), det_NegD_E1_E2))) //(|-D O-V0 V2-V0|+|-D V1-V0 O-V0|)<|-D V1-V0 V2-V0| 
		{
			//不满足(u+v)<=1 
			return false;
		}

		__m128 det_SubOV0_E1_E2 = ::PTVector3SIMDDot(PTVectorSIMD{ vE2 }, PTVectorSIMD{ vCross_SubOV0_E1 }).m_Data; //|O-V0 V1-V0 V2-V0|

		//    |O-V0 V1-V0 V2-V0|
		//t = -----------------
		//    | -D  V1-V0 V2-V0|

		//t>=0
		//又|-D V1-V0 V2-V0|>=Epsilon>0（见上文）
		//<=>|O-V0 V1-V0 V2-V0|<=0
		if (_mm_movemask_ps(_mm_cmpgt_ps(det_SubOV0_E1_E2, _mm_setzero_ps()))) //|O-V0 V1-V0 V2-V0|>0
		{
			//不满足t>=0
			return false;
		}

		if (pt != NULL)
		{
			__m128 vt = _mm_div_ps(det_SubOV0_E1_E2, det_NegD_E1_E2);

			(*pt) = _mm_cvtss_f32(vt);
		}

		return true;
	}
	else
	{
		//平行
		return false;
	}
}
