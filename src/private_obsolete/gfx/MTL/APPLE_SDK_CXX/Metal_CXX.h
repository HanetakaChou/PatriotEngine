#ifndef _METAL_CXX_H_
#define _METAL_CXX_H_ 1

#include "NSRuntime_CXX.h"
#include "Dispatch_CXX.h"

extern "C" struct MTLDevice *MTLCreateSystemDefaultDevice(void);

typedef NSUInteger MTLFeatureSet;
enum
{
    MTLFeatureSet_iOS_GPUFamily1_v1 __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 0,
    MTLFeatureSet_iOS_GPUFamily2_v1 __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 1,

    MTLFeatureSet_iOS_GPUFamily1_v2 __attribute__((availability(ios, introduced = 9.0), availability(macos, unavailable))) = 2,
    MTLFeatureSet_iOS_GPUFamily2_v2 __attribute__((availability(ios, introduced = 9.0), availability(macos, unavailable))) = 3,
    MTLFeatureSet_iOS_GPUFamily3_v1 __attribute__((availability(ios, introduced = 9.0), availability(macos, unavailable))) = 4,

    MTLFeatureSet_iOS_GPUFamily1_v3 __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 5,
    MTLFeatureSet_iOS_GPUFamily2_v3 __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 6,
    MTLFeatureSet_iOS_GPUFamily3_v2 __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 7,

    MTLFeatureSet_iOS_GPUFamily1_v4 __attribute__((availability(ios, introduced = 11.0), availability(macos, unavailable))) = 8,
    MTLFeatureSet_iOS_GPUFamily2_v4 __attribute__((availability(ios, introduced = 11.0), availability(macos, unavailable))) = 9,
    MTLFeatureSet_iOS_GPUFamily3_v3 __attribute__((availability(ios, introduced = 11.0), availability(macos, unavailable))) = 10,
    MTLFeatureSet_iOS_GPUFamily4_v1 __attribute__((availability(ios, introduced = 11.0), availability(macos, unavailable))) = 11,

    MTLFeatureSet_iOS_GPUFamily1_v5 __attribute__((availability(ios, introduced = 12.0), availability(macos, unavailable))) = 12,
    MTLFeatureSet_iOS_GPUFamily2_v5 __attribute__((availability(ios, introduced = 12.0), availability(macos, unavailable))) = 13,
    MTLFeatureSet_iOS_GPUFamily3_v4 __attribute__((availability(ios, introduced = 12.0), availability(macos, unavailable))) = 14,
    MTLFeatureSet_iOS_GPUFamily4_v2 __attribute__((availability(ios, introduced = 12.0), availability(macos, unavailable))) = 15,

    MTLFeatureSet_macOS_GPUFamily1_v1 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 10000,
    MTLFeatureSet_OSX_GPUFamily1_v1 __attribute__((availability(ios, unavailable), availability(macos, deprecated = 10.11))) = MTLFeatureSet_macOS_GPUFamily1_v1,

    MTLFeatureSet_macOS_GPUFamily1_v2 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.12))) = 10001,
    MTLFeatureSet_OSX_GPUFamily1_v2 __attribute__((availability(ios, unavailable), availability(macos, deprecated = 10.12))) = MTLFeatureSet_macOS_GPUFamily1_v2,
    MTLFeatureSet_macOS_ReadWriteTextureTier2 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.12))) = 10002,
    MTLFeatureSet_OSX_ReadWriteTextureTier2 __attribute__((availability(ios, unavailable), availability(macos, deprecated = 10.12))) = MTLFeatureSet_macOS_ReadWriteTextureTier2,

    MTLFeatureSet_macOS_GPUFamily1_v3 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.13))) = 10003,

    MTLFeatureSet_macOS_GPUFamily1_v4 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.14))) = 10004,
    MTLFeatureSet_macOS_GPUFamily2_v1 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.14))) = 10005,
};

bool MTLDevice_supportsFeatureSet(struct MTLDevice *self, MTLFeatureSet featureSet);

typedef NSUInteger MTLPixelFormat;
enum
{
    MTLPixelFormatInvalid = 0,

    /* Normal 8 bit formats */

    MTLPixelFormatA8Unorm = 1,

    MTLPixelFormatR8Unorm = 10,
    MTLPixelFormatR8Unorm_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 11,

    MTLPixelFormatR8Snorm = 12,
    MTLPixelFormatR8Uint = 13,
    MTLPixelFormatR8Sint = 14,

    /* Normal 16 bit formats */

    MTLPixelFormatR16Unorm = 20,
    MTLPixelFormatR16Snorm = 22,
    MTLPixelFormatR16Uint = 23,
    MTLPixelFormatR16Sint = 24,
    MTLPixelFormatR16Float = 25,

