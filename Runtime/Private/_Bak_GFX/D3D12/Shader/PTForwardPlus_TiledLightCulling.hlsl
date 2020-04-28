//
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

//AMD GPUOpen ForwardPlus11
//http://gpuopen.com/gaming-product/forwardplus11-directx-11-sdk-sample


//--------------------------------------------------------------------------------------
// File: ForwardPlus11.hlsl
//
// HLSL file for the ForwardPlus11 sample. Tiled light culling.
//--------------------------------------------------------------------------------------


#include "ForwardPlus11Common.hlsl"

//-----------------------------------------------------------------------------------------
// Textures and Buffers
//-----------------------------------------------------------------------------------------
Buffer<float4> g_PointLightBufferCenterAndRadius : register( t0 );
Buffer<float4> g_SpotLightBufferCenterAndRadius : register( t1 );

#if ( USE_DEPTH_BOUNDS == 1 )   // non-MSAA
Texture2D<float> g_DepthTexture : register( t2 );
#endif

RWTexture3D<uint> g_PerTileLightIndexBufferOut : register( u0 );

//-----------------------------------------------------------------------------------------
// Parameters for the light culling shader
//-----------------------------------------------------------------------------------------
#define NUM_THREADS_X TILE_RES
#define NUM_THREADS_Y TILE_RES
#define NUM_THREADS_PER_TILE (NUM_THREADS_X*NUM_THREADS_Y)

//-----------------------------------------------------------------------------------------
// Group Shared Memory (aka local data share, or LDS)
//-----------------------------------------------------------------------------------------
#if ( USE_DEPTH_BOUNDS == 1 )
groupshared uint ldsZMax;
groupshared uint ldsZMin;
#endif

groupshared uint ldsLightIdxCounter;
groupshared uint ldsLightIdx[MAX_NUM_LIGHTS_PER_TILE];

//-----------------------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------------------

// this creates the standard Hessian-normal-form plane equation from three points, 
// except it is simplified for the case where the first point is the origin
float3 CreatePlaneEquation( float3 b, float3 c )
{
    // normalize(cross( b-a, c-a )), except we know "a" is the origin
    // also, typically there would be a fourth term of the plane equation, 
    // -(n dot a), except we know "a" is the origin
    return normalize(cross(b,c));;
}

// point-plane distance, simplified for the case where 
// the plane passes through the origin
float GetSignedDistanceFromPlane( float3 p, float3 eqn )
{
    // dot(eqn.xyz,p) + eqn.w, , except we know eqn.w is zero 
    // (see CreatePlaneEquation above)
    return dot(eqn,p);
}

bool TestFrustumSides( float3 c, float r, float3 plane0, float3 plane1, float3 plane2, float3 plane3 )
{
    bool intersectingOrInside0 = GetSignedDistanceFromPlane( c, plane0 ) < r;
    bool intersectingOrInside1 = GetSignedDistanceFromPlane( c, plane1 ) < r;
    bool intersectingOrInside2 = GetSignedDistanceFromPlane( c, plane2 ) < r;
    bool intersectingOrInside3 = GetSignedDistanceFromPlane( c, plane3 ) < r;

    return (intersectingOrInside0 && intersectingOrInside1 && 
            intersectingOrInside2 && intersectingOrInside3);
}

// calculate the number of tiles in the horizontal direction
uint GetNumTilesX()
{
    return (uint)( ( g_uWindowWidth + TILE_RES - 1 ) / (float)TILE_RES );
}

// calculate the number of tiles in the vertical direction
uint GetNumTilesY()
{
    return (uint)( ( g_uWindowHeight + TILE_RES - 1 ) / (float)TILE_RES );
}

// convert a point from post-projection space into view space
float4 ConvertProjToView( float4 p )
{
    p = mul( p, g_mProjectionInv );
    p /= p.w;
    return p;
}

