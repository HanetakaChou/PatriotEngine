#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <assert.h>

#include "../include/glex.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

#define DDS_MAGIC MAKEFOURCC('D','D','S',' ')

struct DDS_PIXELFORMAT
{
	uint32_t    size;
	uint32_t    flags;
	uint32_t    fourCC;
	uint32_t    RGBBitCount;
	uint32_t    RBitMask;
	uint32_t    GBitMask;
	uint32_t    BBitMask;
	uint32_t    ABitMask;
};

#define DDS_FOURCC      0X00000004  // DDPF_FOURCC
#define DDS_RGB         0X00000040	// DDPF_RGB

struct DDS_HEADER
{
	uint32_t        size;
	uint32_t        flags;
	uint32_t        height;
	uint32_t        width;
	uint32_t        pitchOrLinearSize;
	uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	uint32_t        mipMapCount;
	uint32_t        reserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t        caps;
	uint32_t        caps2;
	uint32_t        caps3;
	uint32_t        caps4;
	uint32_t        reserved2;
};

struct DDS_HEADER_DXT10
{
	uint32_t        dxgiFormat;
	uint32_t        resourceDimension;
	uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
	uint32_t        arraySize;
	uint32_t        miscFlags2;
};

#define DXGI_FORMAT_R8G8B8A8_UNORM 28
#define DXGI_FORMAT_BC7_UNORM 98

#define D3D10_RESOURCE_DIMENSION_UNKNOWN 0
#define D3D10_RESOURCE_DIMENSION_BUFFER 1
#define D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define D3D10_RESOURCE_DIMENSION_TEXTURE3D 4

#define D3D10_RESOURCE_MISC_TEXTURECUBE 4

#define D3DFMT_R32F 114
#define D3DFMT_G32R32F 115

GLuint GL_APIENTRY glexCreateDDSTextureFromFile(const char*szFileName)
{
	HANDLE hFile = ::CreateFileA(szFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER FileSize;
	::GetFileSizeEx(hFile, &FileSize);
	HANDLE hSection = ::CreateFileMappingW(hFile, NULL, PAGE_READONLY, FileSize.HighPart, FileSize.LowPart, NULL);
	assert(FileSize.HighPart == 0U);
	void *pFileData = ::MapViewOfFileEx(hSection, SECTION_MAP_READ, 0U, 0U, FileSize.LowPart, NULL);

	assert(*reinterpret_cast<const uint32_t*>(pFileData) == DDS_MAGIC);

	DDS_HEADER *pHeader = reinterpret_cast<DDS_HEADER*>(reinterpret_cast<uintptr_t>(pFileData) + sizeof(uint32_t));

	GLuint hTexture;
	::glGenTextures(1, &hTexture);

	if ((pHeader->ddspf.flags & DDS_FOURCC) && (pHeader->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
	{
		DDS_HEADER_DXT10 *pHeaderDXT10 = reinterpret_cast<DDS_HEADER_DXT10*>(reinterpret_cast<uintptr_t>(pFileData) + sizeof(uint32_t) + sizeof(DDS_HEADER));

		assert(pHeaderDXT10->resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D);
		assert(pHeaderDXT10->arraySize >= 1U);
		assert(pHeader->mipMapCount == 1U);
		assert(pHeaderDXT10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM);

		//GLenum target = GL_TEXTURE_2D_ARRAY;
		//GLint level = 1U;
		//GLenum internalformat = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT;
		GLsizei width = pHeader->width;
		GLsizei height = pHeader->height;
		GLsizei depth = pHeaderDXT10->arraySize;
		//GLsizei imageSize = 4 * pHeader->width *pHeader->height * pHeaderDXT10->arraySize;
		void *pTextureData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pFileData) + sizeof(uint32_t) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10));

		if (pHeaderDXT10->miscFlag&D3D10_RESOURCE_MISC_TEXTURECUBE)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, hTexture);
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height);
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 0));
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 1));
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 2));
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 3));
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 4));
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * 5));
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		else
		{
			assert(pHeaderDXT10->arraySize > 1);
			::glBindTexture(GL_TEXTURE_2D_ARRAY, hTexture);
			::glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, depth);	//::glCompressedTexImage3DOES(target, level, internalformat, width, height, depth, 0, imageSize, NULL);
			for (GLsizei i = 0; i < depth; ++i)
			{
				::glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTextureData) + 4 * width * height * i));
			}
			::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
	}
	else
	{
		GLsizei width = pHeader->width;
		GLsizei height = pHeader->height;
		void *pTextureData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pFileData) + sizeof(uint32_t) + sizeof(DDS_HEADER));

		if (pHeader->ddspf.flags & DDS_FOURCC)
		{
			GLenum internalformat;
			GLenum format;
			GLenum type;

			switch (pHeader->ddspf.fourCC)
			{
			case D3DFMT_R32F:
				internalformat = GL_R32F;
				format = GL_RED;
				type = GL_FLOAT;
				break;
			case D3DFMT_G32R32F:
				internalformat = GL_RG32F;
				format = GL_RG;
				type = GL_FLOAT;
				break;
			default:
				assert(0);
			}

			::glBindTexture(GL_TEXTURE_2D, hTexture);
			::glTexStorage2D(GL_TEXTURE_2D, 1, internalformat, width, height);
			::glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, pTextureData);
			::glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (pHeader->ddspf.flags & DDS_RGB)
		{
			GLenum internalformat;
			GLenum format;
			GLenum type;

			assert(pHeader->ddspf.RGBBitCount == 32U);
			assert(pHeader->ddspf.RBitMask == 0X000000FF);
			assert(pHeader->ddspf.GBitMask == 0X0000FF00);
			assert(pHeader->ddspf.BBitMask == 0X00FF0000);
			assert(pHeader->ddspf.ABitMask == 0XFF000000);
			
			internalformat = GL_RGBA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;

			::glBindTexture(GL_TEXTURE_2D, hTexture);
			::glTexStorage2D(GL_TEXTURE_2D, 1, internalformat, width, height);
			::glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, pTextureData);
			::glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	
	
	::UnmapViewOfFile(pFileData);
	::CloseHandle(hSection);
	::CloseHandle(hFile);

	return hTexture;
}