    MTLPixelFormatRG8Unorm = 30,
    MTLPixelFormatRG8Unorm_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 31,
    MTLPixelFormatRG8Snorm = 32,
    MTLPixelFormatRG8Uint = 33,
    MTLPixelFormatRG8Sint = 34,

    /* Packed 16 bit formats */

    MTLPixelFormatB5G6R5Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 40,
    MTLPixelFormatA1BGR5Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 41,
    MTLPixelFormatABGR4Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 42,
    MTLPixelFormatBGR5A1Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 43,

    /* Normal 32 bit formats */

    MTLPixelFormatR32Uint = 53,
    MTLPixelFormatR32Sint = 54,
    MTLPixelFormatR32Float = 55,

    MTLPixelFormatRG16Unorm = 60,
    MTLPixelFormatRG16Snorm = 62,
    MTLPixelFormatRG16Uint = 63,
    MTLPixelFormatRG16Sint = 64,
    MTLPixelFormatRG16Float = 65,

    MTLPixelFormatRGBA8Unorm = 70,
    MTLPixelFormatRGBA8Unorm_sRGB = 71,
    MTLPixelFormatRGBA8Snorm = 72,
    MTLPixelFormatRGBA8Uint = 73,
    MTLPixelFormatRGBA8Sint = 74,

    MTLPixelFormatBGRA8Unorm = 80,
    MTLPixelFormatBGRA8Unorm_sRGB = 81,

    /* Packed 32 bit formats */

    MTLPixelFormatRGB10A2Unorm = 90,
    MTLPixelFormatRGB10A2Uint = 91,

    MTLPixelFormatRG11B10Float = 92,
    MTLPixelFormatRGB9E5Float = 93,

    MTLPixelFormatBGR10A2Unorm __attribute__((availability(ios, introduced = 11.0), availability(macos, introduced = 10.13))) = 94,

    MTLPixelFormatBGR10_XR __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 554,
    MTLPixelFormatBGR10_XR_sRGB __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 555,

    /* Normal 64 bit formats */

    MTLPixelFormatRG32Uint = 103,
    MTLPixelFormatRG32Sint = 104,
    MTLPixelFormatRG32Float = 105,

    MTLPixelFormatRGBA16Unorm = 110,
    MTLPixelFormatRGBA16Snorm = 112,
    MTLPixelFormatRGBA16Uint = 113,
    MTLPixelFormatRGBA16Sint = 114,
    MTLPixelFormatRGBA16Float = 115,

    MTLPixelFormatBGRA10_XR __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 552,
    MTLPixelFormatBGRA10_XR_sRGB __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 553,

    /* Normal 128 bit formats */

    MTLPixelFormatRGBA32Uint = 123,
    MTLPixelFormatRGBA32Sint = 124,
    MTLPixelFormatRGBA32Float = 125,

    /* Compressed formats. */

    /* S3TC/DXT */
    MTLPixelFormatBC1_RGBA __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 130,
    MTLPixelFormatBC1_RGBA_sRGB __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 131,
    MTLPixelFormatBC2_RGBA __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 132,
    MTLPixelFormatBC2_RGBA_sRGB __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 133,
    MTLPixelFormatBC3_RGBA __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 134,
    MTLPixelFormatBC3_RGBA_sRGB __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 135,

    /* RGTC */
    MTLPixelFormatBC4_RUnorm __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 140,
    MTLPixelFormatBC4_RSnorm __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 141,
    MTLPixelFormatBC5_RGUnorm __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 142,
    MTLPixelFormatBC5_RGSnorm __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 143,

    /* BPTC */
    MTLPixelFormatBC6H_RGBFloat __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 150,
    MTLPixelFormatBC6H_RGBUfloat __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 151,
    MTLPixelFormatBC7_RGBAUnorm __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 152,
    MTLPixelFormatBC7_RGBAUnorm_sRGB __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 153,

    /* PVRTC */
    MTLPixelFormatPVRTC_RGB_2BPP __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 160,
    MTLPixelFormatPVRTC_RGB_2BPP_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 161,
    MTLPixelFormatPVRTC_RGB_4BPP __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 162,
    MTLPixelFormatPVRTC_RGB_4BPP_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 163,
    MTLPixelFormatPVRTC_RGBA_2BPP __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 164,
    MTLPixelFormatPVRTC_RGBA_2BPP_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 165,
    MTLPixelFormatPVRTC_RGBA_4BPP __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 166,
    MTLPixelFormatPVRTC_RGBA_4BPP_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 167,

    /* ETC2 */
    MTLPixelFormatEAC_R11Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 170,
    MTLPixelFormatEAC_R11Snorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 172,
    MTLPixelFormatEAC_RG11Unorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 174,
    MTLPixelFormatEAC_RG11Snorm __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 176,
    MTLPixelFormatEAC_RGBA8 __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 178,
    MTLPixelFormatEAC_RGBA8_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 179,

