#include <Windows.h>
#include <xnamath.h>
//#include "../../../../Public/Math/PTMath.h"

#include <math.h>

class Render_Shadow
{
	Render_Shadow()
	{
		//DirectX::XMVector3AngleBetweenVectors
	}

	//CSM
	static inline void CalculateSegmentZFar(
		float SplitCorrection,
		float FrustumZNear, float FrustumZFar,
		float SegmentCount, unsigned SegmentIndex,
		float *pSegmentZFar
	)
	{
		float DistributionFactor = SegmentIndex / SegmentCount;
		float StandardTerm = FrustumZNear * ::powf(FrustumZFar / FrustumZNear, DistributionFactor);
		float CorrectionTerm = FrustumZNear + (FrustumZFar - FrustumZNear)*DistributionFactor;
		*pSegmentZFar= SplitCorrection * StandardTerm + (1.0f - SplitCorrection) * CorrectionTerm;
	}
	
	//PCSS

};