/*
* Copyright (c) 2005 Apple Computer, Inc. All rights reserved.
*
* @APPLE_LICENSE_HEADER_START@
*
* This file contains Original Code and/or Modifications of Original Code
* as defined in and that are subject to the Apple Public Source License
* Version 2.0 (the 'License'). You may not use this file except in
* compliance with the License. Please obtain a copy of the License at
* http://www.opensource.apple.com/apsl/ and read it before using this
* file.
*
* The Original Code and all software distributed under the License are
* distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
* EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* Please see the License for the specific language governing rights and
* limitations under the License.
*
* @APPLE_LICENSE_HEADER_END@
*/
/*	uuid.c
Copyright 1999-2002, Apple, Inc. All rights reserved.
Responsibility: Doug Davidson
*/

#include "uuid.h"

#include <assert.h>

typedef struct
{
	uint8_t eaddr[6]; /* 6 bytes of ethernet hardware address */
} uuid_address_t;

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <stdlib.h>

int GetEthernetAddr(uuid_address_t *addr)
{
	alignas(alignof(ifreq)) char ifrbuf[sizeof(ifreq) * 30];
	struct ifconf ifc;
	bool foundIt = false;

	int s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	assert(s != -1);

	ifc.ifc_buf = ifrbuf;
	ifc.ifc_len = sizeof(ifrbuf);
	int iResult = ::ioctl(s, SIOCGIFCONF, &ifc);
	assert(iResult == 0);

	for (int i = 0; i < (ifc.ifc_len / int(sizeof(ifreq))); ++i)
	{
		iResult = ioctl(s, SIOCGIFHWADDR, &ifc.ifc_req[i]);
		assert(iResult == 0);

		if (ifc.ifc_req[i].ifr_hwaddr.sa_family == ARPHRD_ETHER)
		{
			::memcpy(addr->eaddr, ifc.ifc_req[i].ifr_hwaddr.sa_data, sizeof(uuid_address_t));
			foundIt = 1;
			break;
		}
	}

	iResult = ::close(s);
	assert(iResult == 0);

	return (foundIt ? 0 : -1);
}

/*    uuid.c
*
*        Modifications made by William Woody to make this thing
*    work on the Macintosh.
*/

/*
*
* (c) Copyright 1989 OPEN SOFTWARE FOUNDATION, INC.
* (c) Copyright 1989 HEWLETT-PACKARD COMPANY
* (c) Copyright 1989 DIGITAL EQUIPMENT CORPORATION
* To anyone who acknowledges that this file is provided "AS IS"
* without any express or implied warranty:
*                 permission to use, copy, modify, and distribute this
* file for any purpose is hereby granted without fee, provided that
* the above copyright notices and this notice appears in all source
* code copies, and that none of the names of Open Software
* Foundation, Inc., Hewlett-Packard Company, or Digital Equipment
* Corporation be used in advertising or publicity pertaining to
* distribution of the software without specific, written prior
* permission.  Neither Open Software Foundation, Inc., Hewlett-
* Packard Company, nor Digital Equipment Corporation makes any
* representations about the suitability of this software for any
* purpose.
*
*/
/*
*/
/*
**
**  NAME:
**
**      uuid.c
**
**  FACILITY:
**
**      UUID
**
**  ABSTRACT:
**
**      UUID - routines that manipulate uuid's
**
**
*/


/*    uuid
*
*        Universal Unique ID. Note this definition will result is a 16-byte
*    structure regardless what platform it is on.
*/

typedef struct uuid_v1_t
{
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint8_t clock_seq_hi_and_reserved;
	uint8_t clock_seq_low;
	uint8_t node[6];
}uuid_v1_t;

typedef uint64_t uuid_time_t;

static int GenRandomEthernet(uuid_address_t *addr);

