/*****************************************************************************/
/* stdarg.h   v15.12.3                                                       */
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

#ifndef _STDARG
#define _STDARG

#include <linkage.h>

#if defined(__TMS320C28XX_CLA__)
#error "Header file <stdarg.h> not supported by CLA compiler"
#endif

#pragma diag_push
#pragma CHECK_MISRA("-20.1") /* standard headers must define standard names */
#pragma CHECK_MISRA("-20.2") /* standard headers must define standard names */
#pragma CHECK_MISRA("-19.7") /* macros required for implementation */
#pragma CHECK_MISRA("-19.10") /* need types as macro arguments */

#ifdef __cplusplus
#define va_start(ap, parmN) (ap = ((std::va_list)__va_parmadr(parmN)))
namespace std {
#else
#define va_start(ap, parmN) (ap = ((va_list)__va_parmadr(parmN)))
#endif /* __cplusplus */

typedef char *va_list;

#ifdef __cplusplus
} /* namespace std */
#endif

#define va_end(ap) ((void)0)

/****************************************************************************/
/* RETURN THE NEXT VALUE ON THE STACK ...                                   */
/*                                                                          */
/* (<advance ap>, <access the value>) BECOMES ...                           */
/*                                                                          */
/* ap -= 1 (stack grows toward high addresses)                              */
/* ap -= 1 more if type is long or float                                    */
/* ap -= 1 more if type is long or float and to account for alignment       */
/*       if necessary                                                       */
/*                                                                          */
/* if      (<type is passed by address [struct or union]>) return **ap;     */
/* else if (<type is integer size>)                        return *ap;      */
/*                                                                          */
/* LONG/FLOATS ARE ALWAYS ALIGNED ON AN EVEN WORD BOUNDARY, EVEN WHEN       */
/* PASSED AS PARAMETERS, THUS ap MUST BE ALIGNED FOR THOSE ACCESSES.        */
/****************************************************************************/
#if __LARGE_MODEL__
#define va_arg(ap, type)                                                 \
( (ap -= 1 + (__va_argref(type) ? (((long)ap & 1) ? 2 : 1) :		 \
				  (sizeof(type) == sizeof(int)) ? 0 :    \
                                   ((long)ap & 1) ? sizeof(type) :       \
                                                    sizeof(type) - 1 )), \
   (__va_argref(type)             ? (** (type **) (ap)) : (* (type *) (ap)) ))
#else
#define va_arg(ap, type)                                                 \
( (ap -= 1 + (__va_argref(type) ? (((int)ap & 1) ? 2 : 1) : 		 \
				  (sizeof(type) == sizeof(int)) ? 0 :    \
                                   ((int)ap & 1) ? sizeof(type)      :   \
                                                   sizeof(type) - 1 )),  \
   (__va_argref(type)             ? (** (type **) (ap)) : (* (type *) (ap)) ))
#endif

#pragma diag_pop

#endif /* _STDARG */

#pragma diag_push

/* using declarations must occur outside header guard to support including both
   C and C++-wrapped version of header; see _CPP_STYLE_HEADER check */
/* this code is for C++ mode only and thus also not relevant for MISRA */
#pragma CHECK_MISRA("-19.15")

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::va_list;
#endif /* _CPP_STYLE_HEADER */

#pragma diag_pop
