##############################################################################
#
#
# Makefile for C2000 run-time support library version 15.12.3
#
#
##############################################################################
#
# This Makefile is automatically-generated: you usually should not edit it. 
#
##############################################################################


##############################################################################
#
# This Makefile is specific to compiler RTS library version 15.12.3.  
# The format is subject to change without notice.  This Makefile is intended
# for use with mklib, which understands which 'make' variables to set to build
# each distinct version of the RTS library.
#
# This Makefile uses GNU make language extensions.
#
##############################################################################

##############################################################################
#
# GROUPS
#
# The source files are set up in a modular fashion using "groups."  Groups
# represent major feature clusters, and usually specify source files which
# implement that feature.  When building a library which needs a particular
# feature, the group which represents that feature is "activated" by setting a
# 'make' variable.  This variable can be set directly by the user, or may be
# inferred from the setting of other variables.  Groups (and the files they
# contain) not needed for a certain library are not activated.  Some groups
# are always activated.  Groups which are not always activated are enclosed in
# an "ifeq" testing the corresponding 'make' variable (the name of the
# variable is the name of the group).
#
# Groups can specify any kind of source file (e.g. C, C++, assembly), but do
# not list header files, or source files included from other source files.
# This Makefile is focused on how to build the object files, not listing all
# of the necessary files.  Some groups do not list any source files; these
# groups may still carry information such as extra options to use when that
# group is active, or may activate other groups.  Some groups exist only to be
# tested as part of the activation condition of other groups.
#
# Groups can specify extra compiler options to use when that group is active.
# These can be options intended for all targets (UNIVERSAL_FLAGS), such as the
# CPU version, or can be intended only for a certain type of files (CPP_FLAGS)
# (e.g. the option "--exceptions")
#
##############################################################################

##############################################################################
#
# OPTIONS
#
# You can append or override some options through 'make' variables specified
# on the command line.  Not all options can be changed this way, and some
# options are dangerous to use in a Makefile (e.g. "--skip_assembler").
# Options added by groups below should not be altered; if the option is
# undesired, you're probably building the wrong library.  For example, if you
# don't want "--exceptions", you shouldn't be using EXCEPTIONS=1.
#
# You can append options to the standard list by using EXTRA_FLAGS.  This is
# useful for enabling debugging support or activating silicon errata
# workarounds:
#
# gmake LIB=some.lib EXTRA_FLAGS="-g"
#
# You can completely replace the standard list by using STANDARD_FLAGS, but
# this should almost never be necessary.  WARNING: This Makefile will not
# optimize the files if you replace the standard list and your replacement
# options do not include the right optimization flags.
#
# gmake LIB=some.lib STANDARD_FLAGS="-o3 -oe"
#
##############################################################################

##############################################################################
# Available libraries
#
# You can build each 15.12.3 library using the appropriate command:
#
# gmake library LIB=rts2800_fpu32.lib FPU32=1
# gmake library LIB=rts2800_fpu32_eh.lib FULL_PORTABLE_EH=1 EXCEPTIONS=1 FPU32=1
# gmake library LIB=rts2800_ml.lib LARGE=1
# gmake library LIB=rts2800_ml_eh.lib FULL_PORTABLE_EH=1 LARGE=1 EXCEPTIONS=1
##############################################################################

ifdef CMD_FILE
 -include $(CMD_FILE)
endif
#-----------------------------------------------------------------------------
# Basic definitions
#-----------------------------------------------------------------------------
STANDARD_FLAGS =  -O4 --diag_warning=225 --quiet
ifeq (y,$(shell [ -e $(CGT_BIN)/cl2000 ] && echo y))
  CC = $(CGT_BIN)/cl2000
else
  CC = cl2000
endif
ifeq (y,$(shell [ -e $(CGT_BIN)/ar2000 ] && echo y))
  AR = $(CGT_BIN)/ar2000
else
  AR = ar2000
endif
CD = cd

#-----------------------------------------------------------------------------
# Group TARGET
#-----------------------------------------------------------------------------
TARGET_CLA_FLAGS += --advice:performance=none
UNIVERSAL_FLAGS += --cla_support=cla0

TARGET_CLA_SOURCE = \
	cla_divi.cla \
	cla_divu.cla \
	cla_remi.cla \
	cla_remu.cla

TARGET_C_SOURCE = \
	cpy_tbl.c \
	memcpy.c \
	abs.c \
	strspn.c \
	pre_init.c \
	startup.c \
	pprof_cio.c \
	pprof_user.c \
	outprof.c

