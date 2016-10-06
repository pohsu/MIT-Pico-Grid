;*****************************************************************************
;* ll_mpy28.inc  v15.12.3
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
* This module contains the routine for 64-bit multiply
*
* SYMBOL DEFINITIONS:
*
* I,J - operands of the multiply in ACC:P and stack respectively
* R   - Result of the multiply returned in ACC:P
* xHI - High 32bits of that object
* xLO - Low 32bits of that object
*
******************************************************************************

******************************************************************************
* Set up aliases for stack, register references
******************************************************************************
        .asg   *-SP[6],    IHI 
        .asg   *-SP[8],    ILO 
        .asg   *-SP[2],    JHI
        .asg   *-SP[4],    JLO
        .asg   XAR4,       RLO 

	.page
******************************************************************************
* A 64X64 MULTIPLY WITH A 128-BIT RESULT WOULD BE CALCULATED:
*
*                IHI ILO
*             X  JHI JLO
*             ----------
*             ILO * JLO
*       JLO * IHI
*       ILO * JHI
* IHI * JHI
* ----------------------
*             RHI : RLO
*     128-bit result
*
* BUT WE NEED ONLY THE LOWER 64-BITS OF THIS CALCULATION.  THEREFORE THE
* IHI * JHI CALCULATION ISN'T DONE AT ALL AND THE UPPER HALF OF THE JLO * IHI
* AND ILO * JHI CALCULATIONS IS THROWN AWAY.  ALSO, THE ILO * JLO MULTIPLY
* MUST BE UNSIGNED, BUT THE SIGNNESS OF THE OTHER MULTIPLIES DOESN'T MATTER
* SINCE THE DIFFERENCE ALWAYS APPEARS IN THE UPPER 32-BITS.
******************************************************************************
	.global	LL$$MPY

LL$$MPY:	.asmfunc stack_usage(4)
	PUSH	P		; Push IHI:ILO
	PUSH	ACC		;

	MOVL	XT,JLO		; 
	IMPYL	P, XT, ILO	; 
	MOVL	RLO, P		; RLO = LO32(JLO * ILO)
	QMPYUL	P, XT, ILO	; P   = HI32(JLO * ILO)
	MOVL	XT, JHI		; 
	MOVL	ACC, P		; ACC = HI32(JLO * ILO) 
	IMPYXUL P, XT, ILO	; P   = LO32(JHI * ILO) 
	ADDL    ACC,P		; ACC = HI32(JLO * ILO) + LO32(JHI * ILO)
	MOVL	XT, IHI		; 
	IMPYXUL P, XT, JLO	; P = LO32(IHI * JLO) ==> HI32(RESULT)
	ADDL    ACC,P		; ACC += P
	MOVL    P, RLO   	; P   ==> LO32(RESULT)

	SUBB    SP,#4		; Adjust the stack on return.

******************************************************************************
* RETURN
******************************************************************************
	FFCRET	*XAR7		; return
	.endasmfunc

