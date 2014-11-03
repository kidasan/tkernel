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
 *	@(#)typedef.h
 *
 *	Standard data type definitions
 */

#ifndef	__TYPEDEF_H__
#define __TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * General-purpose data type  
 */
typedef char		B;	/* Signed 8 bit integer */
typedef short		H;	/* Signed 16 bit integer */
typedef int		W;	/* Signed 32 bit integer */
typedef unsigned char	UB;	/* Unsigned 8 bit integer */
typedef unsigned short  UH;	/* Unsigned 16 bit integer */
typedef unsigned int	UW;	/* Unsigned 32 bit integer */

typedef char		VB;	/* Nonuniform type 8 bit data */
typedef short		VH;	/* Nonuniform type 16 bit data */
typedef int		VW;	/* Nonuniform type 32 bit data */
typedef void		*VP;	/* Nonuniform type data pointer */

typedef volatile B	_B;	/* Volatile statement attached */
typedef volatile H	_H;
typedef volatile W	_W;
typedef volatile UB	_UB;
typedef volatile UH	_UH;
typedef volatile UW	_UW;

typedef int		INT;	/* Processor bit width signed integer */
typedef unsigned int	UINT;	/* Processor bit width unsigned integer */

typedef INT		ID;	/* ID general */
typedef	INT		MSEC;	/* Time general (millisecond) */

typedef void		(*FP)();	/* Function address general */
typedef INT		(*FUNCP)();	/* Function address general */

#define LOCAL		static		/* Local symbol definition */
#define EXPORT				/* Global symbol definition */
#define IMPORT		extern		/* Global symbol reference */

/*
 * Boolean value 
 *	Defined as TRUE = 1, but it is always true when not 0.
 *	Thus, comparison such as bool = TRUE are not permitted.
 *	Should be as per bool !=FALSE.
 */
typedef INT		BOOL;
#define TRUE		1	/* True */
#define FALSE		0	/* False */

/*
 * TRON character code
 */
typedef UH		TC;		/* TRON character code */
#define TNULL		((TC)0)		/* End of TRON code character string */

#ifdef __cplusplus
}
#endif
#endif /* __TYPEDEF_H__ */