/*
* Internal structure of universal unique IDs (UUIDs).
*
* There are three "variants" of UUIDs that this code knows about.  The
* variant #0 is what was defined in the 1989 HP/Apollo Network Computing
* Architecture (NCA) specification and implemented in NCS 1.x and DECrpc
* v1.  Variant #1 is what was defined for the joint HP/DEC specification
* for the OSF (in DEC's "UID Architecture Functional Specification Version
* X1.0.4") and implemented in NCS 2.0, DECrpc v2, and OSF 1.0 DCE RPC.
* Variant #2 is defined by Microsoft.
*
* This code creates only variant #1 UUIDs.
*
* The three UUID variants can exist on the same wire because they have
* distinct values in the 3 MSB bits of octet 8 (see table below).  Do
* NOT confuse the version number with these 3 bits.  (Note the distinct
* use of the terms "version" and "variant".) Variant #0 had no version
* field in it.  Changes to variant #1 (should any ever need to be made)
* can be accomodated using the current form's 4 bit version field.
*
* The UUID record structure MUST NOT contain padding between fields.
* The total size = 128 bits.
*
* To minimize confusion about bit assignment within octets, the UUID
* record definition is defined only in terms of fields that are integral
* numbers of octets.
*
* Depending on the network data representation, the multi-octet unsigned
* integer fields are subject to byte swapping when communicated between
* dissimilar endian machines.  Note that all three UUID variants have
* the same record structure; this allows this byte swapping to occur.
* (The ways in which the contents of the fields are generated can and
* do vary.)
*
* The following information applies to variant #1 UUIDs:
*
* The lowest addressed octet contains the global/local bit and the
* unicast/multicast bit, and is the first octet of the address transmitted
* on an 802.3 LAN.
*
* The adjusted time stamp is split into three fields, and the clockSeq
* is split into two fields.
*
* |<------------------------- 32 bits -------------------------->|
*
* +--------------------------------------------------------------+
* |                     low 32 bits of time                      |  0-3  .time_low
* +-------------------------------+-------------------------------
* |     mid 16 bits of time       |  4-5               .time_mid
* +-------+-----------------------+
* | vers. |   hi 12 bits of time  |  6-7               .time_hi_and_version
* +-------+-------+---------------+
* |Res|  clkSeqHi |  8                                 .clock_seq_hi_and_reserved
* +---------------+
* |   clkSeqLow   |  9                                 .clock_seq_low
* +---------------+----------...-----+
* |            node ID               |  8-16           .node
* +--------------------------...-----+
*
* --------------------------------------------------------------------------
*
* The structure layout of all three UUID variants is fixed for all time.
* I.e., the layout consists of a 32 bit int, 2 16 bit ints, and 8 8
* bit ints.  The current form version field does NOT determine/affect
* the layout.  This enables us to do certain operations safely on the
* variants of UUIDs without regard to variant; this increases the utility
* of this code even as the version number changes (i.e., this code does
* NOT need to check the version field).
*
* The "Res" field in the octet #8 is the so-called "reserved" bit-field
* and determines whether or not the uuid is a old, current or other
* UUID as follows:
*
*      MS-bit  2MS-bit  3MS-bit      Variant
*      ---------------------------------------------
*         0       x        x       0 (NCS 1.5)
*         1       0        x       1 (DCE 1.0 RPC)
*         1       1        0       2 (Microsoft)
*         1       1        1       unspecified
*
* --------------------------------------------------------------------------
*
* Internal structure of variant #0 UUIDs
*
* The first 6 octets are the number of 4 usec units of time that have
* passed since 1/1/80 0000 GMT.  The next 2 octets are reserved for
* future use.  The next octet is an address family.  The next 7 octets
* are a host ID in the form allowed by the specified address family.
*
* Note that while the family field (octet 8) was originally conceived
* of as being able to hold values in the range [0..255], only [0..13]
* were ever used.  Thus, the 2 MSB of this field are always 0 and are
* used to distinguish old and current UUID forms.
*
* +--------------------------------------------------------------+
* |                    high 32 bits of time                      |  0-3  .time_high
* +-------------------------------+-------------------------------
* |     low 16 bits of time       |  4-5               .time_low
* +-------+-----------------------+
* |         reserved              |  6-7               .reserved
* +---------------+---------------+
* |    family     |   8                                .family
* +---------------+----------...-----+
* |            node ID               |  9-16           .node
* +--------------------------...-----+
*
*/

/***************************************************************************
*
* Local definitions
*
**************************************************************************/

static const long uuid_c_version = 1;

/*
* local defines used in uuid bit-diddling
*/

#define TIME_LOW_MASK               0X00000000FFFFFFFFU
#define TIME_MID_MASK               0X0000FFFF00000000U
#define TIME_HIGH_MASK              0X0FFF000000000000U
#define TIME_MID_SHIFT_COUNT		32U
#define TIME_HIGH_SHIFT_COUNT       48U

