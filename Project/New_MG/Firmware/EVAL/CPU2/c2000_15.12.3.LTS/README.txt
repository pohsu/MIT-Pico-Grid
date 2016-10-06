C2000 C/C++ CODE GENERATION TOOLS
Release Notes
15.12.3.LTS
June 2016

*******************************************************************************
                             Table of Contents
*******************************************************************************
1) Support Information

2) DWARF 4

3) C2000 Unsigned Integer Division Intrinsics

4) Byte Peripherals Support

5) CLA-only Object File Compatibility

6) Aggregate data in subsections

* Features Included from 15.9.0.STS:
7)  New CLA Compiler Features
      7.1 Removal of CLA Language Restrictions
      7.2 Same Default Language Options as C2000
      7.3 --cla_default Option
8)  RAM Function Support
9)  New C2000 Intrinsics __eallow, __edis
10) Module summary in linker map file

* Features Included from v15.3.0.STS:
11) C99 Math RTS Library Support

-------------------------------------------------------------------------------
1) Support Information
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
1.1) List of Fixed and Known Defects
-------------------------------------------------------------------------------

The list of defects fixed in this release as well as known issues can
be found in the file DefectHistory.txt.

-------------------------------------------------------------------------------
1.2) Compiler Wiki
-------------------------------------------------------------------------------

A Wiki has been established to assist developers in using TI Embedded
Processor Software and Tools.  Developers are encouraged to read and
contribute to the articles.  Registered users can update missing or
incorrect information.  There is a large section of compiler-related
material.  Please visit:

http://processors.wiki.ti.com/index.php?title=Category:Compiler

-------------------------------------------------------------------------------
1.3) Compiler Documentation Errata
-------------------------------------------------------------------------------

Errata for the "MSP430 Optimizing Compiler User's Guide" and the
"MSP430 Assembly Language User's Guide" is available online at the
Texas Instruments Embedded Processors CG Wiki:

http://processors.wiki.ti.com/index.php?title=Category:Compiler

under the 'Compiler Documentation Errata' link.

-------------------------------------------------------------------------------
1.4) TI E2E Community
-------------------------------------------------------------------------------

Questions concerning TI Code Generation Tools can be posted to the TI E2E
Community forums.  The "Development Tools" forum can be found at:

http://e2e.ti.com/support/development_tools/f/default.aspx

-------------------------------------------------------------------------------
1.5) Defect Tracking Database
-------------------------------------------------------------------------------

Compiler defect reports can be tracked at the Development Tools bug
database, SDOWP. The log in page for SDOWP, as well as a link to create
an account with the defect tracking database is found at:

https://cqweb.ext.ti.com/pages/SDO-Web.html

A my.ti.com account is required to access this page.  To find an issue
in SDOWP, enter your bug id in the "Find Record ID" box once logged in.
To find tables of all compiler issues click the queries under the folder:

"Public Queries" -> "Development Tools" -> "TI C-C++ Compiler"

With your SDOWP account you can save your own queries in your
"Personal Queries" folder.

-------------------------------------------------------------------------------
2.  DWARF 4
-------------------------------------------------------------------------------
This release introduces the option to use the DWARF 4 Debugging Format.
DWARF 3 is still enabled by default, but DWARF 4 may be enabled by using
--symdebug:dwarf_version=4.  The RTS still uses DWARF 3.  DWARF versions 2, 3,
and 4 may be intermixed safely.

When DWARF 4 is enabled, type information will be placed in the new
.debug_types section.  At link time, duplicate type information will be
removed.  This method of type merging is superior to those used in DWARF 2 or 3
and will result in a smaller executable.  In addition, the size of intermediate
object files will be reduced in comparison to DWARF 3.

For more information, see:
http://processors.wiki.ti.com/index.php/DWARF_4

-------------------------------------------------------------------------------
3.  C2000 Unsigned Integer Division Intrinsics
-------------------------------------------------------------------------------
The following intrinsic provides efficient support for signed by unsigned long
modulus and division. This variant, better suited for control applications,
varies from the C-language standard in that the quotient does not round towards
0 and the "remainder" is a true modulus calculation.

   long quotient = __euclidean_div_i32byu32(long numerator, 
   		   			    unsigned long denominator, 
					    unsigned long &remainder);

