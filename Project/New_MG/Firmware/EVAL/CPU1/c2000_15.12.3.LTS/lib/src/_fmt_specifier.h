/****************************************************************************/
/*  _FMT_SPECIFIER.H v15.12.3                                               */
/*                                                                          */
/* Copyright (c) 2006-2016 Texas Instruments Incorporated                   */
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
/* 7.8.1 Macros for format specifiers */


#define PRIdLEAST8     	"d"
#define PRIiLEAST8     	"i"
#define PRIoLEAST8     	"o"
#define PRIuLEAST8     	"u"
#define PRIxLEAST8     	"x"
#define PRIXLEAST8     	"X"
#define SCNdLEAST8 	"d"
#define SCNiLEAST8 	"i"
#define SCNoLEAST8 	"o"
#define SCNuLEAST8 	"u"
#define SCNxLEAST8 	"x"
#define SCNd16		"d"
#define SCNi16		"i"
#define SCNo16		"o"
#define SCNu16		"u"
#define SCNx16		"x"
#define SCNdLEAST16 	"d"
#define SCNiLEAST16 	"i"
#define SCNoLEAST16 	"o"
#define SCNuLEAST16 	"u"
#define SCNxLEAST16 	"x"

#define PRIdFAST8     	"d"
#define PRIiFAST8     	"i"
#define PRIoFAST8     	"o"
#define PRIuFAST8     	"u"
#define PRIxFAST8     	"x"
#define PRIXFAST8     	"X"
#define PRIdFAST16    	"d"
#define PRIiFAST16    	"i"
#define PRIoFAST16    	"o"
#define PRIuFAST16    	"u"
#define PRIxFAST16    	"x"
#define PRIXFAST16    	"X"
#define SCNdFAST8 	"d"
#define SCNiFAST8 	"i"
#define SCNoFAST8 	"o"
#define SCNuFAST8 	"u"
#define SCNxFAST8 	"x"
#define SCNdFAST16 	"d"
#define SCNiFAST16 	"i"
#define SCNoFAST16 	"o"
#define SCNuFAST16 	"u"
#define SCNxFAST16 	"x"

#define PRId16		"d"
#define PRIi16		"i"
#define PRIo16		"o"
#define PRIu16		"u"
#define PRIx16		"x"
#define PRIX16		"X"
#define PRIdLEAST16    	"d"
#define PRIiLEAST16    	"i"
#define PRIoLEAST16    	"o"
#define PRIuLEAST16    	"u"
#define PRIxLEAST16    	"x"
#define PRIXLEAST16    	"X"

#define PRId32		"ld"
#define PRIi32		"li"
#define PRIo32		"lo"
#define PRIu32		"lu"
#define PRIx32		"lx"
#define PRIX32		"lX"
#define PRIdLEAST32    	"ld"
#define PRIiLEAST32    	"li"
#define PRIoLEAST32    	"lo"
#define PRIuLEAST32    	"lu"
#define PRIxLEAST32    	"lx"
#define PRIXLEAST32    	"lX"
#define PRIdFAST32    	"ld"
#define PRIiFAST32    	"li"
#define PRIoFAST32    	"lo"
#define PRIuFAST32    	"lu"
#define PRIxFAST32    	"lx"
#define PRIXFAST32    	"lX"
#define SCNd32		"ld"
#define SCNi32		"li"
#define SCNo32		"lo"
#define SCNu32		"lu"
#define SCNx32		"lx"
#define SCNdLEAST32    	"ld"
#define SCNiLEAST32    	"li"
#define SCNoLEAST32    	"lo"
#define SCNuLEAST32    	"lu"
#define SCNxLEAST32    	"lx"
#define SCNdFAST32    	"ld"
#define SCNiFAST32    	"li"
#define SCNoFAST32    	"lo"
#define SCNuFAST32    	"lu"
#define SCNxFAST32    	"lx"


#if defined(__TMS470__) || defined(_TMS320C6X) || defined(__TMS320C28X__) || \
    defined(__ARP32__)  || defined(__MSP430__) || defined(__PRU__)        || \
    defined(__FROZEN__)
#define PRId64		"lld"
#define PRIi64		"lli"
#define PRIo64		"llo"
#define PRIu64		"llu"
#define PRIx64		"llx"
#define PRIX64		"llX"
#define PRIdLEAST64    	"lld"
#define PRIiLEAST64    	"lli"
#define PRIoLEAST64    	"llo"
#define PRIuLEAST64    	"llu"
#define PRIxLEAST64    	"llx"
#define PRIXLEAST64    	"llX"
#define PRIdFAST64    	"lld"
#define PRIiFAST64    	"lli"
#define PRIoFAST64    	"llo"
#define PRIuFAST64    	"llu"
#define PRIxFAST64    	"llx"
#define PRIXFAST64    	"llX"
#define SCNd64		"lld"
#define SCNi64		"lli"
#define SCNo64		"llo"
#define SCNu64		"llu"
#define SCNx64		"llx"
#define SCNdLEAST64    	"lld"
#define SCNiLEAST64    	"lli"
#define SCNoLEAST64    	"llo"
#define SCNuLEAST64    	"llu"
#define SCNxLEAST64    	"llx"
#define SCNdFAST64    	"lld"
#define SCNiFAST64    	"lli"
#define SCNoFAST64    	"llo"
#define SCNuFAST64    	"llu"
#define SCNxFAST64    	"llx"
#endif

#define PRIdPTR		"ld"
#define PRIiPTR		"li"
#define PRIoPTR		"lo"
#define PRIuPTR		"lu"
#define PRIxPTR		"lx"
#define PRIXPTR		"lX"
#define SCNdPTR		"ld"
#define SCNiPTR		"li"
#define SCNoPTR		"lo"
#define SCNuPTR		"lu"
#define SCNxPTR		"lx"

#if defined(__TMS320C27X__) || defined(_TMS320C5XX)
#define PRIdMAX		"ld"
#define PRIiMAX		"li"
#define PRIoMAX		"lo"
#define PRIuMAX		"lu"
#define PRIxMAX		"lx"
#define PRIXMAX		"lX"
#define SCNdMAX		"ld"
#define SCNiMAX		"li"
#define SCNoMAX		"lo"
#define SCNuMAX		"lu"
#define SCNxMAX		"lx"
#elif defined(__TMS320C28X__) || defined(__TMS320C55X__) || \
      defined(__TMS470__)     || defined(_TMS320C6X)     || \
      defined(__ARP32__)      || defined(__MSP430__)     || \
      defined(__PRU__)        || defined(__FROZEN__)
#define PRIdMAX		"lld"
#define PRIiMAX		"lli"
#define PRIoMAX		"llo"
#define PRIuMAX		"llu"
#define PRIxMAX		"llx"
#define PRIXMAX		"llX"
#define SCNdMAX		"lld"
#define SCNiMAX		"lli"
#define SCNoMAX		"llo"
#define SCNuMAX		"llu"
#define SCNxMAX		"llx"
#endif
