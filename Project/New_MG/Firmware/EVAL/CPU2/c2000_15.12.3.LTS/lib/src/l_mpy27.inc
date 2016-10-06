;*****************************************************************************
;* l_mpy27.inc  v15.12.3
;* 
;* Copyright (c) 1997-2016 Texas Instruments Incorporated
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
* This module contains the routine for 32-bit multiply
*
* SYMBOL DEFINITIONS:
*
* I,J - operands of the multiply
* xHI - High word of that object
* xLO - Low word of that object
*
******************************************************************************

;	STACK MAP
;
;	+-----------+	
;	|           |  0  <-- SP (on entry)
;	+-----------+	
;	| ret addr  | -1
;	+-----------+
;	| ret addr  | -2
;	+-----------+
;	|    JHI    | -3
;	+-----------+
;	|    JLO    | -4
;	+-----------+
;	|           |
;
; On entry, operand I is in accumulator 


******************************************************************************
* Set up aliases for stack, register references
******************************************************************************
	.asg	AH,IHI     	; high half of operand I
	.asg	AR6,ILO		; low half of operand I
	.asg	*-SP[1],JHI     ; high half of operand J
	.asg	*-SP[2],JLO     ; low half of operand J

	.page
******************************************************************************
* 16-BIT MULTIPLY, CALCULATE I * J AND RETURN IN ACCUMULATOR "A"
******************************************************************************
	.global	L$$MPY

L$$MPY:		.asmfunc
******************************************************************************
* SET UP 
******************************************************************************
	MOV	ILO,AL		; write out operand I

******************************************************************************
* A 32X32 MULTIPLY WITH A 64-BIT RESULT WOULD BE CALCULATED:
*
*                IHI ILO
*             X  JHI JLO
*             ----------
*             ILO * JLO
*       JLO * IHI
*       ILO * JHI
* IHI * JHI
* ----------------------
*     64-bit result
*
* BUT WE NEED ONLY THE LOWER 32-BITS OF THIS CALCULATION.  THEREFORE THE
* IHI * JHI CALCULATION ISN'T DONE AT ALL AND THE UPPER HALF OF THE JLO * IHI
* AND ILO * JHI CALCULATIONS IS THROWN AWAY.  ALSO, THE ILO * JLO MULTIPLY
* MUST BE UNSIGNED, BUT THE SIGNNESS OF THE OTHER MULTIPLIES DOESN'T MATTER
* SINCE THE DIFFERENCE ALWAYS APPEARS IN THE UPPER 16-BITS.
******************************************************************************
	MOV	T,JLO		; T = JLO
	MPY	ACC,T,IHI	; ACC = JLO * IHI
	MOV	T,ILO		; T = ILO
	MPY	P,T,JHI		; P = ILO * JHI
	ADD	ACC,P		; ACC += ILO * JHI
	MPYU	P,T,JLO		; P = ILO  * JLO
	MOV	ACC,AL << 16 	; ACC = ((JLO * IHI) + (ILO * JHI)) << 16
	ADD	ACC,P		; ACC = (((JLO * IHI) + (ILO * JHI)) << 16) +
	                        ;        (ILO * JLO)

******************************************************************************
* RETURN
******************************************************************************
	FFCRET	*XAR7		; return
	.endasmfunc
