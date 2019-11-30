//--------------------------------------------------
// Author: 张羽乔
// License: LGPL 3
//--------------------------------------------------

#ifndef PT_MATH_MATH_H
#define PT_MATH_MATH_H

#include <stddef.h>
#include <stdint.h>

#if defined(PTWIN32)
#define PT_VECTORCALL __vectorcall
#elif defined(PTPOSIX)
#if defined(PTX86) || defined(PTX64)
#define PT_VECTORCALL __attribute__((vectorcall))
#elif  defined(PTARM) || defined(PTARM64)
#define PT_VECTORCALL
#else
#error 未知的架构
#endif
#else
#error 未知的平台
#endif

//线性代数

struct PTVector4UI
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
};

struct PTVector2F
{
	float x;
	float y;
};

struct PTVector3F
{
	float x;
	float y;
	float z;
};

struct PTVector4F
{
	float x;
	float y; 
	float z;
	float w;
};

struct PTQuaternionF
{
	float x;
	float y;
	float z;
	float w;
};

struct PTDualQuaternionF
{
	PTQuaternionF r[2];
};

struct PTMatrix4x4F
{
	PTVector4F r[4];
};

struct PTVectorSIMD;//列向量
struct PTQuaternionSIMD;//四元数
struct PTDualQuaternionSIMD;//对偶四元数
struct PTMatrixSIMD;//作用于列向量行序为主序(内存布局等价于作用于行向量列序为主序)

inline PTVectorSIMD PT_VECTORCALL PTVector2FLoad(const PTVector2F *pSource);//x y _ _