    MTLPixelFormatETC2_RGB8 __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 180,
    MTLPixelFormatETC2_RGB8_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 181,
    MTLPixelFormatETC2_RGB8A1 __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 182,
    MTLPixelFormatETC2_RGB8A1_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 183,

    /* ASTC */
    MTLPixelFormatASTC_4x4_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 186,
    MTLPixelFormatASTC_5x4_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 187,
    MTLPixelFormatASTC_5x5_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 188,
    MTLPixelFormatASTC_6x5_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 189,
    MTLPixelFormatASTC_6x6_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 190,
    MTLPixelFormatASTC_8x5_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 192,
    MTLPixelFormatASTC_8x6_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 193,
    MTLPixelFormatASTC_8x8_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 194,
    MTLPixelFormatASTC_10x5_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 195,
    MTLPixelFormatASTC_10x6_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 196,
    MTLPixelFormatASTC_10x8_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 197,
    MTLPixelFormatASTC_10x10_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 198,
    MTLPixelFormatASTC_12x10_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 199,
    MTLPixelFormatASTC_12x12_sRGB __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 200,

    MTLPixelFormatASTC_4x4_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 204,
    MTLPixelFormatASTC_5x4_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 205,
    MTLPixelFormatASTC_5x5_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 206,
    MTLPixelFormatASTC_6x5_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 207,
    MTLPixelFormatASTC_6x6_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 208,
    MTLPixelFormatASTC_8x5_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 210,
    MTLPixelFormatASTC_8x6_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 211,
    MTLPixelFormatASTC_8x8_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 212,
    MTLPixelFormatASTC_10x5_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 213,
    MTLPixelFormatASTC_10x6_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 214,
    MTLPixelFormatASTC_10x8_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 215,
    MTLPixelFormatASTC_10x10_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 216,
    MTLPixelFormatASTC_12x10_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 217,
    MTLPixelFormatASTC_12x12_LDR __attribute__((availability(ios, introduced = 8.0), availability(macos, unavailable))) = 218,

    /*!
     @constant MTLPixelFormatGBGR422
     @abstract A pixel format where the red and green channels are subsampled horizontally.  Two pixels are stored in 32 bits, with shared red and blue values, and unique green values.
     @discussion This format is equivalent to YUY2, YUYV, yuvs, or GL_RGB_422_APPLE/GL_UNSIGNED_SHORT_8_8_REV_APPLE.   The component order, from lowest addressed byte to highest, is Y0, Cb, Y1, Cr.  There is no implicit colorspace conversion from YUV to RGB, the shader will receive (Cr, Y, Cb, 1).  422 textures must have a width that is a multiple of 2, and can only be used for 2D non-mipmap textures.  When sampling, ClampToEdge is the only usable wrap mode.
     */
    MTLPixelFormatGBGR422 = 240,

    /*!
     @constant MTLPixelFormatBGRG422
     @abstract A pixel format where the red and green channels are subsampled horizontally.  Two pixels are stored in 32 bits, with shared red and blue values, and unique green values.
     @discussion This format is equivalent to UYVY, 2vuy, or GL_RGB_422_APPLE/GL_UNSIGNED_SHORT_8_8_APPLE. The component order, from lowest addressed byte to highest, is Cb, Y0, Cr, Y1.  There is no implicit colorspace conversion from YUV to RGB, the shader will receive (Cr, Y, Cb, 1).  422 textures must have a width that is a multiple of 2, and can only be used for 2D non-mipmap textures.  When sampling, ClampToEdge is the only usable wrap mode.
     */
    MTLPixelFormatBGRG422 = 241,

    /* Depth */

    MTLPixelFormatDepth16Unorm __attribute__((availability(ios, introduced = 13.0), availability(macos, introduced = 10.12))) = 250,
    MTLPixelFormatDepth32Float = 252,

    /* Stencil */

    MTLPixelFormatStencil8 = 253,

    /* Depth Stencil */

    MTLPixelFormatDepth24Unorm_Stencil8 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 255,
    MTLPixelFormatDepth32Float_Stencil8 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 260,

    MTLPixelFormatX32_Stencil8 __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.12))) = 261,
    MTLPixelFormatX24_Stencil8 __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.12))) = 262,
};

typedef NSUInteger MTLVertexStepFunction;
enum
{
    MTLVertexStepFunctionConstant = 0,
    MTLVertexStepFunctionPerVertex = 1,
    MTLVertexStepFunctionPerInstance = 2,
    MTLVertexStepFunctionPerPatch = 3,
    MTLVertexStepFunctionPerPatchControlPoint = 4
};

typedef NSUInteger MTLVertexFormat;
enum
{
    MTLVertexFormatInvalid = 0,

    MTLVertexFormatUChar2 = 1,
    MTLVertexFormatUChar3 = 2,
    MTLVertexFormatUChar4 = 3,

    MTLVertexFormatChar2 = 4,
    MTLVertexFormatChar3 = 5,
    MTLVertexFormatChar4 = 6,