C_SOURCE += $(TARGET_C_SOURCE)


TARGET_ASM_SOURCE = \
	IQ.asm \
	IQxmpy.asm \
	abstract.asm \
	boot.asm \
	epilog.asm \
	fd_add.asm \
	fd_cmp.asm \
	fd_div.asm \
	fd_mpy.asm \
	fd_neg.asm \
	fd_sub.asm \
	fd_tofs.asm \
	fd_toi.asm \
	fd_tol.asm \
	fd_toll.asm \
	fd_tou.asm \
	fd_toul.asm \
	fd_toull.asm \
	fs_add.asm \
	fs_cmp.asm \
	fs_div.asm \
	fs_mpy.asm \
	fs_neg.asm \
	fs_tofd.asm \
	fs_toi.asm \
	fs_tol.asm \
	fs_toll.asm \
	fs_tou.asm \
	fs_toul.asm \
	fs_toull.asm \
	i_div.asm \
	i_tofd.asm \
	i_tofs.asm \
	l_div.asm \
	l_tofd.asm \
	l_tofs.asm \
	ll_abs.asm \
	ll_aox.asm \
	ll_cmp.asm \
	ll_div.asm \
	ll_mpy.asm \
	ll_tofd.asm \
	ll_tofs.asm \
	prolog.asm \
	setjmp.asm \
	u_div.asm \
	u_tofd.asm \
	u_tofs.asm \
	ul_tofd.asm \
	ul_tofs.asm \
	ull_tofd.asm \
	ull_tofs.asm \
	cpy_utils.asm \
	l_lsl.asm \
	l_lsr.asm \
	l_mpy.asm \
	ul_lsr.asm

ASM_SOURCE += $(TARGET_ASM_SOURCE)


#-----------------------------------------------------------------------------
# Group TSPA_CPP
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group FREEBSD_MATH
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group EXCEPTIONS
#-----------------------------------------------------------------------------
ifeq ($(EXCEPTIONS),1)
CPP_FLAGS += --exceptions
endif


#-----------------------------------------------------------------------------
# Group FREEBSD_C99_COMPLEX
#-----------------------------------------------------------------------------
FREEBSD_C99_COMPLEX_C_FLAGS += --c99

#-----------------------------------------------------------------------------
# Group FREEBSD_MATH_COMMON
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group GENERIC
#-----------------------------------------------------------------------------
UNIVERSAL_FLAGS += -c --ti_lib --keep_unneeded_types=false
CPP_FLAGS += -fg --rtti

#-----------------------------------------------------------------------------
# Group STLPORT_CPP
#-----------------------------------------------------------------------------

STLPORT_CPP_CPP_SOURCE = \
	allocators.cpp \
	bitset.cpp \
	complex.cpp \
	complex_io.cpp \
	complex_trig.cpp \
	_ctype.cpp \
	dll_main.cpp \
	istream.cpp \
	ostream.cpp \
	sstream.cpp \
	string.cpp

CPP_SOURCE += $(STLPORT_CPP_CPP_SOURCE)


#-----------------------------------------------------------------------------
# Group EDG_C99_COMPLEX_HELPER
#-----------------------------------------------------------------------------
EDG_C99_COMPLEX_HELPER_CPP_FLAGS += --building_runtime
EDG_C99_COMPLEX_HELPER_C_FLAGS += --building_runtime

EDG_C99_COMPLEX_HELPER_C_SOURCE = \
	c99_complex.c

#-----------------------------------------------------------------------------
# Group C2800
#-----------------------------------------------------------------------------
UNIVERSAL_FLAGS += -v28

#-----------------------------------------------------------------------------
# Group FREEBSD_C99_COMPLEX_32_BIT
#-----------------------------------------------------------------------------
FREEBSD_C99_COMPLEX_32_BIT_C_FLAGS += --c99

FREEBSD_C99_COMPLEX_32_BIT_C_SOURCE = \
	catrigf.c \
	k_expf.c \
	s_carg.c \
	s_cargf.c \
	s_cargl.c \
	s_ccoshf.c \
	s_cexpf.c \
	s_cimag.c \
	s_cimagf.c \
	s_cimagl.c \
	s_conj.c \
	s_conjf.c \
	s_conjl.c \
	s_cproj.c \
	s_cprojf.c \
	s_cprojl.c \
	s_creal.c \
	s_crealf.c \
	s_creall.c \
	s_csinhf.c \
	s_csqrtf.c \
	s_ctanhf.c \
	w_cabs.c \
	w_cabsf.c \
	w_cabsl.c \
	s_log1pf.c \
	e_hypotf.c \
	s_copysignf.c

