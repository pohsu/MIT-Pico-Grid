;*****************************************************************************
;* PROCEDURAL ABSTRACTION FUNCTIONS FOR C28X v15.12.3                         *
;*                                                                           *
;* Copyright (c) 2001-2016 Texas Instruments Incorporated                    *
;* http://www.ti.com/                                                        *
;*                                                                           *
;*  Redistribution and  use in source  and binary forms, with  or without    *
;*  modification,  are permitted provided  that the  following conditions    *
;*  are met:                                                                 *
;*                                                                           *
;*     Redistributions  of source  code must  retain the  above copyright    *
;*     notice, this list of conditions and the following disclaimer.         *
;*                                                                           *
;*     Redistributions in binary form  must reproduce the above copyright    *
;*     notice, this  list of conditions  and the following  disclaimer in    *
;*     the  documentation  and/or   other  materials  provided  with  the    *
;*     distribution.                                                         *
;*                                                                           *
;*     Neither the  name of Texas Instruments Incorporated  nor the names    *
;*     of its  contributors may  be used to  endorse or  promote products    *
;*     derived  from   this  software  without   specific  prior  written    *
;*     permission.                                                           *
;*                                                                           *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS    *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT    *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT    *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT    *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT    *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     *
;*                                                                           *
;*****************************************************************************

;*****************************************************************************
;* ABSTRACT FUNCTION FOR SATLOW16 INTRINSIC                                  *
;*****************************************************************************
	.sect	".text"
	.global	_satlow16

_satlow16:	.asmfunc stack_usage(2)
	SETC	OVM
	CLRC	SXM
	ADD	ACC,#65535 << 15
	SUB	ACC,#65535 << 15
	SUB	ACC,#65535 << 15
	ADD	ACC,#65535 << 15
	CLRC	OVM
	LRETR
	.endasmfunc

;*****************************************************************************
;* ABSTRACT FUNCTION FOR ARRAY COPIES                                        *
;*****************************************************************************
	.sect	".text"
	.global	_abstract_func1_c28x

_abstract_func1_c28x:	.asmfunc stack_usage(2)
	MOV 	AL,*XAR5++
	MOV 	*XAR4++,AL
	BANZ 	_abstract_func1_c28x,AR6--
	LRETR
	.endasmfunc

;*****************************************************************************
;* ABSTRACT FUNCTION FOR ARRAY COPIES                                        *
;*****************************************************************************
	.sect	".text"
	.global	_abstract_func2_c28x

_abstract_func2_c28x:	.asmfunc stack_usage(2)
	MOV 	AL,*XAR4++
	MOV 	*XAR5++,AL
	BANZ 	_abstract_func2_c28x,AR6--
	LRETR
	.endasmfunc