    MTLVertexFormatUChar2Normalized = 7,
    MTLVertexFormatUChar3Normalized = 8,
    MTLVertexFormatUChar4Normalized = 9,

    MTLVertexFormatChar2Normalized = 10,
    MTLVertexFormatChar3Normalized = 11,
    MTLVertexFormatChar4Normalized = 12,

    MTLVertexFormatUShort2 = 13,
    MTLVertexFormatUShort3 = 14,
    MTLVertexFormatUShort4 = 15,

    MTLVertexFormatShort2 = 16,
    MTLVertexFormatShort3 = 17,
    MTLVertexFormatShort4 = 18,

    MTLVertexFormatUShort2Normalized = 19,
    MTLVertexFormatUShort3Normalized = 20,
    MTLVertexFormatUShort4Normalized = 21,

    MTLVertexFormatShort2Normalized = 22,
    MTLVertexFormatShort3Normalized = 23,
    MTLVertexFormatShort4Normalized = 24,

    MTLVertexFormatHalf2 = 25,
    MTLVertexFormatHalf3 = 26,
    MTLVertexFormatHalf4 = 27,

    MTLVertexFormatFloat = 28,
    MTLVertexFormatFloat2 = 29,
    MTLVertexFormatFloat3 = 30,
    MTLVertexFormatFloat4 = 31,

    MTLVertexFormatInt = 32,
    MTLVertexFormatInt2 = 33,
    MTLVertexFormatInt3 = 34,
    MTLVertexFormatInt4 = 35,

    MTLVertexFormatUInt = 36,
    MTLVertexFormatUInt2 = 37,
    MTLVertexFormatUInt3 = 38,
    MTLVertexFormatUInt4 = 39,

    MTLVertexFormatInt1010102Normalized = 40,
    MTLVertexFormatUInt1010102Normalized = 41,

    MTLVertexFormatUChar4Normalized_BGRA = 42,

    MTLVertexFormatUChar = 45,
    MTLVertexFormatChar = 46,
    MTLVertexFormatUCharNormalized = 47,
    MTLVertexFormatCharNormalized = 48,

    MTLVertexFormatUShort = 49,
    MTLVertexFormatShort = 50,
    MTLVertexFormatUShortNormalized = 51,
    MTLVertexFormatShortNormalized = 52,

    MTLVertexFormatHalf = 53
};

struct MTLVertexDescriptor *MTLVertexDescriptor_alloc();
struct MTLVertexDescriptor *MTLVertexDescriptor_init(struct MTLVertexDescriptor *self);
struct MTLVertexBufferLayoutDescriptorArray *MTLVertexDescriptor_layouts(struct MTLVertexDescriptor *self);
struct MTLVertexBufferLayoutDescriptor *MTLVertexBufferLayoutDescriptorArray_objectAtIndexedSubscript(struct MTLVertexBufferLayoutDescriptorArray *self, NSUInteger index);
void MTLVertexBufferLayoutDescriptor_setStride(struct MTLVertexBufferLayoutDescriptor *self, NSUInteger stride);
void MTLVertexBufferLayoutDescriptor_setStepFunction(struct MTLVertexBufferLayoutDescriptor *self, MTLVertexStepFunction stepFunction);
void MTLVertexBufferLayoutDescriptor_setStepRate(struct MTLVertexBufferLayoutDescriptor *self, NSUInteger stepRate);
struct MTLVertexAttributeDescriptorArray *MTLVertexDescriptor_attributes(struct MTLVertexDescriptor *self);
struct MTLVertexAttributeDescriptor *MTLVertexAttributeDescriptorArray_objectAtIndexedSubscript(struct MTLVertexAttributeDescriptorArray *self, NSUInteger index);
void MTLVertexAttributeDescriptor_setFormat(struct MTLVertexAttributeDescriptor *self, MTLVertexFormat format);
void MTLVertexAttributeDescriptor_setOffset(struct MTLVertexAttributeDescriptor *self, NSUInteger offset);
void MTLVertexAttributeDescriptor_setBufferIndex(struct MTLVertexAttributeDescriptor *self, NSUInteger bufferIndex);
void MTLVertexDescriptor_release(struct MTLVertexDescriptor *self);
NSUInteger MTLVertexDescriptor_retainCount(struct MTLVertexDescriptor *self);