#-----------------------------------------------------------------------------
# Group GENERIC_TI
#-----------------------------------------------------------------------------

GENERIC_TI_C_SOURCE = \
	ecvt.c \
	exit.c \
	fcvt.c \
	memory.c \
	_bufread.c \
	_data_synch.c \
	_io_perm.c \
	_lock.c \
	_mutex.c \
	_printfi.c \
	_printfi_min.c \
	_printfi_nf.c \
	_pthread.c \
	_scanfi.c \
	_scanfi_min.c \
	_scanfi_nf.c \
	_tvaltostr.c \
	args_main.c \
	asctime.c \
	assert.c \
	atexit.c \
	atof.c \
	atoi.c \
	atol.c \
	atoll.c \
	bcmp.c \
	bcopy.c \
	bsearch.c \
	bzero.c \
	clearerr.c \
	clock.c \
	ctime_.c \
	ctime64.c \
	ctype.c \
	defs.c \
	difftime.c \
	difftime64.c \
	fclose.c \
	feof.c \
	ferror.c \
	fflush.c \
	ffs.c \
	fgetc.c \
	fgetpos.c \
	fgets.c \
	fopen.c \
	fprintf.c \
	fputc.c \
	fputs.c \
	fread.c \
	fscanf.c \
	fseek.c \
	fsetpos.c \
	ftell.c \
	fwrite.c \
	getenv.c \
	gmtime.c \
	gmtime64.c \
	index.c \
	isalnum.c \
	isalpha.c \
	isascii.c \
	iscntrl.c \
	isdigit.c \
	isgraph.c \
	islower.c \
	isprint.c \
	ispunct.c \
	isspace.c \
	isupper.c \
	isxdigit.c \
	localtim.c \
	localtim64.c \
	lowlev.c \
	ltoa.c \
	memccpy.c \
	memchr.c \
	memcmp.c \
	memmov.c \
	memset.c \
	mktime.c \
	mktime64.c \
	multibyte.c \
	perror.c \
	printf.c \
	qsort.c \
	rand.c \
	remove.c \
	rewind.c \
	rindex.c \
	setbuf.c \
	setlocale.c \
	setvbuf.c \
	sinit.c \
	snprintf.c \
	sprintf.c \
	sscanf.c \
	strcasecmp.c \
	strcat.c \
	strchr.c \
	strcmp.c \
	strcoll.c \
	strcpy.c \
	strcspn.c \
	strdup.c \
	strerror.c \
	strftime.c \
	strlen.c \
	strncasecmp.c \
	strncat.c \
	strncmp.c \
	strncpy.c \
	strpbrk.c \
	strrchr.c \
	strstr.c \
	strtod.c \
	strtof.c \
	strtok.c \
	strtol.c \
	strtold.c \
	strtoll.c \
	strtoul.c \
	strtoull.c \
	strtoimax.c \
	strtoumax.c \
	strxfrm.c \
	time.c \
	time64.c \
	tmpfile.c \
	tmpnam.c \
	tmzone.c \
	toascii.c \
	tolower.c \
	toupper.c \
	trgdrv.c \
	trgmsg.c \
	ungetc.c \
	vfprintf.c \
	vprintf.c \
	vsnprintf.c \
	vsprintf.c \
	wctype.c

C_SOURCE += $(GENERIC_TI_C_SOURCE)


#-----------------------------------------------------------------------------
# Group GENERIC_MATH_LIBRARY_INTEGER
#-----------------------------------------------------------------------------

GENERIC_MATH_LIBRARY_INTEGER_C_SOURCE = \
	div.c \
	ldiv.c \
	lldiv.c

C_SOURCE += $(GENERIC_MATH_LIBRARY_INTEGER_C_SOURCE)


#-----------------------------------------------------------------------------
# Group FREEBSD_MATH_64_BIT
#-----------------------------------------------------------------------------
FREEBSD_MATH_64_BIT_C_FLAGS += --c99

