#ifndef PT_GRAPHIC_ASSET_ASSETMESH_H
#define PT_GRAPHIC_ASSET_ASSETMESH_H

#include <stdint.h>

struct PTFORMAT_R32G32B32_FLOAT
{
	float x;
	float y;
	float z;
};

struct PTFORMAT_R8G8B8A8_SNORM
{
	int8_t x;
	int8_t y;
};


struct PTFORMAT_R8G8_UNORM
{
	uint8_t x;
	uint8_t y;
};

struct PTFORMAT_R8G8B8A8_UNORM
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t w;
};

struct PTFORMAT_R8G8B8A8_UINT
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t w;
};

#endif