struct MTLRenderPipelineDescriptor *MTLRenderPipelineDescriptor_alloc();
struct MTLRenderPipelineDescriptor *MTLRenderPipelineDescriptor_init(struct MTLRenderPipelineDescriptor *self);
void MTLRenderPipelineDescriptor_setLabel(struct MTLRenderPipelineDescriptor *self, struct NSString *label);
void MTLRenderPipelineDescriptor_setVertexFunction(struct MTLRenderPipelineDescriptor *self, struct MTLFunction *vertexFunction);
void MTLRenderPipelineDescriptor_setFragmentFunction(struct MTLRenderPipelineDescriptor *self, struct MTLFunction *fragmentFunction);
void MTLRenderPipelineDescriptor_setVertexDescriptor(struct MTLRenderPipelineDescriptor *self, struct MTLVertexDescriptor *vertexDescriptor);
void MTLRenderPipelineDescriptor_setSampleCount(struct MTLRenderPipelineDescriptor *self, NSUInteger sampleCount);
struct MTLRenderPipelineColorAttachmentDescriptorArray *MTLRenderPipelineDescriptor_colorAttachments(struct MTLRenderPipelineDescriptor *self);
struct MTLRenderPipelineColorAttachmentDescriptor *MTLRenderPipelineColorAttachmentDescriptorArray_objectAtIndexedSubscript(struct MTLRenderPipelineColorAttachmentDescriptorArray *self, NSUInteger attachmentIndex);
void MTLRenderPipelineColorAttachmentDescriptor_setPixelFormat(struct MTLRenderPipelineColorAttachmentDescriptor *self, MTLPixelFormat pixelFormat);
void MTLRenderPipelineDescriptor_setDepthAttachmentPixelFormat(struct MTLRenderPipelineDescriptor *self, MTLPixelFormat pixelFormat);
void MTLRenderPipelineDescriptor_setStencilAttachmentPixelFormat(struct MTLRenderPipelineDescriptor *self, MTLPixelFormat pixelFormat);
void MTLRenderPipelineDescriptor_release(struct MTLRenderPipelineDescriptor *self);
NSUInteger MTLRenderPipelineDescriptor_retainCount(struct MTLRenderPipelineDescriptor *self);
static inline void MTLRenderPipelineDescriptor_setLabel(struct MTLRenderPipelineDescriptor *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLRenderPipelineDescriptor_setLabel(self, string);
    NSString_release(string);
}
static inline struct MTLRenderPipelineColorAttachmentDescriptor *MTLRenderPipelineDescriptor_colorAttachmentAt(struct MTLRenderPipelineDescriptor *self, NSUInteger attachmentIndex)
{
    return MTLRenderPipelineColorAttachmentDescriptorArray_objectAtIndexedSubscript(MTLRenderPipelineDescriptor_colorAttachments(self), attachmentIndex);
}

struct MTLRenderPipelineState *MTLDevice_newRenderPipelineStateWithDescriptor(struct MTLDevice *self, struct MTLRenderPipelineDescriptor *descriptor, struct NSError **error);

typedef NSUInteger MTLCompareFunction;
enum
{
    MTLCompareFunctionNever = 0,
    MTLCompareFunctionLess = 1,
    MTLCompareFunctionEqual = 2,
    MTLCompareFunctionLessEqual = 3,
    MTLCompareFunctionGreater = 4,
    MTLCompareFunctionNotEqual = 5,
    MTLCompareFunctionGreaterEqual = 6,
    MTLCompareFunctionAlways = 7
};

struct MTLDepthStencilDescriptor *MTLDepthStencilDescriptor_alloc();
struct MTLDepthStencilDescriptor *MTLDepthStencilDescriptor_init(struct MTLDepthStencilDescriptor *self);
void MTLDepthStencilDescriptor_setDepthCompareFunction(struct MTLDepthStencilDescriptor *self, MTLCompareFunction compareFunction);
void MTLDepthStencilDescriptor_setDepthWriteEnabled(struct MTLDepthStencilDescriptor *self, bool depthWriteEnabled);
void MTLDepthStencilDescriptor_release(struct MTLDepthStencilDescriptor *self);
NSUInteger MTLDepthStencilDescriptor_retainCount(struct MTLDepthStencilDescriptor *self);

struct MTLDepthStencilState *MTLDevice_newDepthStencilStateWithDescriptor(struct MTLDevice *self, struct MTLDepthStencilDescriptor *descriptor);

typedef NSUInteger MTLCPUCacheMode;
enum
{
    MTLCPUCacheModeDefaultCache __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 0,
    MTLCPUCacheModeWriteCombined __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 1,
};

typedef NSUInteger MTLStorageMode;
enum
{
    MTLStorageModeShared __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0,
    MTLStorageModeManaged __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = 1,
    MTLStorageModePrivate __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 2,
    MTLStorageModeMemoryless __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = 3,
};

typedef NSUInteger MTLHazardTrackingMode;
enum
{
    MTLHazardTrackingModeDefault __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.13))) = 0,
    MTLHazardTrackingModeUntracked __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.13))) = 1,
    MTLHazardTrackingModeTracked __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.15))) = 2,
};

