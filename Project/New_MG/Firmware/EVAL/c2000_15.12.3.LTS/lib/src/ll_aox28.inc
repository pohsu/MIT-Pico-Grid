;*****************************************************************************
;* ll_aox28.inc  v15.12.3
;* 
;* Copyright (c) 2003-2016 Texas Instruments Incorporated
;* http://www.ti.com/ 
;* 
;*  Redistribution and  use in source  and binary forms, with  or without
;*  modification,  are permitted provided  that the  following conditions
;*  are met:
;* 
;*     Redistributions  of source  code must  retain the  above copyright
;*     notice, this list of conditions and the following disclaimer.
;* 
;*     Redistributions in binary form  must reproduce the above copyright
;*     notice, this  list of conditions  and the following  disclaimer in
;*     the  documentation  and/or   other  materials  provided  with  the
;*     distribution.
;* 
;*     Neither the  name of Texas Instruments Incorporated  nor the names
;*     of its  contributors may  be used to  endorse or  promote products
;*     derived  from   this  software  without   specific  prior  written
;*     permission.
;* 
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*****************************************************************************

******************************************************************************
* This module contains 64-bit bitwise AND, OR and XOR routines.              
******************************************************************************

	.page

	.global	LL$$AND
	.global	LL$$OR
	.global	LL$$XOR

        .asg   *-SP[1],    HI_MSB
        .asg   *-SP[2],    HI_LSB
        .asg   *-SP[3],    LO_MSB
        .asg   *-SP[4],    LO_LSB


LL$$AND:	.asmfunc stack_usage(0)
	AND	HI_MSB, AH
	AND	HI_LSB, AL
	MOVL	ACC,LO_LSB
	AND	AH,PH
	AND	AL,PL
	MOVL	P,ACC
	MOVL	ACC,HI_LSB

	FFCRET	*XAR7		; return
	.endasmfunc

LL$$OR:		.asmfunc stack_usage(0)
	OR	HI_MSB, AH
	OR	HI_LSB, AL
	MOVL	ACC,LO_LSB
	OR	AH,PH
	OR	AL,PL
	MOVL	P,ACC
	MOVL	ACC,HI_LSB

	FFCRET	*XAR7		; return
	.endasmfunc

LL$$XOR:	.asmfunc stack_usage(0)
	XOR	HI_MSB, AH
	XOR	HI_LSB, AL
	MOVL	ACC,LO_LSB
	XOR	AH,PH
	XOR	AL,PL
	MOVL	P,ACC
	MOVL	ACC,HI_LSB

	FFCRET	*XAR7		; return
	.endasmfunc