inline void PT_VECTORCALL PTVector2FStore(PTVector2F* pDestination, PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector3FLoad(const PTVector3F *pSource);//x y z _

inline void PT_VECTORCALL PTVector3FStore(PTVector3F* pDestination, PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector4FLoad(const PTVector4F *pSource);//x y z w

inline void PT_VECTORCALL PTVector4FStore(PTVector4F* pDestination, PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDReplicate(float Value);

inline float PT_VECTORCALL PTVectorSIMDGetX(PTVectorSIMD V);

inline float PT_VECTORCALL PTVectorSIMDGetY(PTVectorSIMD V);

inline float PT_VECTORCALL PTVectorSIMDGetZ(PTVectorSIMD V);

inline float PT_VECTORCALL PTVectorSIMDGetW(PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDNegate(PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDAdd(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDSubtract(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDMultiply(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVectorSIMDLess(PTVectorSIMD V1, PTVectorSIMD V2);

inline bool PT_VECTORCALL PTVector2SIMDToBool(PTVectorSIMD V);

inline bool PT_VECTORCALL PTVector3SIMDToBool(PTVectorSIMD V);

inline bool PT_VECTORCALL PTVector4SIMDToBool(PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector2SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVector4SIMDDot(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDCross(PTVectorSIMD V1, PTVectorSIMD V2);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDNormalize(PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransformCoord(PTMatrixSIMD M, PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransformTangent(PTMatrixSIMD M, PTVectorSIMD V);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransform(PTQuaternionSIMD qR, PTVectorSIMD vPosition);

inline PTVectorSIMD PT_VECTORCALL PTVector3SIMDTransform(PTDualQuaternionSIMD dqRT, PTVectorSIMD vPosition);

inline PTVectorSIMD PT_VECTORCALL PTVector4SIMDTransform(PTMatrixSIMD M, PTVectorSIMD V);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationMatrix(PTMatrixSIMD mRotation);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationX(float Angle);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationY(float Angle);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationZ(float Angle);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationXYZ(float AngleX, float AngleY, float AngleZ);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationAxis(PTVectorSIMD Axis, float Angle);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDRotationNormalAxis(PTVectorSIMD NormalAxis, float Angle);

inline PTQuaternionSIMD PT_VECTORCALL PTQuaternionSIMDMultiply(PTQuaternionSIMD q0, PTQuaternionSIMD q1);

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDRotationTranslationMatrix(PTMatrixSIMD mRotationTranslation);

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDRotationTranslation(PTQuaternionSIMD qRotation, float fTranslationX, float fTranslationY, float fTranslationZ);

inline PTDualQuaternionSIMD PT_VECTORCALL PTDualQuaternionSIMDMultiply(PTDualQuaternionSIMD dq0, PTDualQuaternionSIMD dq1);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4FLoad(const PTMatrix4x4F* pSource);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4FLoadA(const PTMatrix4x4F* pSource);

inline void PT_VECTORCALL PTMatrix4x4FStore(PTMatrix4x4F* pDestination, PTMatrixSIMD M);

inline void PT_VECTORCALL PTMatrix4x4FStoreA(PTMatrix4x4F* pDestination, PTMatrixSIMD M);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDTranspose(PTMatrixSIMD M);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDTranslation(float OffsetX, float OffsetY, float OffsetZ);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDRotationQuaternion(PTQuaternionSIMD Q);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDLookToRH(PTVectorSIMD EyePosition, PTVectorSIMD EyeDirection, PTVectorSIMD UpDirection, PTMatrixSIMD *pInverse = NULL);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDPerspectiveFovRH(float FovAngleY, float AspectHByW, float NearZ, float FarZ, PTMatrixSIMD *pInverse = NULL);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDPerspectiveRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ, PTMatrixSIMD *pInverse = NULL);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDOrthographicRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ);

inline PTMatrixSIMD PT_VECTORCALL PTMatrix4x4SIMDMultiply(PTMatrixSIMD M1, PTMatrixSIMD M2);


//相交测试

enum PTIntersectionType
{
	PTDISJOINT = 0,//互斥
	PTINTERSECT = 1,//相交
	PTCONTAIN = 2,//包含
};

struct PTFrustumF
{
	PTVector4F Plane[6];
};

struct PTAABF
{
	PTVector3F Center;
	PTVector3F HalfDiagonal;
};

struct PTFrustumSIMD;
struct PTSphereSIMD;
struct PTAABSIMD;
struct PTOBSIMD;

inline PTFrustumSIMD PT_VECTORCALL PTFrustumSIMDLoadRH(PTMatrixSIMD ViewProjection);

inline void PT_VECTORCALL PTFrustumFStore(PTFrustumF *pDestination, PTFrustumSIMD F);
inline void PT_VECTORCALL PTFrustumFStoreA(PTFrustumF *pDestination, PTFrustumSIMD F);

inline PTSphereSIMD PT_VECTORCALL PTSphereSIMDLoadFromPoint(uint32_t Count, PTVector4F const *pPointArray, uint32_t Stride);
inline PTSphereSIMD PT_VECTORCALL PTSphereSIMDLoad(PTVector3F *pCenter,float Radius);
inline PTIntersectionType PT_VECTORCALL PTFrustumSIMDSphereSIMDIntersect(PTFrustumSIMD F, PTSphereSIMD S);

inline PTAABSIMD PT_VECTORCALL PTAABSIMDLoadFromPoint(uint32_t Count, PTVector4F const *pPointArray, uint32_t Stride);
inline PTAABSIMD PT_VECTORCALL PTAABSIMDLoad(PTAABF *pSource);
inline void PT_VECTORCALL PTAABFStore(PTAABF *pDestination, PTAABSIMD AAB);
inline PTIntersectionType PT_VECTORCALL PTFrustumSIMDAABSIMDIntersect(PTFrustumSIMD F, PTAABSIMD AAB);

//inline PTSIMDOB PT_VECTORCALL PTSIMDOBLoad(PTV3F *pCenter, PTV3F *pHalfDiagonal, PTSIMDMatrix Rotation);
//inline PTSIMDOB PT_VECTORCALL PTSIMDOBLoad(PTV3F *pCenter, PTV3F *pHalfDiagonal, PTV3F *pRotationXYZ);



//inline PTIntersectionType PT_VECTORCALL PTFrustumSIMDOBSIMDIntersect(PTSIMDFrustum F, PTSIMDOB OB);

struct PTRaySIMD;
struct PTTriangleSIMD;

inline PTRaySIMD PT_VECTORCALL PTRaySIMDLoad(PTVector3F *pOrigin, PTVector3F *pDirection);

inline bool PT_VECTORCALL PTRaySIMDSphereSIMDIntersect(PTRaySIMD R, PTSphereSIMD S, float *pt = NULL);

inline PTTriangleSIMD PT_VECTORCALL PTTriangleSIMDLoad(PTVector3F *pV0, PTVector3F *pV1, PTVector3F *pV2);

//右手系 //逆时针环绕顺序 //保留从正面射入
inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_CullBack(PTRaySIMD R, PTTriangleSIMD T, float *pt = NULL);

//右手系 //逆时针环绕顺序 //保留从背面射入
inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_CullFront(PTRaySIMD R, PTTriangleSIMD T, float *pt = NULL);

//No Cull
inline bool PT_VECTORCALL PTRaySIMDTriangleSIMDIntersect_NoCull(PTRaySIMD R, PTTriangleSIMD T, float *pt = NULL);

#if defined(PTX86) || defined(PTX64)
#include <xmmintrin.h>
#include <emmintrin.h>
#include "SSE/PTMathSIMD.inl"
#elif defined(PTARM) || defined(PTARM64)
#include <arm_neon.h>
#include "NEON/PTMathSIMD.inl"
#else
#error 未知的架构
#endif

#endif