/*
*    The following was modified in order to prevent overlap because
*    our clock is (theoretically) accurate to 1us (or 1s in CarbonLib)
*/


#define MAX_TIME_ADJUST             9U            /* Max adjust before tick */

#define CLOCK_SEQ_LOW_MASK          0XFFU
#define CLOCK_SEQ_HIGH_MASK         0X3F00U
#define CLOCK_SEQ_HIGH_SHIFT_COUNT  8U
#define CLOCK_SEQ_FIRST             1U
#define CLOCK_SEQ_LAST              0X3FFFU

/*
* Note: If CLOCK_SEQ_BIT_BANG == true, then we can avoid the modulo
* operation.  This should save us a divide instruction and speed
* things up.
*/

#ifndef CLOCK_SEQ_BIT_BANG
#define CLOCK_SEQ_BIT_BANG          1
#endif

#if CLOCK_SEQ_BIT_BANG
#define CLOCK_SEQ_BUMP(seq)         ((*seq) = ((*seq) + 1U) & CLOCK_SEQ_LAST)
#else
#define CLOCK_SEQ_BUMP(seq)         ((*seq) = ((*seq) + 1U) % (CLOCK_SEQ_LAST + 1U))
#endif

#define UUID_VERSION_BITS           (uuid_c_version << 12)
#define UUID_RESERVED_BITS          0X80U

static uuid_time_t time_now = 0U; /* utc time as of last query*/
static uuid_time_t time_last;    /* utc time last time I looked      */
static uint16_t time_adjust;  /* 'adjustment' to ensure uniqness  */
static uint16_t clock_seq;    /* 'adjustment' for backwards clocks*/

typedef enum
{
	uuid_e_less_than,
	uuid_e_equal_to,
	uuid_e_greater_than
} uuid_compval_t;

/****************************************************************************
*
* local function declarations
*
****************************************************************************/

/*
* T R U E _ R A N D O M
*/
static uint16_t true_random(void);

/*
* N E W _ C L O C K _ S E Q
*
* Ensure clock_seq is up-to-date
*
* Note: clock_seq is architected to be 14-bits (unsigned) but
*       I've put it in here as 16-bits since there isn't a
*       14-bit unsigned integer type (yet)
*/
static void new_clock_seq(uint16_t * /*clock_seq*/);

/************************************************************************/
/*                                                                        */
/*    New Routines                                                        */
/*                                                                        */
/************************************************************************/

/*
* saved copy of our IEEE 802 address for quick reference
*/

static uuid_address_t saved_addr = { { 0, 0, 0, 0, 0, 0 } };
static bool got_address = false;
static int last_addr_result = -1;

static int GenRandomEthernet(uuid_address_t *addr)
{
	for (int i = 0; i < 6; ++i)
	{
		addr->eaddr[i] = static_cast<uint8_t>(true_random()) & uint8_t(0XFFU);
	}
	return 0;
}

/*
**++
**
**  ROUTINE NAME:       uuid_get_address
**
**  SCOPE:              PUBLIC
**
**  DESCRIPTION:
**
**  Return our IEEE 802 address.
**
**  This function is not really "public", but more like the SPI functions
**  -- available but not part of the official API.  We've done this so
**  that other subsystems (of which there are hopefully few or none)
**  that need the IEEE 802 address can use this function rather than
**  duplicating the gore it does (or more specifically, the gore that
**  "uuid__get_os_address" does).
**
**  INPUTS:             none
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      addr            IEEE 802 address
**
**      status          return status value
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

static int uuid_get_address(uuid_address_t *addr)
{

	/*
	* just return address we determined previously if we've
	* already got one
	*/
	if (got_address)
	{
		::memcpy(addr, &saved_addr, sizeof(uuid_address_t));
		return last_addr_result;
	}

	/*
	* Otherwise, call the system specific routine.
	*/

	last_addr_result = GetEthernetAddr(addr);

	/*
	*    Was this an error? If so, I need to generate a random
	*    sequence to use in place of an Ethernet address.
	*/
	if (last_addr_result != 0)
	{
		last_addr_result = GenRandomEthernet(addr);
	}

	if (last_addr_result == 0)
	{
		got_address = true;
		/* On no error copy */
		::memcpy(&saved_addr, addr, sizeof(uuid_address_t));
	}

	return last_addr_result;
}

/*****************************************************************************
*
*  Macro definitions
*
****************************************************************************/