The following intrinsics are also provided and map directly to hardware
instructions <SUBCUL ACC, loc32> and its repeat version.

Repeat conditional long subtraction used in unsigned modulus division:
   unsigned long quotient = __rpt_subcul(unsigned long numerator, 
  	   		   	         unsigned long denominator, 
				 	 unsigned long &remainder, 
					 int rpt_cnt);

Conditional long subraction instruction
    unsigned long quotient = __subcul(unsigned long numerator, 
   	    	 	              unsigned long denominator,
                                      unsigned long &remainder);

-------------------------------------------------------------------------------
4)  Byte Peripherals Support
-------------------------------------------------------------------------------
The C2000 architecture has 16-bit words.  Some peripherals, however, are 8-bit
byte addressable.  The  byte peripherals bridge translates addresses between
the CPU and the byte peripherals by treating the address as a byte address.  
Therefore, only some C2000 addresses map correctly to the byte peripherals. 
Even and odd addresses to 16-bit data will both map to the same data element on
the byte peripheral. The same is true for addresses to 32-bit data. Thus,
addresses for 16-bit accesses must be 32-bit aligned and those for 32-bit
accesses must be 64-bit aligned. 

Driver libraries and bitfield header files are provided to access
periperhals. To support these libraries and enable customers to extend them or
write their own, we introduce a source-level intrinsic and type attributes
to access byte peripheral data.

The C2000 driver library accesses byte peripheral data at the correct starting
address. However, on C2000, operations on 32-bit data are often broken up
into 2 operations on 16-bit data because these are more efficient on the
architecture. Accesses to 32-bit byte peripheral data cannot be broken up
regularly into 2 16-bit accesses because the starting offset for the second
16-bits will be incorrect.  Thus, the __byte_peripheral_32 intrinsic can be
used to access a 32-bit byte peripherals data address, preventing these
accesses from being broken up. The intrinsic returns a reference to an 
unsigned long and can be used both to read and write data.

   unsigned long &__byte_peripheral_32(unsigned long *x);

For bitfield support, 2 type attributes have been introduced that can  be 
applied to typedefs of unsigned ints and unsigned longs.
For example:
   typedef unsigned int bp_16 __attribute__((byte_peripheral)) ; 
   typedef unsigned long bp_32 __attribute__((byte_peripheral));

The typedef names do not matter. The attributes automatically apply the
volatile key word and handle alignment. All struct members for byte
peripheral structs, whether bitfields or not, must have these attributes
applied via typedefs to ensure proper alignment of the struct accesses. 
Please note that struct layout will be different due to differences in 
alignment, so the bitfields cannot always be accessed via the same 
container types as in regular structs.

For example, in the following examples, the bit positions for byte
peripheral bitfield types are shown in comparison to regular bitfields.
Because 16-bit accesses must be 32-bit aligned, it is not possible to
access bits at offsets 16-31 with a 16-bit container.  To access these
bits, a 32-bit container must be used.  In both examples, the same
layout as in the regular case can be achieved by changing the field 
types to bp_32 from field 4 on in example 1 and field 2 on in example 2.

struct example1 {       // regular bits position  //byte peripherals
    bp_16 field1:9;     // 0-8                     // 0-8
    bp_16 field2:6;     // 9-14                    // 9-14
    bp_32 field3:4;     // 15-18                   // 15-18
    bp_16 field4:1;     // 19                      // 32
    bp_16 field5:5;     // 20-24                   // 33-37
    bp_16 field6:7;     // 25-31                   // 38-44
};

struct example2{        // regular bits position //byte peripherals
    bp_32 field1:29;    // 0-28                  // 0-28
    bp_16 field2:1;     // 29                    //32
    bp_16 field3:1;     // 30                    //33
    bp_16 field4:1;     // 31                    //34
};

Because the alignment will create padding in any objects that are declared,
it is recommended that byte peripheral addresses be cast as byte peripheral
struct types instead of declaring objects of those struct types.