//-----------------------------------------------------------------------------------------
// Light culling shader
//-----------------------------------------------------------------------------------------
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void CullLightsCS( uint3 globalIdx : SV_DispatchThreadID, uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID )
{
    uint localIdxFlattened = localIdx.x + localIdx.y*NUM_THREADS_X;

	float3 frustumEqn0;
	float3 frustumEqn1;
	float3 frustumEqn2;
	float3 frustumEqn3;
    {   
		//创建视锥体的上下左右4个平面
		//正半空间在视锥体外

		//Gil Gribb  Klaus Hartmann,"Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix", 06/15/2001
		//http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

		uint uWindowWidthEvenlyDivisibleByTileRes = TILE_RES*GetNumTilesX();
		uint uWindowHeightEvenlyDivisibleByTileRes = TILE_RES*GetNumTilesY();

		float XLeft = (float(TILE_RES*groupIdx.x) / float(uWindowWidthEvenlyDivisibleByTileRes))*2.0f - 1.0f;
		float XRight = (float(TILE_RES*(groupIdx.x + 1)) / float(uWindowWidthEvenlyDivisibleByTileRes))*2.0f - 1.0f;
		float YTop = 1.0f - (float(TILE_RES*groupIdx.y) / float(uWindowHeightEvenlyDivisibleByTileRes))*2.0f;
		float YBottom = 1.0f - (float(TILE_RES*(groupIdx.y + 1)) / float(uWindowHeightEvenlyDivisibleByTileRes))*2.0f;

		//以LeftPlane为例
		//由于正半空间在视锥体外
		//->[x y z 1][P.C0] / [x y z 1][P.C3] < XLeft
		//由于[x y z 1][P.C1]大于0（在右手系中，会乘以-1，最终结果也是大于0的）
		//-> [x y z 1][P.C0] < [x y z 1][P.C3]*XLeft
		//-> [x y z 1]([P.C3]*XLeft - [P.C0]) > 0
		//由于平面[A B C D]被定义为[x y z 1][A B C D]=0
		//->[A B C D] = [P.C3]*XLeft - [P.C0]
		//归一化
		//->[A B C D] /= length([A B C D].xyz)
		float4 FrustumPlaneLeft = float4(g_mProjection._m03_m13_m23_m33)*XLeft - float4(g_mProjection._m00_m10_m20_m30);
		FrustumPlaneLeft /= length(FrustumPlaneLeft.xyz);

		//[x y z 1][P.C0] / [x y z 1][P.C3] > XRight
		//[x y z 1]([P.C0] - [P.C3]*XRight) > 0
		float4 FrustumPlaneRight = float4(g_mProjection._m00_m10_m20_m30) - float4(g_mProjection._m03_m13_m23_m33)*XRight;
		FrustumPlaneRight /= length(FrustumPlaneRight.xyz);

		//[x y z 1][P.C1] / [x y z 1][P.C3] > YTop
		//[x y z 1]([P.C1] - [P.C3]*YTop) > 0
		float4 FrustumPlaneTop = float4(g_mProjection._m01_m11_m21_m31) - float4(g_mProjection._m03_m13_m23_m33)*YTop;
		FrustumPlaneTop /= length(FrustumPlaneTop.xyz);

		//[x y z 1][P.C1] / [x y z 1][P.C3] < YBottom
		//[x y z 1]([P.C3]*YBottom - [P.C1]) > 0
		float4 FrustumPlaneBottom = float4(g_mProjection._m03_m13_m23_m33)*YBottom - float4(g_mProjection._m01_m11_m21_m31);
		FrustumPlaneBottom /= length(FrustumPlaneBottom.xyz);
    
		//由于在ViewSpace中，视锥体的顶点一定经过原点，因此以上4个平面的w分量一定为0
		frustumEqn0 = FrustumPlaneTop.xyz;
		frustumEqn1 = FrustumPlaneRight.xyz;
		frustumEqn2 = FrustumPlaneBottom.xyz;
		frustumEqn3 = FrustumPlaneLeft.xyz;
    }

    // calculate the min and max depth for this tile, 
    // to form the front and back of the frustum
#if ( USE_DEPTH_BOUNDS == 1 )   // non-MSAA
	//初始化
	if (localIdxFlattened == 0)
	{
		ldsZMax = 0;
		ldsZMin = 0x7F7FFFFF;//asuint(FLT_MAX) IEEE float
	}
	
	GroupMemoryBarrierWithGroupSync();

	//映射
	float2 PositionClipZW = float2(g_DepthTexture.mips[0][globalIdx.xy], 1.0f);
	//---------
	//|b 1|
	//|a 0|
	//---------
	//|0 1/a|
	//|1 -b/a|
	//---------
	//dot(ClipZW, g_mProjectionInv._m22_m32) = Depth * 0 + 1 * 1 = 1
	//---------
	//float2 ViewZW = float2(1.0f, dot(ClipZW, g_mProjectionInv._m23_m33)); 
	//以上结论并不适用于正交投影
	float2 PositionViewZW = mul(PositionClipZW, float2x2(g_mProjectionInv._m22_m23, g_mProjectionInv._m32_m33));
	PositionViewZW.xy /= PositionViewZW.yy;
	
	uint PositionViewZ_U = asuint(PositionViewZW.x);

	//归约？？？
	InterlockedMax(ldsZMax, PositionViewZ_U);
	InterlockedMin(ldsZMin, PositionViewZ_U);
	
	GroupMemoryBarrierWithGroupSync();

	float maxZ = asfloat(ldsZMax);
	float minZ = asfloat(ldsZMin);
#endif

	if (localIdxFlattened == 0)
	{
		ldsLightIdxCounter = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	//映射
    // loop over the lights and do a sphere vs. frustum intersection test
    uint uNumPointLights = g_uNumLights & 0xFFFFu;
    for(uint i=localIdxFlattened; i<uNumPointLights; i+=NUM_THREADS_PER_TILE)
    {
        float4 center = g_PointLightBufferCenterAndRadius[i];
        float r = center.w;
        center.xyz = mul( float4(center.xyz, 1), g_mWorldView ).xyz;

        // test if sphere is intersecting or inside frustum
        if( TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3) )
        {
#if ( USE_DEPTH_BOUNDS != 0 )
			//在ViewSpace中，视锥体的NearZ和FarZ平面的法线方向是显然的，并不需要计算
            if( -center.z + minZ < r && center.z - maxZ < r )
#else
            if( -center.z < r )
#endif
            {

				//归约
                // do a thread-safe increment of the list counter 
                // and put the index of this light into the list
                uint dstIdx = 0;
                InterlockedAdd( ldsLightIdxCounter, 1, dstIdx );
                ldsLightIdx[dstIdx] = i;
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    // and again for spot lights
    uint uNumPointLightsInThisTile = ldsLightIdxCounter;
    uint uNumSpotLights = (g_uNumLights & 0xFFFF0000u) >> 16;
    for(uint j=localIdxFlattened; j<uNumSpotLights; j+=NUM_THREADS_PER_TILE)
    {
        float4 center = g_SpotLightBufferCenterAndRadius[j];
        float r = center.w;
        center.xyz = mul( float4(center.xyz, 1), g_mWorldView ).xyz;

        // test if sphere is intersecting or inside frustum
        if( TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3) )
        {
#if ( USE_DEPTH_BOUNDS != 0 )
            if( -center.z + minZ < r && center.z - maxZ < r )
#else
            if( -center.z < r )
#endif
            {
                // do a thread-safe increment of the list counter 
                // and put the index of this light into the list
                uint dstIdx = 0;
                InterlockedAdd( ldsLightIdxCounter, 1, dstIdx );
                ldsLightIdx[dstIdx] = j;
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    {   // write back
		uint2 startOffset = groupIdx.xy;

		for (uint i = localIdxFlattened; i<uNumPointLightsInThisTile; i += NUM_THREADS_PER_TILE)
		{
			// per-tile list of light indices
			g_PerTileLightIndexBufferOut[uint3(startOffset, i)] = ldsLightIdx[i];
		}

		for (uint j = (localIdxFlattened + uNumPointLightsInThisTile); j<ldsLightIdxCounter; j += NUM_THREADS_PER_TILE)
		{
			// per-tile list of light indices
			g_PerTileLightIndexBufferOut[uint3(startOffset, j + 1)] = ldsLightIdx[j];
		}

        if( localIdxFlattened == 0 )
        {
            // mark the end of each per-tile list with a sentinel (point lights)
			g_PerTileLightIndexBufferOut[uint3(startOffset, uNumPointLightsInThisTile)] = LIGHT_INDEX_BUFFER_SENTINEL;

            // mark the end of each per-tile list with a sentinel (spot lights)
			g_PerTileLightIndexBufferOut[uint3(startOffset, ldsLightIdxCounter + 1)] = LIGHT_INDEX_BUFFER_SENTINEL;
        }
    }
}