/*
*  Define constant designation difference in Unix and DTSS base times:
*  DTSS UTC base time is October 15, 1582.
*  Unix base time is January 1, 1970.
*/
#define uuid_c_os_base_time_diff 0X01B21DD213814000   

/*
* U U I D _ _ G E T _ O S _ T I M E
*
* Get OS time - contains platform-specific code.
*/

static uuid_time_t uuid_get_os_time()
{
	struct timeval tv;
	int iResult = ::gettimeofday(&tv, NULL);
	assert(iResult == 0);
	return (1000000U * static_cast<uint64_t>(tv.tv_sec) + static_cast<uint64_t>(tv.tv_usec)) + uuid_c_os_base_time_diff;
}


/*
**++
**
**  ROUTINE NAME:       init
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**  Startup initialization routine for the UUID module.
**
**  INPUTS:             none
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      status          return status value
**
**          uuid_s_ok
**          uuid_s_coding_error
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     void
**
**  SIDE EFFECTS:       none
**
**--
**/

static struct __uuid_init
{
	inline __uuid_init()
	{
		time_last = uuid_get_os_time();
		clock_seq = true_random();
	}
}__uuid_init_done;


static int _CFGenerateV1UUID(uuid_v1_t *uuid)
{
	int err;

	//get our hardware network address
	uuid_address_t eaddr;
	err = uuid_get_address(&eaddr);
	if (err != 0)
		return err;

	int got_no_time = false;
	do
	{
		//get the current time
		time_now = ::uuid_get_os_time();

		/*
		* do stuff like:
		*
		*  o check that our clock hasn't gone backwards and handle it
		*    accordingly with clock_seq
		*  o check that we're not generating uuid's faster than we
		*    can accommodate with our time_adjust fudge factor
		*/
		if (time_now < time_last)
		{
			new_clock_seq(&clock_seq);
			time_adjust = 0;
		}
		else if (time_now > time_last)
		{
			time_adjust = 0;
		}
		else
		{
			if (time_adjust == MAX_TIME_ADJUST)
			{
				//spin your wheels while we wait for the clock to tick
				got_no_time = true;
			}
		}
	} while (got_no_time);

	time_last = time_now;

	if (time_adjust != 0)
	{
		time_now += time_adjust;
	}

	/*
	* now construct a uuid with the information we've gathered
	* plus a few constants
	*/
	uuid->time_low = static_cast<uint32_t>(time_now);
	uuid->time_mid = static_cast<uint32_t>((time_now&TIME_MID_MASK) >> TIME_MID_SHIFT_COUNT);

	uuid->time_hi_and_version = static_cast<uint32_t>((time_now & TIME_HIGH_MASK) >> TIME_HIGH_SHIFT_COUNT);
	uuid->time_hi_and_version |= UUID_VERSION_BITS;

	uuid->clock_seq_low = clock_seq & CLOCK_SEQ_LOW_MASK;
	uuid->clock_seq_hi_and_reserved = (clock_seq & CLOCK_SEQ_HIGH_MASK) >> CLOCK_SEQ_HIGH_SHIFT_COUNT;

	uuid->clock_seq_hi_and_reserved |= UUID_RESERVED_BITS;

	::memcpy(uuid->node, &eaddr, sizeof(uuid_address_t));

	return 0;
}

int uuid_generate_time_safe(uuid_t out)
{
	return _CFGenerateV1UUID(reinterpret_cast<uuid_v1_t*>(out));
}

/****************************************************************************
**
**    U U I D   T R U E   R A N D O M   N U M B E R   G E N E R A T O R
**
*****************************************************************************/
static inline uint16_t true_random(void)
{
	uint16_t Buf;
	::arc4random_buf(&Buf, sizeof(uint16_t));
	return Buf;
}


/*****************************************************************************
*
*  LOCAL PROCEDURES - procedures used staticly by the UUID module
*
****************************************************************************/

/*
** N E W _ C L O C K _ S E Q
**
** Ensure *clkseq is up-to-date
**
** Note: clock_seq is architected to be 14-bits (unsigned) but
**       I've put it in here as 16-bits since there isn't a
**       14-bit unsigned integer type (yet)
**/

static void new_clock_seq(uint16_t *clkseq)
{
	CLOCK_SEQ_BUMP(clkseq);
	if (*clkseq == 0U)
	{
		*clkseq = 1U;
	}
}