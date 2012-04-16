#include "get_num.h"

/*-
 * Copyright (c) 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Keith Muller of the University of California, San Diego and Lance
 * Visser of Convex Computer Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Convert an expression of the following forms to a uintmax_t.
 * 	1) A positive decimal number.
 *	2) A positive decimal number followed by a 'b' or 'B' (mult by 512).
 *	3) A positive decimal number followed by a 'k' or 'K' (mult by 1 << 10).
 *	4) A positive decimal number followed by a 'm' or 'M' (mult by 1 << 20).
 *	5) A positive decimal number followed by a 'g' or 'G' (mult by 1 << 30).
 *	5) A positive decimal number followed by a 'w' or 'W' (mult by sizeof int).
 *	6) Two or more positive decimal numbers (with/without [BbKkMmGgWw])
 *	   separated by 'x' or 'X' (also '*' for backwards compatibility),
 *	   specifying the product of the indicated values.
 */

/* static uintmax_t */
uintmax_t get_num(const char *val)
{
	uintmax_t num, mult, prevnum;
	char *expr;

	errno = 0;
	num = strtouq(val, &expr, 0);
	if (errno != 0)				/* Overflow or underflow. */
		err(1, "overflow or underflow");
		//err(1, "%s", oper);
	
	if (expr == val)			/* No valid digits. */
		errx(1, "illegal numeric value");
		//errx(1, "%s: illegal numeric value", oper);

	mult = 0;
	switch (*expr) {
	case 'B':
	case 'b':
		mult = 512;
		break;
	case 'K':
	case 'k':
		mult = 1 << 10;
		break;
	case 'M':
	case 'm':
		mult = 1 << 20;
		break;
	case 'G':
	case 'g':
		mult = 1 << 30;
		break;
	case 'W':
	case 'w':
		mult = sizeof(int);
		break;
	default:
		;
	}

	if (mult != 0) {
		prevnum = num;
		num *= mult;
		/* Check for overflow. */
		if (num / mult != prevnum)
			goto erange;
		expr++;
	}

	switch (*expr) {
		case '\0':
			break;
		case '*':			/* Backward compatible. */
		case 'X':
		case 'x':
			mult = get_num(expr + 1);
			prevnum = num;
			num *= mult;
			if (num / mult == prevnum)
				break;
erange:
			errx(1, "%s", strerror(ERANGE));
			//errx(1, "%s: %s", oper, strerror(ERANGE));
		default:
			errx(1, "illegal numeric value");
			//errx(1, "%s: illegal numeric value", oper);
	}
	return (num);
}