FREEBSD_MATH_64_BIT_C_SOURCE = \
	b_exp.c \
	b_log.c \
	b_tgamma.c \
	e_acos.c \
	e_acosh.c \
	e_asin.c \
	e_atan2.c \
	e_atanh.c \
	e_cosh.c \
	e_exp.c \
	e_fmod.c \
	e_lgamma.c \
	e_lgamma_r.c \
	e_log.c \
	e_log10.c \
	e_log2.c \
	e_pow.c \
	e_rem_pio2.c \
	k_rem_pio2.c \
	e_remainder.c \
	e_sinh.c \
	e_sqrt.c \
	k_cos.c \
	k_sin.c \
	k_tan.c \
	s_asinh.c \
	s_atan.c \
	s_cbrt.c \
	s_ceil.c \
	s_cos.c \
	s_erf.c \
	s_exp2.c \
	s_expm1.c \
	s_fabs.c \
	s_floor.c \
	s_fma.c \
	s_frexp.c \
	s_ilogb.c \
	s_logb.c \
	s_modf.c \
	s_nextafter.c \
	s_remquo.c \
	s_rint.c \
	s_round.c \
	s_scalbn.c \
	s_sin.c \
	s_tan.c \
	s_tanh.c \
	s_trunc.c

#-----------------------------------------------------------------------------
# Group EDG_CPP_SUPPLEMENTAL
#-----------------------------------------------------------------------------
EDG_CPP_SUPPLEMENTAL_CPP_FLAGS += --building_runtime -DUSE_EDG_EXCEPTION_CLASSES=1

EDG_CPP_SUPPLEMENTAL_CPP_SOURCE = \
	exception_.cpp \
	bad_alloc.cpp

#-----------------------------------------------------------------------------
# Group FREEBSD_MATH_TI_SUPPLEMENT
#-----------------------------------------------------------------------------
FREEBSD_MATH_TI_SUPPLEMENT_C_FLAGS += --c99

FREEBSD_MATH_TI_SUPPLEMENT_C_SOURCE = \
	ti_fenv.c \
	ti_fmax.c \
	ti_nan.c

#-----------------------------------------------------------------------------
# Group FREEBSD_SUPPLEMENTAL_C
#-----------------------------------------------------------------------------

FREEBSD_SUPPLEMENTAL_C_C_SOURCE = \
	wcslen.c \
	wmemchr.c \
	wmemcmp.c \
	wmemcpy.c \
	wmemmove.c \
	wmemset.c \
	wcscmp.c \
	wcsncmp.c \
	wcsncpy.c \
	ascii.c

C_SOURCE += $(FREEBSD_SUPPLEMENTAL_C_C_SOURCE)


#-----------------------------------------------------------------------------
# Group FULL_PORTABLE_EH
#-----------------------------------------------------------------------------
ifeq ($(EXCEPTIONS),1)
FULL_PORTABLE_EH=1
endif

ifeq ($(FULL_PORTABLE_EH),1)
FULL_PORTABLE_EH_C_FLAGS += --building_runtime
FULL_PORTABLE_EH_CPP_FLAGS += --building_runtime

FULL_PORTABLE_EH_CPP_SOURCE = \
	eh_util.cpp \
	throw_edg.cpp
endif


#-----------------------------------------------------------------------------
# Group TSPA_COMPLEX_HEADERS
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group CLASSIC_CPP_MATH_OVERLOADS
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group CLASSIC_ERRNO
#-----------------------------------------------------------------------------

CLASSIC_ERRNO_C_SOURCE = \
	errno.c

C_SOURCE += $(CLASSIC_ERRNO_C_SOURCE)


#-----------------------------------------------------------------------------
# Group FPU32
#-----------------------------------------------------------------------------
ifeq ($(FPU32),1)
UNIVERSAL_FLAGS += --float_support=fpu32
endif


#-----------------------------------------------------------------------------
# Group FREEBSD_C99_COMPLEX_64_BIT
#-----------------------------------------------------------------------------
FREEBSD_C99_COMPLEX_64_BIT_C_FLAGS += --c99

FREEBSD_C99_COMPLEX_64_BIT_C_SOURCE = \
	catrig.c \
	k_exp.c \
	s_ccosh.c \
	s_cexp.c \
	s_csinh.c \
	s_csqrt.c \
	s_ctanh.c \
	s_log1p.c \
	e_hypot.c \
	s_copysign.c

#-----------------------------------------------------------------------------
# Group STLPORT_LOCALE
#-----------------------------------------------------------------------------
STLPORT_LOCALE_CPP_FLAGS += -O3 --inline_recursion_limit=12

STLPORT_LOCALE_CPP_SOURCE = \
	unified_locale.cpp

#-----------------------------------------------------------------------------
# Group LARGE
#-----------------------------------------------------------------------------
ifeq ($(LARGE),1)
UNIVERSAL_FLAGS += -ml
endif


