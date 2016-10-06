;******************************************************************************
;*                                                                            *
;*  fd_util28.inc v15.12.3                                                    *
;*                                                                            *
;* Copyright (c) 2002-2016 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;*  fd_util28 - Macros to manipulate IEEE 754 format double precision         *
;*              floating point number.                                        *
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*; This file contains macros to help the implementation of 64 bit double rts  *
*; routines. All the 64 bit double math functions have common operations like *
*; unpacking, normalizing etc. These operations are implemented as the        *
*; following macros:                                                          *
*;                                                                            *
*; UNPACK64    - Unpack the 64 bit double representation into mantissa and    *
*;               exponent.                                                    *
*; OVERFLOW64  - Fill the ACC:P with the 64 bit overflow double value.        *
*; NORMALIZE64 - Normalize the mantissa for the 64 bit double representation. *
*; ROUND64     - Round the mantissa to the nearest value.                     *
*; PACK64      - Pack 64 bit double value.                                    *
*;                                                                            *
*; +------------------------------------------------------------------+
*; | DOUBLE PRECISION FLOATING POINT FORMAT                           |
*; |   64-bit representation                                          |
*; |   31 30      20 19                  0                            |
*; |   +-+----------+---------------------+                           |
*; |   |S|     E    |        M1           |    High Mantissa          |
*; |   +-+----------+---------------------+                           |
*; |                                                                  |
*; |   31                                0                            |
*; |   +----------------------------------+                           |
*; |   |             M2                   |    Low Mantissa           |
*; |   +----------------------------------+                           |
*; |                                                                  |
*; |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE               |
*; |                                 1 - NEGATIVE VALUE               |
*; |                                                                  |
*; |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0              |
*; |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)    |
*; |                        1111111111 - INFINITY                     |
*; |                                                                  |
*; |   <M1:M2>  MANTISSA FIELDS:  FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
*; +------------------------------------------------------------------+

*;*****************************************************************************
*; OVERFLOW64 - Generate the double overflow value. If sign is set then ACC:P *
*;              gets 0xFFF0000:000000000; otherwise 0x7FF00000:00000000       *
*;                                                                            *
*;              INPUT :                                                       *
*;                      SIGN - the sign of the overflow.                      *
*;              OUTPUT:                                                       *
*;                      the overflow value in ACC:P                           *
*;*****************************************************************************
*
OVERFLOW64	.macro   SIGN
	ZAPA				; Zero out ACC:P
	AND	AH, SIGN, #0x8000	; Pack the sign bit
	OR 	AH, #0x7FF0		; Pack 0x7FF for exponent 
	.endm
*
*;*****************************************************************************
*; UNPACK64   - Unpack the double float number into mantissa high, mantissa   *
*;              low and exponent. The implied 1 is added to the mantissa.     *
*;              The unpacked values are updated in the respective macro       *
*;              parameters.                                                   *
*;              1. Extract the exponent in OP1_EXP                            *
*;              2. Extract the mantissa in ACC:P in bits 62 to 11             *
*;              3. Add the implied one at bit 63                              *
*;                                                                            *
*;              INPUT :                                                       *
*;                      INP   - pointer to double precision floating point    *
*;				number        				      *
*;                      TMP32 - 32 bit temporary register                     *
*;              OUTPUT:                                                       *
*;                      EXP - 11 bit Exponent                                 *
*;                      ACC:P - mantissa with implied one                     *
*;*****************************************************************************
*
UNPACK64	.macro	INP, EXP
	MOVL	P, *+INP[0]	; Load input into ACC:P
	MOVL    ACC, *+INP[2]   ;
        LSL     AH, 1           ; remove sign
        LSR     AH, 5           ; remove high mantissa 
        MOV     EXP, AH         ; store exponent 
	MOVL	ACC, *+INP[2]   ; restore upper half of OP
	LSL64	ACC:P, 11	; Shift sign and 10 bits of exponent 
        OR      AH, #0x8000     ; add implied 1 to mantissa
	.endm
*
*;*****************************************************************************
*; NORMALIZE64 - Normalize the mantissa for the 64 bit double representation. *
*;                                                                            *
*;              INPUT :                                                       *
*;                      ACC:P - mantissa to be normalized.                    *
*;                      EXP   - 11 bit Exponent                               *
*;              OUTPUT:                                                       *
*;                      ACC:P - Normalized Mantissa                           *
*;                      EXP   - Exponent adjusted for normalization.          *
*; NOTE: EXP and TMP16 should be loc16                                        *
*;*****************************************************************************
*
NORMALIZE64	.macro  EXP, TMP16
        CSB     ACC             ; Find the number of sign bits in ACC (sn)
        LSL64   ACC:P, T        ; Shift (sn-1) bits out.
        MOV     TMP16, T	; Save the shift value in RES_EXP for now.
        CSB     ACC             ; Find the number of sign bits in ACC (sn)
        LSL64   ACC:P, T        ; Shift (sn-1) bits out.
	PUSH    AH              ; Save result high mantissa.
        MOV     AH, EXP         ; Adjust the exponent.
        SUB     AH, T		;                
        SUB     AH, TMP16       ;
	MOV     EXP, AH         ; save result exponent
	POP	AH 
	.endm

*;*****************************************************************************
*; ROUND64 - Round the mantissa to the nearest value.                         *
*;                                                                            *
*;              INPUT :                                                       *
*;                      ACC:P - Mantissa                                      *
*;                      RES_EXP - 11 bit Exponent                             *
*;                      TMP32   - 32 bit Temporary register                   *
*;              OUTPUT:                                                       *
*;                      ACC:P - Rounded mantissa                              *
*;                      RES_EXP - Rounding Adjusted exponent.                 *
*; NOTE: RES_EXP should be from loc16                                         *
*;       TMP32 should be from loc32                                           *
*;*****************************************************************************
*
ROUND64	.macro	RES_EXP, TMP32
        MOVL    TMP32, #0x200   ;
        ADDUL   P, TMP32        ;Round to the nearest
        MOVL    TMP32, #0       ;
        ADDCL   ACC, TMP32      ;

        B       RND64_SET_MANT?, GEQ  ;
        AND     AH, #0x7FFF     ; Clear the carry bit.                         
        INC     RES_EXP         ; Adjust exponent if rounding generated a carry
RND64_SET_MANT?
	.endm

*;*****************************************************************************
*; PACK64 - Pack 64 bit double precision IEEE 754 format number               *
*;                                                                            *
*;              INPUT :                                                       *
*;                      ACC:P - Mantissa in bits 51 - 0                       *
*;                      SIGN  - Sign of the double value.                     *
*;                      EXP   - 11 bit Exponent                               *
*;              OUTPUT:                                                       *
*;                      ACC:P - Packed IEEE 754 foramt double precision       *
*;                              floating point value.                         *
*; NOTE: EXP and SIGN should be from loc16                                    *
*;*****************************************************************************
*
PACK64	.macro	SIGN, EXP          ;            
	AND	SIGN, #0x8000	   ; ACC:P 0000 0000 0000 MMMM : M47 - M0     
	OR 	SIGN, AH	   ; SIGN = S000 0000 0000 MMMM
	MOV	AH, EXP		   ; AH = 0000 0EEE EEEE EEEE
	LSL	AH, #4		   ; AH = 0EEE EEEE EEEE 0000
	OR	AH, SIGN	   ; ACC:P = SEEE EEEE EEEE MMMM : M47 - M0
	.endm

