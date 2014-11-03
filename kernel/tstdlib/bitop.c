/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

/*
 *	bitop.c (tstdlib)
 *	T-Kernel common standard library
 */

#include <basic.h>
#include <tk/tkernel.h>

/*** macros ***/
/* bit operation macro */
#if BIGENDIAN
#define _BIT_SET_N(n) ( (UB)0x80 >> ((UW)(n) & 0x07U) )
#define _BIT_SHIFT(n) ( (UB)n >> 1 )
#define _BIT_MSK_N(n) ( (UB)0xFF >> ((UW)(n) & 0x07U) )
#else
#define _BIT_SET_N(n) ( (UB)0x01 << ((UW)(n) & 0x07U) )
#define _BIT_SHIFT(n) ( (UB)(n) << 1 )
#define _BIT_MSK_N(n) ( (UB)0xFF << ((UW)(n) & 0x07U) )
#endif

/*** bit operation ***/
/* tstdlib_bitclr : clear specified bit */
void
tstdlib_bitclr( VP base, W offset )
{
	register UB *cp, mask;
	
	if (offset < 0) {
		return;
	}

	cp = (UB*)base;
	cp += offset / 8;

	mask = _BIT_SET_N(offset);

	*cp &= ~mask;
}

/* tstdlib_bitset : set specified bit */
void
tstdlib_bitset( VP base, W offset )
{
	register UB *cp, mask;
	
	if (offset < 0) {
		return;
	}

	cp = (UB*)base;
	cp += offset / 8;

	mask = _BIT_SET_N(offset);

	*cp |= mask;
}

/* tstdlib_bittest : check specified bit */
BOOL
tstdlib_bittest( VP base, W offset )
{
	register UB *cp, mask;
	
	if (offset < 0) {
		return FALSE;
	}

	cp = (UB*)base;
	cp += offset / 8;

	mask = _BIT_SET_N(offset);

	return (BOOL)(*cp & mask);
}

/* tstdlib_bitsearch0 : perform 0 search on bit string */
W
tstdlib_bitsearch0( VP base, W offset, W width )
{
	register UB *cp, mask, cpmask;
	register W position;

	if ((offset < 0) || (width < 0)) {
		return -1;
	}

	cp = (UB*)base;
	cp += offset / 8;

	position = 0;
	mask = _BIT_SET_N(offset);
	cpmask = (UB)(~_BIT_MSK_N(offset));	/* mask pattern ex) offset == 2, 00000011 */

	while (position < width) {
		if ((*cp | cpmask) != 0xFF) {	/* includes 0 --> search bit of 0 */
			while (1) {
				if (!(*cp & mask)) {
					if (position < width) {
						return position;
					} else {
						return -1;
					}
				}
				mask = _BIT_SHIFT(mask);
				++position;
			}
		} else {		/* all bits are 1 --> 1 Byte skip */
			if (position) {
				position += 8;
			} else {
				position = 8 - (W)((UW)offset & 7);
				mask = _BIT_SET_N(0);
				cpmask = (UB)(~_BIT_MSK_N(0));
			}
			cp++;
		}
	}

	return -1;
}

/* tstdlib_bitsearch1 : perform 1 search on bit string */
W
tstdlib_bitsearch1( VP base, W offset, W width )
{
	register UB *cp, mask, cpmask;
	register W position;

	if ((offset < 0) || (width < 0)) {
		return -1;
	}

	cp = (UB*)base;
	cp += offset / 8;

	position = 0;
	mask = _BIT_SET_N(offset);
	cpmask = _BIT_MSK_N(offset);	/* mask pattern ex) offset == 2, 11111100 */

	while (position < width) {
		if (*cp & cpmask) {		/* includes 1 --> search bit of 1 */
			while (1) {
				if (*cp & mask) {
					if (position < width) {
						return position;
					} else {
						return -1;
					}
				}
				mask = _BIT_SHIFT(mask);
				++position;
			}
		} else {		/* all bits are 0 --> 1 Byte skip */
			if (position) {
				position += 8;
			} else {
				position = 8 - (W)((UW)offset & 7);
				mask = _BIT_SET_N(0);
				cpmask = _BIT_MSK_N(0);
			}
			cp++;
		}
	}

	return -1;
}
