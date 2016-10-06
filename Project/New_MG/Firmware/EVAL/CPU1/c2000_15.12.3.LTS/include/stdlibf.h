/*****************************************************************************/
/* stdlibf.h   v15.12.3                                                      */
/*     far versions of stdlib.h functions                                    */
/*                                                                           */
/* Copyright (c) 1996-2016 Texas Instruments Incorporated                    */
/* http://www.ti.com/                                                        */
/*                                                                           */
/*  Redistribution and  use in source  and binary forms, with  or without    */
/*  modification,  are permitted provided  that the  following conditions    */
/*  are met:                                                                 */
/*                                                                           */
/*     Redistributions  of source  code must  retain the  above copyright    */
/*     notice, this list of conditions and the following disclaimer.         */
/*                                                                           */
/*     Redistributions in binary form  must reproduce the above copyright    */
/*     notice, this  list of conditions  and the following  disclaimer in    */
/*     the  documentation  and/or   other  materials  provided  with  the    */
/*     distribution.                                                         */
/*                                                                           */
/*     Neither the  name of Texas Instruments Incorporated  nor the names    */
/*     of its  contributors may  be used to  endorse or  promote products    */
/*     derived  from   this  software  without   specific  prior  written    */
/*     permission.                                                           */
/*                                                                           */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS    */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT    */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT    */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT    */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     */
/*                                                                           */
/*****************************************************************************/
#ifndef _STDLIBF
#define _STDLIBF

#pragma diag_push
#pragma CHECK_MISRA("-6.3")

#ifndef __cplusplus
_CODE_ACCESS long          far_strtol (const char *st,
                                       char **endptr, 
				       int base);
_CODE_ACCESS unsigned long far_strtoul(const char *st,
                                       char **endptr, 
				       int base);
_CODE_ACCESS double        far_strtod (const char *st,
                                       char **endptr);
_CODE_ACCESS long double   far_strtold (const char *st,
                                        char **endptr);
_CODE_ACCESS long long     far_strtoll(const char *st,
                                       char **endptr, 
		 		       int base);
_CODE_ACCESS unsigned long long far_strtoull(const char *st, 
                                                   char **endptr,
                                                   int base);
_CODE_ACCESS int           far_atoi(const char *st);
_CODE_ACCESS long          far_atol(const char *st);
_IDECL       double        far_atof(const char *st);
_CODE_ACCESS long long     far_atoll(const char *st);

_CODE_ACCESS void     *far_calloc  (unsigned long num, unsigned long size);
_CODE_ACCESS void     *far_malloc  (unsigned long size);
_CODE_ACCESS void     *far_realloc (void *ptr, unsigned long size);
_CODE_ACCESS void          far_free    (void *ptr);

_CODE_ACCESS void     *far_bsearch(const void *key, 
                                       const void *base, 
			               size_t nmemb, size_t size, 
			               int (*compar)(const void *a,
                                                     const void *b));
_CODE_ACCESS void          far_qsort  (void *base, 
                                       size_t nmemb, size_t size,
			               int (*compar)(const void *a, 
                                                     const void *b));

_CODE_ACCESS int	   free_memory(void);
_CODE_ACCESS int           max_free(void);
_CODE_ACCESS void         *chkheap(void);

#ifdef _INLINE
#pragma diag_push
#pragma CHECK_MISRA("-8.5") /* need to define inline function */
static __inline double far_atof(const char *st) 
{ 
   return far_strtod(st, (char **)NULL);
}
#pragma diag_pop
#endif /* _INLINE */

#else /* __cplusplus */

namespace std {

extern "C" int	       free_memory(void);
extern "C" int         max_free(void);
extern "C" void        *chkheap(void);
}
#endif /* __cplusplus */

#pragma diag_pop

#endif /* _STDLIBF */

#pragma diag_push

/* using declarations must occur outside header guard to support including both
   C and C++-wrapped version of header; see _CPP_STYLE_HEADER check */
/* this code is for C++ mode only and thus also not relevant for MISRA */
#pragma CHECK_MISRA("-19.15")

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::free_memory; 
using std::max_free;
using std::chkheap;
#endif /* _CPP_STYLE_HEADER */

#pragma diag_pop