enum
{
    MTLResourceCPUCacheModeShift = 0,
    MTLResourceCPUCacheModeMask = (0xfUL << MTLResourceCPUCacheModeShift),
    MTLResourceStorageModeShift = 4,
    MTLResourceStorageModeMask = (0xfUL << MTLResourceStorageModeShift),
    MTLResourceHazardTrackingModeShift = 8,
    MTLResourceHazardTrackingModeMask = (0x3UL << MTLResourceHazardTrackingModeShift)
};

typedef NSUInteger MTLResourceOptions;
enum
{
    MTLResourceCPUCacheModeDefaultCache __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift,
    MTLResourceCPUCacheModeWriteCombined __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = MTLCPUCacheModeWriteCombined << MTLResourceCPUCacheModeShift,

    MTLResourceStorageModeShared __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = MTLStorageModeShared << MTLResourceStorageModeShift,
    MTLResourceStorageModeManaged __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.11))) = MTLStorageModeManaged << MTLResourceStorageModeShift,
    MTLResourceStorageModePrivate __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = MTLStorageModePrivate << MTLResourceStorageModeShift,
    MTLResourceStorageModeMemoryless __attribute__((availability(ios, introduced = 10.0), availability(macos, unavailable))) = MTLStorageModeMemoryless << MTLResourceStorageModeShift,

    MTLResourceHazardTrackingModeDefault __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.13))) = MTLHazardTrackingModeDefault << MTLResourceHazardTrackingModeShift,
    MTLResourceHazardTrackingModeUntracked __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.13))) = MTLHazardTrackingModeUntracked << MTLResourceHazardTrackingModeShift,
    MTLResourceHazardTrackingModeTracked __attribute__((availability(ios, introduced = 10.0), availability(macos, introduced = 10.15))) = MTLHazardTrackingModeTracked << MTLResourceHazardTrackingModeShift,

    MTLResourceOptionCPUCacheModeDefault __attribute__((availability(ios, introduced = 8.0, deprecated = 9.0), availability(macos, deprecated = 10.11))) = MTLResourceCPUCacheModeDefaultCache,
    MTLResourceOptionCPUCacheModeWriteCombined __attribute__((availability(ios, introduced = 8.0, deprecated = 9.0), availability(macos, deprecated = 10.11))) = MTLResourceCPUCacheModeWriteCombined,
};

void MTLResource_setLabel(struct MTLResource *self, struct NSString *label);

struct MTLBuffer *MTLDevice_newBufferWithLength(struct MTLDevice *self, NSUInteger length, MTLResourceOptions options);
void MTLBuffer_setLabel(struct MTLBuffer *self, struct NSString *label);
void *MTLBuffer_contents(struct MTLBuffer *self);
void MTLBuffer_release(struct MTLBuffer *self);
NSUInteger MTLBuffer_retainCount(struct MTLBuffer *self);
static inline void MTLBuffer_setLabel(struct MTLBuffer *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLBuffer_setLabel(self, string);
    NSString_release(string);
}

typedef NSUInteger MTLTextureType;
enum
{
    MTLTextureType1D __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 0,
    MTLTextureType1DArray __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 1,
    MTLTextureType2D __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 2,
    MTLTextureType2DArray __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 3,
    MTLTextureType2DMultisample __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 4,
    MTLTextureTypeCube __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 5,
    MTLTextureTypeCubeArray __attribute__((availability(ios, introduced = 11.0), availability(macos, introduced = 10.11))) = 6,
    MTLTextureType3D __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 7,
    MTLTextureType2DMultisampleArray __attribute__((availability(ios, unavailable), availability(macos, introduced = 10.14))) = 8,
    MTLTextureTypeTextureBuffer __attribute__((availability(ios, introduced = 12.0), availability(macos, introduced = 10.14))) = 9,
};

typedef NSUInteger MTLTextureUsage;
enum
{
    MTLTextureUsageUnknown __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0x0000,
    MTLTextureUsageShaderRead __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0x0001,
    MTLTextureUsageShaderWrite __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0x0002,
    MTLTextureUsageRenderTarget __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0x0004,
    MTLTextureUsagePixelFormatView __attribute__((availability(ios, introduced = 9.0), availability(macos, introduced = 10.11))) = 0x0010,
};