#-----------------------------------------------------------------------------
# Group FREEBSD_FENV
#-----------------------------------------------------------------------------

FREEBSD_FENV_C_SOURCE = \
	fenv.c

C_SOURCE += $(FREEBSD_FENV_C_SOURCE)


#-----------------------------------------------------------------------------
# Group TI_SUPPLEMENTAL_C
#-----------------------------------------------------------------------------

TI_SUPPLEMENTAL_C_C_SOURCE = \
	signal.c \
	wchar.c

C_SOURCE += $(TI_SUPPLEMENTAL_C_C_SOURCE)


#-----------------------------------------------------------------------------
# Group OPENBSD_C99_COMPLEX
#-----------------------------------------------------------------------------
OPENBSD_C99_COMPLEX_C_FLAGS += --c99

OPENBSD_C99_COMPLEX_C_SOURCE = \
	s_clog.c \
	s_clogf.c \
	s_clogl.c \
	s_cpow.c \
	s_cpowf.c \
	s_cpowl.c

#-----------------------------------------------------------------------------
# Group FREEBSD_MATH_32_BIT
#-----------------------------------------------------------------------------
FREEBSD_MATH_32_BIT_C_FLAGS += --c99

FREEBSD_MATH_32_BIT_C_SOURCE = \
	e_acosf.c \
	e_acoshf.c \
	e_asinf.c \
	e_atan2f.c \
	e_atanhf.c \
	e_coshf.c \
	e_expf.c \
	e_fmodf.c \
	e_lgammaf.c \
	e_lgammaf_r.c \
	e_log10f.c \
	e_log2f.c \
	e_logf.c \
	e_powf.c \
	e_remainderf.c \
	e_sinhf.c \
	e_sqrtf.c \
	k_cosf.c \
	k_sinf.c \
	k_tanf.c \
	s_asinhf.c \
	s_atanf.c \
	s_cbrtf.c \
	s_ceilf.c \
	s_cosf.c \
	s_erff.c \
	s_exp2f.c \
	s_expm1f.c \
	s_fabsf.c \
	s_fdim.c \
	s_floorf.c \
	s_fmaf.c \
	s_frexpf.c \
	s_ilogbf.c \
	s_logbf.c \
	s_llrint.c \
	s_llrintf.c \
	s_llrintl.c \
	s_llround.c \
	s_llroundf.c \
	s_llroundl.c \
	s_lrint.c \
	s_lrintf.c \
	s_lrintl.c \
	s_lround.c \
	s_lroundf.c \
	s_lroundl.c \
	s_modff.c \
	s_nearbyint.c \
	s_nextafterf.c \
	s_nexttowardf.c \
	s_remquof.c \
	s_rintf.c \
	s_roundf.c \
	s_scalbln.c \
	s_scalbnf.c \
	s_sinf.c \
	s_tanf.c \
	s_tanhf.c \
	s_tgammaf.c \
	s_truncf.c

#-----------------------------------------------------------------------------
# Group GENERIC_EDG
#-----------------------------------------------------------------------------
GENERIC_EDG_C_FLAGS += --building_runtime
GENERIC_EDG_CPP_FLAGS += --building_runtime

GENERIC_EDG_CPP_SOURCE = \
	array_del.cpp \
	array_new.cpp \
	array_nodel.cpp \
	array_nonew.cpp \
	array_pdel.cpp \
	array_pnew.cpp \
	delete.cpp \
	delnothrow.cpp \
	dtor_list.cpp \
	error.cpp \
	guard.cpp \
	memzero.cpp \
	new_.cpp \
	newhandler.cpp \
	newnothrow.cpp \
	placedel.cpp \
	placenew.cpp \
	pure_virt.cpp \
	rtti.cpp \
	set_new.cpp \
	typeinfo_.cpp \
	vars.cpp \
	vec_cctor.cpp \
	vec_newdel.cpp \
	vla_alloc.cpp

#-----------------------------------------------------------------------------
# Group FREEBSD_C99_COMPLEX_COMMON
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Group CLASSIC_MATH_LIBRARY_INTEGER
#-----------------------------------------------------------------------------

CLASSIC_MATH_LIBRARY_INTEGER_C_SOURCE = \
	labs.c \
	llabs.c \
	imaxabs.c \
	imaxdiv.c

C_SOURCE += $(CLASSIC_MATH_LIBRARY_INTEGER_C_SOURCE)