-------------------------------------------------------------------------------
5)  CLA-only Object File Compatibility
-------------------------------------------------------------------------------
CLA-only object files will now be compatible with any C2000 object files
regardless of device support options for FPU and TMU.  Previously, when CLA
object files were compiled, any C2000 device support options limited linking
compatibility even though code for those devices was not present in the 
CLA object.


--------------------------------------------------------------------------------
6) Aggregate data in subsections
--------------------------------------------------------------------------------
The compiler will now place all aggregate data (arrays, structs, and unions)
into subsections. This gives the linker more granularity for removing unused
data during the final link step. The behavior can be controlled using the
--gen_data_subsections=on,off option. The default is on.


-------------------------------------------------------------------------------
7)  New CLA Compiler Features
-------------------------------------------------------------------------------
The 15.6.0.STS version of the C2000 compiler continues to improve support and
ease of use for CLA code generation. 

-------------------------------------------------------------------------------
7.1  Removal of CLA Language Restrictions
-------------------------------------------------------------------------------
Integer divide, modulus and unsigned integer compares are now permitted in CLA
code. If the compiler can't eliminate or transform these non-native operations
through optimization, performance advice will be issued indicating that the 
operations might lead to poor performance.  

-------------------------------------------------------------------------------
7.2 Same Default Language Options as C2000
-------------------------------------------------------------------------------
The compiler will now support the same default language options for CLA as 
for C2000, such as being in GCC mode by default.

-------------------------------------------------------------------------------
7.3 --cla_default Option
-------------------------------------------------------------------------------
The --cla_default option can be used to treat .c files as CLA files.


-------------------------------------------------------------------------------
8) RAM Function Support
-------------------------------------------------------------------------------
The ramfunc attribute can be used to specify that a function will be placed in
and executed out of RAM. This allows the compiler to optimize functions for
RAM execution, as well as to automatically copy functions to RAM on flash-based
devices.

The attribute is applied to a function with GCC attribute syntax, as follows:
   __attribute__((ramfunc))
   void f(void) { ... }

The --ramfunc=on option can be used to indicate that the attribute should be
applied to all functions in a translations unit, eliminating the need for 
source code modification.

The --no_fast_branch/-me option is deprecated and no longer has any effect.
Fast branch instructions will be generated for ramfunc functions. Regular
branch instructions will be generated for all other functions. For projects 
or files previously compiled without the --no_fast_branch option, use the
--ramfunc=on option instead to generate fast branch instructions for all
functions.

For more information, see:
http://processors.wiki.ti.com/index.php/Placing_functions_in_RAM


-------------------------------------------------------------------------------
9) New C2000 Intrinsics __eallow, __edis
-------------------------------------------------------------------------------
The following intrinsics are now supported for C2000:
   void __eallow(void);  // generates EALLOW instruction
   void __edis(void);    // generates EDIS instruction

-------------------------------------------------------------------------------
10) Module summary in linker map file
-------------------------------------------------------------------------------
The linker map file now contains a module summary view. This view organizes
the object files by directory or library and displays the code, read-write, and
read-only size that the file contributed to the resulting executable.

Sample output:

MODULE SUMMARY

       Module                     code    ro data   rw data
       ------                     ----    -------   -------
    .\Application\
       file1.obj                  1146    0         920    
       file2.obj                  316     0         0      
    +--+--------------------------+-------+---------+---------+
       Total:                     1462    0         920 

 mylib.lib
       libfile1.obj               500     0         0      
       libfile2.obj               156     4         0      
       libfile3.obj               122     0         20      
    +--+--------------------------+-------+---------+---------+
       Total:                     778     4         20

       Heap:                      0       0         0      
       Stack:                     0       0         1024   
       Linker Generated:          424     200       0      
    +--+--------------------------+-------+---------+---------+
       Grand Total:               2664    204       1964

-------------------------------------------------------------------------------
11) C99 Math RTS Library Support
-------------------------------------------------------------------------------
The RTS Math Library has been changed. C99 math support is now available,
including long double (64-bit) and float versions of floating point math 
routines. 

See the following link for a list of C99 math routines:
http://en.wikibooks.org/wiki/C_Programming/C_Reference/math.h#C99_functions