struct MTLTextureDescriptor *MTLTextureDescriptor_alloc();
struct MTLTextureDescriptor *MTLTextureDescriptor_init(struct MTLTextureDescriptor *self);
void MTLTextureDescriptor_setTextureType(struct MTLTextureDescriptor *self, MTLTextureType textureType);
void MTLTextureDescriptor_setPixelFormat(struct MTLTextureDescriptor *self, MTLPixelFormat pixelFormat);
void MTLTextureDescriptor_setWidth(struct MTLTextureDescriptor *self, NSUInteger width);
void MTLTextureDescriptor_setHeight(struct MTLTextureDescriptor *self, NSUInteger height);
void MTLTextureDescriptor_setDepth(struct MTLTextureDescriptor *self, NSUInteger depth);
void MTLTextureDescriptor_setMipmapLevelCount(struct MTLTextureDescriptor *self, NSUInteger mipmapLevelCount);
void MTLTextureDescriptor_setSampleCount(struct MTLTextureDescriptor *self, NSUInteger sampleCount);
void MTLTextureDescriptor_setArrayLength(struct MTLTextureDescriptor *self, NSUInteger arrayLength);
void MTLTextureDescriptor_setResourceOptions(struct MTLTextureDescriptor *self, MTLResourceOptions resourceOptions);
void MTLTextureDescriptor_setUsage(struct MTLTextureDescriptor *self, MTLTextureUsage usage);
void MTLTextureDescriptor_release(struct MTLTextureDescriptor *self);
NSUInteger MTLTextureDescriptor_retainCount(struct MTLTextureDescriptor *self);

struct MTLTexture *MTLDevice_newTextureWithDescriptor(struct MTLDevice *self, struct MTLTextureDescriptor *descriptor);
void MTLTexture_setLabel(struct MTLTexture *self, struct NSString *label);
void MTLTexture_release(struct MTLTexture *self);
NSUInteger MTLTexture_retainCount(struct MTLTexture *self);
static inline void MTLTexture_setLabel(struct MTLTexture *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLTexture_setLabel(self, string);
    NSString_release(string);
}

struct MTLCommandQueue *MTLDevice_newCommandQueue(struct MTLDevice *self);

struct MTLCommandBuffer *MTLCommandQueue_commandBuffer(struct MTLCommandQueue *self);
void MTLCommandBuffer_setLabel(struct MTLCommandBuffer *self, struct NSString *label);
void MTLCommandBuffer_addCompletedHandler(struct MTLCommandBuffer *self, void *pUserData, NSUInteger throttlingIndex, void (*pfnCallback)(void *, NSUInteger, struct MTLCommandBuffer *));
void MTLCommandBuffer_presentDrawable(struct MTLCommandBuffer *self, struct MTLDrawable *drawable);
void MTLCommandBuffer_commit(struct MTLCommandBuffer *self);
NSUInteger MTLCommandBuffer_retainCount(struct MTLCommandBuffer *self);
static inline void MTLCommandBuffer_setLabel(struct MTLCommandBuffer *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLCommandBuffer_setLabel(self, string);
    NSString_release(string);
}

void MTLCommandEncoder_setLabel(struct MTLCommandEncoder *self, struct NSString *label);
void MTLCommandEncoder_pushDebugGroup(struct MTLCommandEncoder *self, struct NSString *string);
void MTLCommandEncoder_popDebugGroup(struct MTLCommandEncoder *self);
void MTLCommandEncoder_endEncoding(struct MTLCommandEncoder *self);

typedef NSUInteger MTLPrimitiveType;
enum
{
    MTLPrimitiveTypePoint __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 0,
    MTLPrimitiveTypeLine __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 1,
    MTLPrimitiveTypeLineStrip __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 2,
    MTLPrimitiveTypeTriangle __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 3,
    MTLPrimitiveTypeTriangleStrip __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 4
};

typedef NSUInteger MTLCullMode;
enum
{
    MTLCullModeNone __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 0,
    MTLCullModeFront __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 1,
    MTLCullModeBack __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 2
};

typedef NSUInteger MTLWinding;
enum
{
    MTLWindingClockwise __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 0,
    MTLWindingCounterClockwise __attribute__((availability(ios, introduced = 8.0), availability(macos, introduced = 10.11))) = 1
};

struct MTLRenderCommandEncoder *MTLCommandBuffer_renderCommandEncoderWithDescriptor(struct MTLCommandBuffer *self, struct MTLRenderPassDescriptor *renderPassDescriptor);
void MTLRenderCommandEncoder_setLabel(struct MTLRenderCommandEncoder *self, struct NSString *label);
void MTLRenderCommandEncoder_pushDebugGroup(struct MTLRenderCommandEncoder *self, struct NSString *string);
void MTLRenderCommandEncoder_popDebugGroup(struct MTLRenderCommandEncoder *self);
void MTLRenderCommandEncoder_setCullMode(struct MTLRenderCommandEncoder *self, MTLCullMode cullMode);
void MTLRenderCommandEncoder_setFrontFacingWinding(struct MTLRenderCommandEncoder *self, MTLWinding frontFacingWinding);
void MTLRenderCommandEncoder_setRenderPipelineState(struct MTLRenderCommandEncoder *self, struct MTLRenderPipelineState *pipelineState);
void MTLRenderCommandEncoder_setDepthStencilState(struct MTLRenderCommandEncoder *self, struct MTLDepthStencilState *depthStencilState);
void MTLRenderCommandEncoder_setVertexBuffer(struct MTLRenderCommandEncoder *self, struct MTLBuffer *buffer, NSUInteger offset, NSUInteger index);
void MTLRenderCommandEncoder_setFragmentBuffer(struct MTLRenderCommandEncoder *self, struct MTLBuffer *buffer, NSUInteger offset, NSUInteger index);
void MTLRenderCommandEncoder_setFragmentTexture(struct MTLRenderCommandEncoder *self, struct MTLTexture *texture, NSUInteger index);
void MTLRenderCommandEncoder_drawPrimitives(struct MTLRenderCommandEncoder *self, MTLPrimitiveType primitiveType, NSUInteger vertexStart, NSUInteger vertexCount, NSUInteger instanceCount, NSUInteger baseInstance);
void MTLRenderCommandEncoder_endEncoding(struct MTLRenderCommandEncoder *self);
static inline void MTLRenderCommandEncoder_setLabel(struct MTLRenderCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLRenderCommandEncoder_setLabel(self, string);
    NSString_release(string);
}
static inline void MTLRenderCommandEncoder_pushDebugGroup(struct MTLRenderCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLRenderCommandEncoder_pushDebugGroup(self, string);
    NSString_release(string);
}