GENERIC_EDG_CPP_OBJS_1 = $(patsubst %.cpp,$(OBJ)/%.obj,$(notdir $(GENERIC_EDG_CPP_SOURCE)))
$(GENERIC_EDG_CPP_OBJS_1): $(OBJ)/%.obj: %.cpp
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(GENERIC_EDG_FLAGS) $(CPP_FLAGS) $(GENERIC_EDG_CPP_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

TARGET_CLA_OBJS_2 = $(patsubst %.cla,$(OBJ)/%.obj,$(notdir $(TARGET_CLA_SOURCE)))
$(TARGET_CLA_OBJS_2): $(OBJ)/%.obj: %.cla
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(TARGET_FLAGS) $(C_FLAGS) $(TARGET_CLA_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FREEBSD_MATH_32_BIT_C_OBJS_3 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(FREEBSD_MATH_32_BIT_C_SOURCE)))
$(FREEBSD_MATH_32_BIT_C_OBJS_3): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FREEBSD_MATH_32_BIT_FLAGS) $(C_FLAGS) $(FREEBSD_MATH_32_BIT_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

OPENBSD_C99_COMPLEX_C_OBJS_4 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(OPENBSD_C99_COMPLEX_C_SOURCE)))
$(OPENBSD_C99_COMPLEX_C_OBJS_4): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(OPENBSD_C99_COMPLEX_FLAGS) $(C_FLAGS) $(OPENBSD_C99_COMPLEX_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

STLPORT_LOCALE_CPP_OBJS_5 = $(patsubst %.cpp,$(OBJ)/%.obj,$(notdir $(STLPORT_LOCALE_CPP_SOURCE)))
$(STLPORT_LOCALE_CPP_OBJS_5): $(OBJ)/%.obj: %.cpp
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(STLPORT_LOCALE_FLAGS) $(CPP_FLAGS) $(STLPORT_LOCALE_CPP_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FREEBSD_C99_COMPLEX_64_BIT_C_OBJS_6 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(FREEBSD_C99_COMPLEX_64_BIT_C_SOURCE)))
$(FREEBSD_C99_COMPLEX_64_BIT_C_OBJS_6): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FREEBSD_C99_COMPLEX_64_BIT_FLAGS) $(C_FLAGS) $(FREEBSD_C99_COMPLEX_64_BIT_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FULL_PORTABLE_EH_CPP_OBJS_7 = $(patsubst %.cpp,$(OBJ)/%.obj,$(notdir $(FULL_PORTABLE_EH_CPP_SOURCE)))
$(FULL_PORTABLE_EH_CPP_OBJS_7): $(OBJ)/%.obj: %.cpp
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FULL_PORTABLE_EH_FLAGS) $(CPP_FLAGS) $(FULL_PORTABLE_EH_CPP_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FREEBSD_MATH_TI_SUPPLEMENT_C_OBJS_8 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(FREEBSD_MATH_TI_SUPPLEMENT_C_SOURCE)))
$(FREEBSD_MATH_TI_SUPPLEMENT_C_OBJS_8): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FREEBSD_MATH_TI_SUPPLEMENT_FLAGS) $(C_FLAGS) $(FREEBSD_MATH_TI_SUPPLEMENT_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

EDG_CPP_SUPPLEMENTAL_CPP_OBJS_9 = $(patsubst %.cpp,$(OBJ)/%.obj,$(notdir $(EDG_CPP_SUPPLEMENTAL_CPP_SOURCE)))
$(EDG_CPP_SUPPLEMENTAL_CPP_OBJS_9): $(OBJ)/%.obj: %.cpp
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(EDG_CPP_SUPPLEMENTAL_FLAGS) $(CPP_FLAGS) $(EDG_CPP_SUPPLEMENTAL_CPP_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FREEBSD_MATH_64_BIT_C_OBJS_10 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(FREEBSD_MATH_64_BIT_C_SOURCE)))
$(FREEBSD_MATH_64_BIT_C_OBJS_10): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FREEBSD_MATH_64_BIT_FLAGS) $(C_FLAGS) $(FREEBSD_MATH_64_BIT_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

FREEBSD_C99_COMPLEX_32_BIT_C_OBJS_11 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(FREEBSD_C99_COMPLEX_32_BIT_C_SOURCE)))
$(FREEBSD_C99_COMPLEX_32_BIT_C_OBJS_11): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(FREEBSD_C99_COMPLEX_32_BIT_FLAGS) $(C_FLAGS) $(FREEBSD_C99_COMPLEX_32_BIT_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

EDG_C99_COMPLEX_HELPER_C_OBJS_12 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(EDG_C99_COMPLEX_HELPER_C_SOURCE)))
$(EDG_C99_COMPLEX_HELPER_C_OBJS_12): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(EDG_C99_COMPLEX_HELPER_FLAGS) $(C_FLAGS) $(EDG_C99_COMPLEX_HELPER_C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

CPP_OBJS_13 = $(patsubst %.cpp,$(OBJ)/%.obj,$(notdir $(CPP_SOURCE)))
$(CPP_OBJS_13): $(OBJ)/%.obj: %.cpp
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(UNIVERSAL_FLAGS) $(CPP_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

ASM_OBJS_14 = $(patsubst %.asm,$(OBJ)/%.obj,$(notdir $(ASM_SOURCE)))
$(ASM_OBJS_14): $(OBJ)/%.obj: %.asm
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(UNIVERSAL_FLAGS) $(C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ)
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(UNIVERSAL_FLAGS) $(C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) --asm_dependency=$(OBJ)/$(basename $(@F)).pp
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

C_OBJS_15 = $(patsubst %.c,$(OBJ)/%.obj,$(notdir $(C_SOURCE)))
$(C_OBJS_15): $(OBJ)/%.obj: %.c
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(UNIVERSAL_FLAGS) $(C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp

CLA_OBJS_16 = $(patsubst %.cla,$(OBJ)/%.obj,$(notdir $(CLA_SOURCE)))
$(CLA_OBJS_16): $(OBJ)/%.obj: %.cla
	$(CC) $< $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS) $(UNIVERSAL_FLAGS) $(UNIVERSAL_FLAGS) $(C_FLAGS) -I$(INC) -fr $(OBJ) -fs $(OBJ) -ft $(OBJ) -ppa -ppd --pp_directory=$(OBJ)
	@$(CP_CMD) $(OBJ)/$(basename $(@F)).pp $(OBJ)/$(basename $(@F)).P
	@$(SED_CMD) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ)/$(basename $(@F)).pp >> $(OBJ)/$(basename $(@F)).P
	@$(RM_CMD) -f $(OBJ)/$(basename $(@F)).pp
ALL_OBJS = $(GENERIC_EDG_CPP_OBJS_1) $(TARGET_CLA_OBJS_2) $(FREEBSD_MATH_32_BIT_C_OBJS_3) $(OPENBSD_C99_COMPLEX_C_OBJS_4) $(STLPORT_LOCALE_CPP_OBJS_5) $(FREEBSD_C99_COMPLEX_64_BIT_C_OBJS_6) $(FULL_PORTABLE_EH_CPP_OBJS_7) $(FREEBSD_MATH_TI_SUPPLEMENT_C_OBJS_8) $(EDG_CPP_SUPPLEMENTAL_CPP_OBJS_9) $(FREEBSD_MATH_64_BIT_C_OBJS_10) $(FREEBSD_C99_COMPLEX_32_BIT_C_OBJS_11) $(EDG_C99_COMPLEX_HELPER_C_OBJS_12) $(CPP_OBJS_13) $(ASM_OBJS_14) $(C_OBJS_15) $(CLA_OBJS_16)
-include $(ALL_OBJS:.obj=.P)

library: $(ALL_OBJS)
	$(CD) $(OBJ); $(AR) -qa $(LIB) *.obj

info:
	@echo GENERIC_EDG_CPP_OBJS_1 $(GENERIC_EDG_CPP_OBJS_1)
	@echo TARGET_CLA_OBJS_2 $(TARGET_CLA_OBJS_2)
	@echo FREEBSD_MATH_32_BIT_C_OBJS_3 $(FREEBSD_MATH_32_BIT_C_OBJS_3)
	@echo OPENBSD_C99_COMPLEX_C_OBJS_4 $(OPENBSD_C99_COMPLEX_C_OBJS_4)
	@echo STLPORT_LOCALE_CPP_OBJS_5 $(STLPORT_LOCALE_CPP_OBJS_5)
	@echo FREEBSD_C99_COMPLEX_64_BIT_C_OBJS_6 $(FREEBSD_C99_COMPLEX_64_BIT_C_OBJS_6)
	@echo FULL_PORTABLE_EH_CPP_OBJS_7 $(FULL_PORTABLE_EH_CPP_OBJS_7)
	@echo FREEBSD_MATH_TI_SUPPLEMENT_C_OBJS_8 $(FREEBSD_MATH_TI_SUPPLEMENT_C_OBJS_8)
	@echo EDG_CPP_SUPPLEMENTAL_CPP_OBJS_9 $(EDG_CPP_SUPPLEMENTAL_CPP_OBJS_9)
	@echo FREEBSD_MATH_64_BIT_C_OBJS_10 $(FREEBSD_MATH_64_BIT_C_OBJS_10)
	@echo FREEBSD_C99_COMPLEX_32_BIT_C_OBJS_11 $(FREEBSD_C99_COMPLEX_32_BIT_C_OBJS_11)
	@echo EDG_C99_COMPLEX_HELPER_C_OBJS_12 $(EDG_C99_COMPLEX_HELPER_C_OBJS_12)
	@echo CPP_OBJS_13 $(CPP_OBJS_13)
	@echo ASM_OBJS_14 $(ASM_OBJS_14)
	@echo C_OBJS_15 $(C_OBJS_15)
	@echo CLA_OBJS_16 $(CLA_OBJS_16)
	@echo CPP_FLAGS $(CPP_FLAGS)
	@echo C_FLAGS $(C_FLAGS)
	@echo EDG_C99_COMPLEX_HELPER_C_FLAGS $(EDG_C99_COMPLEX_HELPER_C_FLAGS)
	@echo EDG_C99_COMPLEX_HELPER_FLAGS $(EDG_C99_COMPLEX_HELPER_FLAGS)
	@echo EDG_CPP_SUPPLEMENTAL_CPP_FLAGS $(EDG_CPP_SUPPLEMENTAL_CPP_FLAGS)
	@echo EDG_CPP_SUPPLEMENTAL_FLAGS $(EDG_CPP_SUPPLEMENTAL_FLAGS)
	@echo EXTRA_FLAGS $(EXTRA_FLAGS)
	@echo FREEBSD_C99_COMPLEX_32_BIT_C_FLAGS $(FREEBSD_C99_COMPLEX_32_BIT_C_FLAGS)
	@echo FREEBSD_C99_COMPLEX_32_BIT_FLAGS $(FREEBSD_C99_COMPLEX_32_BIT_FLAGS)
	@echo FREEBSD_C99_COMPLEX_64_BIT_C_FLAGS $(FREEBSD_C99_COMPLEX_64_BIT_C_FLAGS)
	@echo FREEBSD_C99_COMPLEX_64_BIT_FLAGS $(FREEBSD_C99_COMPLEX_64_BIT_FLAGS)
	@echo FREEBSD_MATH_32_BIT_C_FLAGS $(FREEBSD_MATH_32_BIT_C_FLAGS)
	@echo FREEBSD_MATH_32_BIT_FLAGS $(FREEBSD_MATH_32_BIT_FLAGS)
	@echo FREEBSD_MATH_64_BIT_C_FLAGS $(FREEBSD_MATH_64_BIT_C_FLAGS)
	@echo FREEBSD_MATH_64_BIT_FLAGS $(FREEBSD_MATH_64_BIT_FLAGS)
	@echo FREEBSD_MATH_TI_SUPPLEMENT_C_FLAGS $(FREEBSD_MATH_TI_SUPPLEMENT_C_FLAGS)
	@echo FREEBSD_MATH_TI_SUPPLEMENT_FLAGS $(FREEBSD_MATH_TI_SUPPLEMENT_FLAGS)
	@echo FULL_PORTABLE_EH_CPP_FLAGS $(FULL_PORTABLE_EH_CPP_FLAGS)
	@echo FULL_PORTABLE_EH_FLAGS $(FULL_PORTABLE_EH_FLAGS)
	@echo GENERIC_EDG_CPP_FLAGS $(GENERIC_EDG_CPP_FLAGS)
	@echo GENERIC_EDG_FLAGS $(GENERIC_EDG_FLAGS)
	@echo OPENBSD_C99_COMPLEX_C_FLAGS $(OPENBSD_C99_COMPLEX_C_FLAGS)
	@echo OPENBSD_C99_COMPLEX_FLAGS $(OPENBSD_C99_COMPLEX_FLAGS)
	@echo STANDARD_FLAGS $(STANDARD_FLAGS)
	@echo STLPORT_LOCALE_CPP_FLAGS $(STLPORT_LOCALE_CPP_FLAGS)
	@echo STLPORT_LOCALE_FLAGS $(STLPORT_LOCALE_FLAGS)
	@echo TARGET_CLA_FLAGS $(TARGET_CLA_FLAGS)
	@echo TARGET_FLAGS $(TARGET_FLAGS)
	@echo UNIVERSAL_FLAGS $(UNIVERSAL_FLAGS)
