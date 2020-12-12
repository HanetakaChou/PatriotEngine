#if defined(PTWIN32)
#if defined(PTARM) || defined(PTARM64) 
//ARM同时支持大端和小端
#elif defined(PTX86) || defined(PTX64)
inline int16_t PTG_Assert_LEToH(int16_t Value)
{
	return Value;
}
inline uint16_t PTG_Assert_LEToH(uint16_t Value)
{
	return Value;
}
inline char16_t PTG_Assert_LEToH(char16_t Value)
{
	return Value;
}
inline int32_t PTG_Assert_LEToH(int32_t Value)
{
	return Value;
}
inline uint32_t PTG_Assert_LEToH(uint32_t Value)
{
	return Value;
}
inline float PTG_Assert_LEToH(float Value)
{
	return Value;
}
#else
#error 未知的架构
#endif
#elif defined(PTPOSIX)
#if defined(PTARM) || defined(PTARM64) 
//ARM同时支持大端和小端
#include <endian.h>
inline int16_t PTG_Assert_LEToH(int16_t ValueLE)
{
	uint16_t ValueH = le16toh(*reinterpret_cast<uint16_t*>(&ValueLE));
	return *reinterpret_cast<int16_t*>(&ValueH);
}
inline uint16_t PTG_Assert_LEToH(uint16_t ValueLE)
{
	return le16toh(ValueLE);
}
inline char16_t PTG_Assert_LEToH(char16_t ValueLE)
{
	return le16toh(static_cast<uint16_t>(ValueLE));
}
inline uint32_t PTG_Assert_LEToH(uint32_t ValueLE)
{
	return le32toh(ValueLE);
}
inline int32_t PTG_Assert_LEToH(int32_t ValueLE)
{
	uint32_t ValueH = le32toh(*reinterpret_cast<uint32_t*>(&ValueLE));
	return *reinterpret_cast<int32_t*>(&ValueH);
}
inline float PTG_Assert_LEToH(float ValueLE)
{
	uint32_t ValueH = le32toh(*reinterpret_cast<uint32_t*>(&ValueLE));
	return *reinterpret_cast<float*>(&ValueH);
}
#elif defined(PTX86) || defined(PTX64)
inline int16_t PTG_Assert_LEToH(int16_t Value)
{
	return Value;
}
inline uint16_t PTG_Assert_LEToH(uint16_t Value)
{
	return Value;
}
inline char16_t PTG_Assert_LEToH(char16_t Value)
{
	return Value;
}
inline int32_t PTG_Assert_LEToH(int32_t Value)
{
	return Value;
}
inline uint32_t PTG_Assert_LEToH(uint32_t Value)
{
	return Value;
}
inline float PTG_Assert_LEToH(float Value)
{
	return Value;
}
#else
#error 未知的架构
#endif
#else
#error 未知的平台
#endif
