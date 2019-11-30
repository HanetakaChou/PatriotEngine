#ifndef SB_RENDERHOST_H
#define SB_RENDERHOST_H

#include "../SBCommon.h"
#include "SBRenderCommon.h"

#include <stdint.h>

struct ISBRenderInput;
struct ISBRenderWorldNode;
struct ISBRenderMesh;
struct ISBRenderLight;
struct ISBRenderLightOmni;
struct ISBRenderLightSpot;
struct ISBRenderWorld;

struct ISBRenderInput
{
	virtual void MeshCreate(char AssetName[], ISBRenderMesh *pMesh) = 0;
	virtual void LightOmniCreate(char AssetName[], ISBRenderLightOmni *pLight) = 0;
	virtual void LightSpotCreate(char AssetName[], ISBRenderLightSpot *pLight) = 0;
	virtual void WorldCreate(char AssetName[], ISBRenderWorld *pWorld) = 0;
};

struct ISBRenderWorldNode
{
	virtual void Retain() = 0;
	virtual void Release() = 0;
};

struct ISBRenderMesh:ISBRenderWorldNode
{
	
};

struct ISBRenderLight:ISBRenderWorldNode
{

};

struct ISBRenderLightOmni :ISBRenderLight
{

};

struct ISBRenderWorld :ISBRenderWorldNode
{
	virtual void NodeAdd(ISBRenderWorldNode *pNodeToAdd, ISBRenderWorldNode *pParent = NULL) = 0;
	virtual void NodeRemove(ISBRenderWorldNode *pNodeToRemove) = 0;
};

#endif