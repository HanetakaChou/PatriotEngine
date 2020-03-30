bool LoadTextureDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                               struct DDS_HEADER const **header,
                               uint8_t const **bitData, size_t *bitSize);