;******************************************************************************
;* IQ XMPY CODE FOR C28X v15.12.3                                             *
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
;*                                                                            *
;* __IQXMPY  - Implementation of __IQxmpy() intrinsic when N is not a         *
;*             constant.                                                      *
;******************************************************************************
;* __IQxmpy(long A, long B, int N)                                            *
;*                                          A is in ACC                       *
;*                                          B is in *-SP[4]                   *
;*                                          N is in AR4                       *
;* Result is returned in ACC.                                                 *
;******************************************************************************

	.sect	".text"
	.global	__IQXMPY

__IQXMPY:	.asmfunc stack_usage(2)
        MOVL XAR7,ACC          ; save A
	MOV  AL,AR4            ; test N 
	SB $10,LT
        MOVL XT,XAR7           ; N is positive
	IMPYL P,XT,*-SP[4]     ; calculate A * B = 64-bits result
	QMPYL ACC,XT,*-SP[4]
	MOV T,AR4              ; 
	LSL64 ACC:P,T          ; scale the result by N and return in ACC.
        LRETR

$10:                           ; N is negative
        NEG AL                 ; negate N
        MOV AR4,AL       
        MOVL XT,XAR7
	IMPYL P,XT,*-SP[4]     ; calculate A * B = 64-bits result
	QMPYL ACC,XT,*-SP[4]
	MOV T,AR4
	ASR64 ACC:P,T          ; scale the result by N and return in ACC.
        LRETR

	.endasmfunc

