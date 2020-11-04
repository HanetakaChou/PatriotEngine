/***
*sect_attribs.h - section attributes for IA64 CRTs
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#pragma section(".CRTMP$XCA",long,read)
#pragma section(".CRTMP$XCZ",long,read)
#pragma section(".CRTMP$XIA",long,read)
#pragma section(".CRTMP$XIZ",long,read)

#pragma section(".CRTMA$XCA",long,read)
#pragma section(".CRTMA$XCZ",long,read)
#pragma section(".CRTMA$XIA",long,read)
#pragma section(".CRTMA$XIZ",long,read)

#pragma section(".CRTVT$XCA",long,read)
#pragma section(".CRTVT$XCZ",long,read)

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCAA",long,read)
#pragma section(".CRT$XCC",long,read)
#pragma section(".CRT$XCZ",long,read)
#pragma section(".CRT$XDA",long,read)
#pragma section(".CRT$XDC",long,read)
#pragma section(".CRT$XDZ",long,read)
#pragma section(".CRT$XIA",long,read)
#pragma section(".CRT$XIAA",long,read)
#pragma section(".CRT$XIC",long,read)
#pragma section(".CRT$XID",long,read)
#pragma section(".CRT$XIY",long,read)
#pragma section(".CRT$XIZ",long,read)
#pragma section(".CRT$XLA",long,read)
#pragma section(".CRT$XLC",long,read)
#pragma section(".CRT$XLD",long,read)
#pragma section(".CRT$XLZ",long,read)
#pragma section(".CRT$XPA",long,read)
#pragma section(".CRT$XPB",long,read)
#pragma section(".CRT$XPX",long,read)
#pragma section(".CRT$XPXA",long,read)
#pragma section(".CRT$XPZ",long,read)
#pragma section(".CRT$XTA",long,read)
#pragma section(".CRT$XTB",long,read)
#pragma section(".CRT$XTX",long,read)
#pragma section(".CRT$XTZ",long,read)

#pragma section(".rdata$T",long,read)
#pragma section(".rtc$IAA",long,read)
#pragma section(".rtc$IZZ",long,read)
#pragma section(".rtc$TAA",long,read)
#pragma section(".rtc$TZZ",long,read)

#define _CRTALLOC(x) __declspec(allocate(x))
