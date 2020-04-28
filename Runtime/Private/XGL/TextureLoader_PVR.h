#ifndef _TEXTURELOADER_PVR_H_
#define _TEXTURELOADER_PVR_H_ 1

bool PVRTextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                        struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset);

bool PVRTextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                      uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                      struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate);

#endif