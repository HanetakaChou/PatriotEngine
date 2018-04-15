#ifndef PT_SYSTEM_CONVUTF_H
#define PT_SYSTEM_CONVUTF_H

#include <stdint.h>

inline void PTSConv_UTF8ToUTF16(const char *pInBuf, size_t * const pInCharsLeft, wchar_t *pOutbuf, size_t * const pOutCharsLeft)
{
	while ((*pInCharsLeft) >= 1)
	{
		uint32_t ucs4code = 0;//Accumulator

		//UTF-8 To UCS-4
		if ((*pInBuf) < 128U)//0XXX XXXX
		{
			ucs4code += (*pInBuf);//Accumulate

			++pInBuf;
			--(*pInCharsLeft);
		}
		else if ((*pInBuf) < 192U) //10XX XXXX
		{
			return;//false
		}
		else if ((*pInBuf) < 224U)//110X XXXX 10XX XXXX
		{

			if ((*pInCharsLeft) >= 2)
			{
				ucs4code += (((*pInBuf) - 192U) << 6U);//Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}

			}
			else
			{
				return;//false
			}

		}
		else if ((*pInBuf) < 240U)//1110 XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pInCharsLeft) >= 3)
			{
				ucs4code += (((*pInBuf) - 224U) << 12U);//Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 6U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}
			}
			else
			{
				return;//false
			}
		}
		else // ((*pInBuf) >= 240U) 1111 0XXX 10XX XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pInCharsLeft) >= 4)
			{
				ucs4code += (((*pInBuf) - 240U) << 18U);//Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 12U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 6U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U)//10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}
			}
			else
			{
				return;//false
			}
		}


		//UCS-4 To UTF-16
		if (ucs4code < 0X10000U)
		{
			if ((*pOutCharsLeft) >= 1)
			{
				(*pOutbuf) = static_cast<uint16_t>(ucs4code);

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return;//false
			}
		}
		else//ucs4code >= 0X10000U
		{
			if ((*pOutCharsLeft) >= 2)
			{
				(*pOutbuf) = static_cast<uint16_t>((((ucs4code - 65536U)&(0XFFC00U)) >> 10U) + 0XD800U);//110110xxxxxxxxxx 

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint16_t>(((ucs4code - 65536U)&(0X3FFU)) + 0XDC00U);//110111xxxxxxxxxx

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return;//false
			}
		}
	}

	return;//true
}

inline void PTSConv_UTF16ToUTF8(const wchar_t *pInBuf, size_t * const pInCharsLeft, char *pOutbuf, size_t * const pOutCharsLeft)
{
	while ((*pInCharsLeft) >= 1)
	{
		uint32_t ucs4code = 0;//Accumulator

		//UTF-16 To UCS-4
		if ((*pInBuf) >= 0XD800U && (*pInBuf) <= 0XDBFFU)//110110xxxxxxxxxx
		{
			if ((*pInCharsLeft) >= 2)
			{
				ucs4code += (((*pInBuf) - 0XD800U) << 10U);//Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 0XDC00U && (*pInBuf) <= 0XDFFF)//110111xxxxxxxxxx
				{
					ucs4code += ((*pInBuf) - 0XDC00U);//Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return;//false
				}

				ucs4code += 0X10000U;

			}
			else
			{
				return;//false
			}
		}
		else
		{
			ucs4code += (*pInBuf);//Accumulate

			++pInBuf;
			--(*pInCharsLeft);
		}

		//UCS-4 To UTF-16
		if (ucs4code < 128U)//0XXX XXXX
		{
			if ((*pOutCharsLeft) >= 1)
			{
				(*pOutbuf) = static_cast<uint8_t>(ucs4code);

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return;//false
			}
		}
		else if (ucs4code < 2048U)//110X XXXX 10XX XXXX
		{
			if ((*pOutCharsLeft) >= 2)
			{
				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0X7C0U) >> 6U) + 192U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return;//false
			}
		}
		else if (ucs4code < 0X10000U)//1110 XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pOutCharsLeft) >= 3)
			{
				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0XF000U) >> 12U) + 224U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0XFC0U) >> 6U) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return;//false
			}
		}
		else if (ucs4code < 0X200000U)//1111 0XXX 10XX XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pOutCharsLeft) >= 4)
			{
				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0X1C0000U) >> 18U) + 240U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0X3F000U) >> 12U) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>(((ucs4code & 0XFC0U) >> 6U) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);

				(*pOutbuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

				++pOutbuf;
				--(*pOutCharsLeft);
			}
			else//ucs4code >= 0X200000U
			{
				return;//false
			}
		}
	}
}

#endif