struct MTLParallelRenderCommandEncoder *MTLCommandBuffer_parallelRenderCommandEncoderWithDescriptor(struct MTLCommandBuffer *self, struct MTLRenderPassDescriptor *renderPassDescriptor);
void MTLParallelRenderCommandEncoder_setLabel(struct MTLParallelRenderCommandEncoder *self, struct NSString *label);
void MTLParallelRenderCommandEncoder_pushDebugGroup(struct MTLParallelRenderCommandEncoder *self, struct NSString *string);
void MTLParallelRenderCommandEncoder_popDebugGroup(struct MTLParallelRenderCommandEncoder *self);
struct MTLRenderCommandEncoder *MTLParallelRenderCommandEncoder_renderCommandEncoder(struct MTLParallelRenderCommandEncoder *self);
void MTLParallelRenderCommandEncoder_endEncoding(struct MTLParallelRenderCommandEncoder *self);
static inline void MTLParallelRenderCommandEncoder_setLabel(struct MTLParallelRenderCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLParallelRenderCommandEncoder_setLabel(self, string);
    NSString_release(string);
}
static inline void MTLParallelRenderCommandEncoder_pushDebugGroup(struct MTLParallelRenderCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLParallelRenderCommandEncoder_pushDebugGroup(self, string);
    NSString_release(string);
}

typedef struct
{
    NSUInteger x;
    NSUInteger y;
    NSUInteger z;
} MTLOrigin;

typedef struct
{
    NSUInteger width;
    NSUInteger height;
    NSUInteger depth;
} MTLSize;

struct MTLBlitCommandEncoder *MTLCommandBuffer_blitCommandEncoder(struct MTLCommandBuffer *self);
void MTLBlitCommandEncoder_setLabel(struct MTLBlitCommandEncoder *self, struct NSString *label);
void MTLBlitCommandEncoder_pushDebugGroup(struct MTLBlitCommandEncoder *self, struct NSString *string);
void MTLBlitCommandEncoder_popDebugGroup(struct MTLBlitCommandEncoder *self);
void MTLBlitCommandEncoder_copyFromBuffer(struct MTLBlitCommandEncoder *self, struct MTLBuffer *sourceBuffer, NSUInteger sourceOffset, NSUInteger sourceBytesPerRow, NSUInteger sourceBytesPerImage, MTLSize sourceSize, struct MTLTexture *destinationTexture, NSUInteger destinationSlice, NSUInteger destinationLevel, MTLOrigin destinationOrigin);
void MTLBlitCommandEncoder_endEncoding(struct MTLBlitCommandEncoder *self);
static inline void MTLBlitCommandEncoder_setLabel(struct MTLBlitCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLBlitCommandEncoder_setLabel(self, string);
    NSString_release(string);
}
static inline void MTLBlitCommandEncoder_pushDebugGroup(struct MTLBlitCommandEncoder *self, char const *label)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), label);
    MTLBlitCommandEncoder_pushDebugGroup(self, string);
    NSString_release(string);
}

struct MTLLibrary *MTLDevice_newLibraryWithData(struct MTLDevice *self, dispatch_data_t data, struct NSError **error);
void MTLLibrary_release(struct MTLLibrary *self);

struct MTLFunction *MTLLibrary_newFunctionWithName(struct MTLLibrary *self, struct NSString *functionName);
void MTLFunction_release(struct MTLFunction *self);
NSUInteger MTLFunction_retainCount(struct MTLFunction *self);
static inline struct MTLFunction *MTLLibrary_newFunctionWithName(struct MTLLibrary *self, char const *functionName)
{
    struct NSString *string = NSString_initWithUTF8String(NSString_alloc(), functionName);
    struct MTLFunction *function = MTLLibrary_newFunctionWithName(self, string);
    NSString_release(string);
    return function;
}

#endif
