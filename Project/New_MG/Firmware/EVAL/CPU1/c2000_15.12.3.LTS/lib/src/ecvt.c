/****************************************************************************/
/*  ecvt v15.12.3                                                           */
/*                                                                          */
/* Copyright (c) 1993-2016 Texas Instruments Incorporated                   */
/* http://www.ti.com/                                                       */
/*                                                                          */
/*  Redistribution and  use in source  and binary forms, with  or without   */
/*  modification,  are permitted provided  that the  following conditions   */
/*  are met:                                                                */
/*                                                                          */
/*     Redistributions  of source  code must  retain the  above copyright   */
/*     notice, this list of conditions and the following disclaimer.        */
/*                                                                          */
/*     Redistributions in binary form  must reproduce the above copyright   */
/*     notice, this  list of conditions  and the following  disclaimer in   */
/*     the  documentation  and/or   other  materials  provided  with  the   */
/*     distribution.                                                        */
/*                                                                          */
/*     Neither the  name of Texas Instruments Incorporated  nor the names   */
/*     of its  contributors may  be used to  endorse or  promote products   */
/*     derived  from   this  software  without   specific  prior  written   */
/*     permission.                                                          */
/*                                                                          */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS   */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT   */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT   */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT   */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT   */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/*                                                                          */
/****************************************************************************/
#include <stdlib.h>
#include <math.h>

#define MANTISSA_WORD 0
#define EXPONENT_WORD 1

/****************************************************************************/
/* THIS FUNCTION IS NON-STANDARD AND IS PROVIDED ONLY AS A BUILDING BLOCK   */
/* FOR PRINTF.                                                              */
/****************************************************************************/
char *ecvt(long double value, register int ndigit, int *decpt, int *sign)
{
    static char out[100]; 

    int   digits = 0;                  /* NUMBER OF DIGITS BEFORE .       */
    char *pos    = out + 1;     
    int   temp;

    out[0] = '0';                      /* JUST IN CASE WE ROUND.          */
    ndigit++;                          /* DO ONE EXTRA DIGIT FOR ROUNDING */

    /*--------------------------------------------------------------------*/
    /* IF TRYING TO CONVERT INFINITY, RETURN HUGE_VALL OF PROPER SIGN     */
    /*--------------------------------------------------------------------*/
    {
       long *ptr = (long *)&value;

       if (((ptr[EXPONENT_WORD] >> 20) & 0x7ff) == 0x7ff)
          value = (ptr[EXPONENT_WORD] & 0x80000000) ? -HUGE_VALL : HUGE_VALL;
    }

    /*--------------------------------------------------------------------*/
    /* PERFORM PRESCALING - MAKE SURE NUMBER HAS INTEGRAL WHOLE PART      */
    /*--------------------------------------------------------------------*/
    if (*sign = (value < 0)) value = -value;
    while (value > 0x7FFFFFFF) { value /= 10; digits++; }
    while (value && value < 1) { value *= 10; digits--; }

    /*--------------------------------------------------------------------*/
    /* WRITE OUT INTEGRAL PART OF NUMBER.                                 */
    /*--------------------------------------------------------------------*/
    pos    += temp = ltoa((long)value, pos);
    *decpt  = digits + temp;

    /*--------------------------------------------------------------------*/
    /* WRITE OUT FRACTIONAL PART OF NUMBER                                */
    /*--------------------------------------------------------------------*/
    if (temp >= ndigit) 
	pos = out + ndigit + 1;
    else if ((ndigit -= temp) > 0) do
    {
        value -= (long)value;
        *pos++ = (int)(value *= 10.0) + '0';
    } 
    while (--ndigit);

    /*--------------------------------------------------------------------*/
    /* PERFORM ROUNDING.  NOTE THAT pos IS CURRENTLY POINTING AT AN EXTRA */
    /* DIGIT WHICH WAS CONVERTED FOR THIS PURPOSE.                        */
    /*--------------------------------------------------------------------*/
    if (*--pos >= '5')
    {
	char *ptr = pos;
	while ((*--ptr += 1) > '9') *ptr = '0';
	if (ptr == out) { *--pos = 0; *decpt += 1; return out; }
    }
	
    *pos = 0;
    return out + 1;
}

