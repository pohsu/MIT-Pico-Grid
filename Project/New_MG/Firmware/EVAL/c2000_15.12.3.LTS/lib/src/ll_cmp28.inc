;*****************************************************************************
;* ll_cmp28.inc  v15.12.3
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
* This module contains signed and unsigned 64-bit compares.           
*
* SYMBOL DEFINITIONS:
*
* I,J - Input 1 and 2 that needs to be compared.
* AL  - Return value
*     OP1 > OP2  : return 1
*     OP1 < OP2  : return -1
*     OP1 == OP2 : return 0
* xHI - High 32bits of that object
* xLO - Low 32bits of that object
*
******************************************************************************

******************************************************************************
* Set up aliases for stack, register references
******************************************************************************
	.asg   *-SP[2],  JHI
	.asg   *-SP[4],  JLO

	.page
******************************************************************************
* Signed Compare (LL$CMP):
* if IHI is equal to JHI
*    return 0 if ILO is equal to JLO
*    return 1 if ILO is higher than JLO (unsigned compare)
*    return -1 if ILO is lower than JLO (unsigned compare)
* else
*    return 0 if IHI is equal to JHI
*    return 1 if IHI is greater than JHI (signed compare)
*    return -1 if IHI is less than JHI (signed compare)
*    
* Unsigned Compare (ULL$CMP):
* if IHI is equal to JHI
*    return 0 if ILO is equal to JLO
*    return 1 if ILO is higher than JLO (unsigned compare)
*    return -1 if ILO is lower than JLO (unsigned compare)
* else
*    return 0 if IHI is equal to JHI
*    return 1 if IHI is higher than JHI (unsigned compare)
*    return -1 if IHI is lower than JHI (unsigned compare)
*    
*                        
******************************************************************************
	.global	LL$$CMP

LL$$CMP:	.asmfunc stack_usage(0)
	MOV	T,#-1		;
	CMPL	ACC,JHI		; 
	B	$5,NEQ	;

	MOVL	ACC,P  		; 
	CMPL	ACC,JLO		;
	MOVB	T,#0,EQ		; 
	MOVB	T,#1,HI		;
	MOV 	AL,T     	;

	FFCRET	*XAR7		; Return
$5:
	MOVB	T,#0,EQ		;
	MOVB	T,#1,GT		;
	MOV 	AL,T     	;

	FFCRET	*XAR7		; Return
	.endasmfunc

	.global	ULL$$CMP

ULL$$CMP:	.asmfunc stack_usage(0)
	MOV	T,#-1		;
	CMPL	ACC,JHI		; 
	B	$10,NEQ	;
	MOVL	ACC,P		;
	CMPL	ACC,JLO		;
$10:
	MOVB	T,#0,EQ		;
	MOVB	T,#1,HI		;
	MOV 	AL,T     	;

	FFCRET	*XAR7		; Return
	.endasmfunc

