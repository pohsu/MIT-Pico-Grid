/****************************************************************************/
/*  memcpy v15.12.3                                                         */
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
#include <string.h>

void *memcpy(void *to, const void *from, size_t n)
{
     register char *rto   = (char *) to;
     register char *rfrom = (char *) from;
     register unsigned int rn;
     register unsigned int nn = (unsigned int) n;

     /***********************************************************************/
     /*  Copy up to the first 64K. At the end compare the number of chars   */
     /*  moved with the size n. If they are equal return. Else continue to  */
     /*  copy the remaining chars.                                          */
     /***********************************************************************/
     for (rn = 0; rn < nn; rn++) *rto++ = *rfrom++;
     if (nn == n) return (to);

     /***********************************************************************/
     /*  Write the memcpy of size >64K using nested for loops to make use   */
     /*  of BANZ instrunction.                                              */
     /***********************************************************************/
     {
        register unsigned int upper = (unsigned int)(n >> 16);
        register unsigned int tmp;
        for (tmp = 0; tmp < upper; tmp++)
	{
           for (rn = 0; rn < 65535; rn++) 
		*rto++ = *rfrom++;
	   *rto++ = *rfrom++;
	}
     }

     return (to);
}

