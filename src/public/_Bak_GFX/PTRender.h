#ifndef PT_RENDERCAMERA_H
#define PT_RENDERCAMERA_H

#include "../PTCommon.h"
#include "PTGraphicCommon.h"
#include "../Window/PTWindowOutput.h"

struct ISBRenderWorld;
struct ISBRenderCamera;
struct ISBRenderPhoto;

struct ISBRenderDevice
{
	virtual ISBRenderWorld *WorldCreate() = 0;
	virtual ISBRenderCamera *CameraCreate() = 0;
};

struct ISBRenderWorld
{
	virtual ISBRenderWorld *Destory() = 0;
};


struct ISBRenderCamera
{
	virtual ISBRenderPhoto *Photograph(ISBRenderWorld *pWorld) = 0;
};

struct ISBRenderPhoto
{
	virtual void Present(IPTWindowOutput *pOutput) = 0;
};


#endif