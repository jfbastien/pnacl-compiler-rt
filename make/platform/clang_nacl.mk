Description := Static runtime libraries for clang/NaCl.

###

CC := clang
Arch := unknown
Configs :=

# We don't currently have any general purpose way to target architectures other
# than the compiler defaults (because there is no generalized way to invoke
# cross compilers). For now, we just find the target archicture of the compiler
# and only define configurations we know that compiler can generate.
CompilerTargetTriple := $(shell \
	$(CC) -v 2>&1 | grep 'Target:' | cut -d' ' -f2)
ifneq ($(DEBUGMAKE),)
ifeq ($(CompilerTargetTriple),)
$(error "unable to infer compiler target triple for $(CC)")
endif
endif

CompilerTargetArch := $(firstword $(subst -, ,$(CompilerTargetTriple)))
$(call CheckValue,CompilerTargetTriple)
# Only define configs if we detected a nacl target.
ifneq ($(findstring -nacl,$(CompilerTargetTriple)),)

# Configurations which just include all the runtime functions.
ifeq ($(call contains,i686,$(CompilerTargetArch)),true)
Configs += full-i386
Arch.full-i386 := i386
else
ifeq ($(call contains,x86_64,$(CompilerTargetArch)),true)
Configs += full-x86_64
Arch.full-x86_64 := x86_64
else
ifeq ($(call contains,arm,$(CompilerTargetArch)),true)
Configs += full-arm
Arch.full-arm := armv7
endif
endif
endif

endif

###

$(call CheckValue,CFLAGS)
CFLAGS := -Wall -Werror -O3 -fomit-frame-pointer $(EXTRA_CFLAGS)
$(call CheckValue,CFLAGS)
# Use the integrated assembler on x86-64 to ensure sandbox base-address hiding.
CFLAGS.full-i386 := $(CFLAGS) -m32 -integrated-as
CFLAGS.full-x86_64 := $(CFLAGS) -m64 -integrated-as
CFLAGS.full-arm := $(CFLAGS)

# The following are common to all platforms and are also included in PNaCl:
IdivFunctions := divdi3 divsi3 udivdi3 udivsi3 divmoddi4 divmodsi4 udivmoddi4 \
                 udivmodsi4 moddi3 modsi3 umoddi3 umodsi3

# These are copies of a few libm functions, to avoid having to link
# with libm for compiler-rt
LibMFunctions := compilerrt_fmaxf compilerrt_fmax \
                 compilerrt_logb compilerrt_logbf
FPComplexFunctions := $(LibMFunctions) divdc3 divsc3 muldc3 mulsc3

FPRoundToZeroFunctions := fixdfdi fixdfsi fixsfdi fixsfsi fixunsdfdi fixunsdfsi \
		          fixunssfdi fixunssfsi

FPRoundToEvenFunctions := floatdidf floatdisf floatsidf floatsisf floatundidf \
			  floatundisf floatunsidf floatunsisf

FPPowFunctions := powisf2 powidf2

PopCountFunctions := popcountsi2 popcountdi2

# The following are used everywhere except x86-64:
OverflowFunctions := mulodi4 mulosi4


Int128Functions := fixdfti fixsfti fixunsdfti fixunssfti \
                   floatuntidf floatuntisf muloti4 \
                   floattidf floattisf udivmodti4 clzti2 ctzti2 \
                   udivti3  umodti3  modti3 multi3 divti3 popcountti2

# TODO(dschuff): maybe add some of the following. They are currently handled w/
# native instructions but non-LLVM codegens might want them.
# Soft-float arith: addsf3 adddf3 addtf3 subsf3 subdf3 subtf3
# Integer bit manipulation: ashldi3 ashlti3 ashrdi3 ashrti3 lshrdi3 lshrti3
#  clzsi2 clzdi2 clzti2 ctzsi2 ctzdi2 ctzti2
#  ffsdi2 ffsti2 paritysi2 paritydi2 parityti2
#  bswapsi2 bswapdi2 (arm only)
# Integer arithmetic: negdi2 negti2 muldi3 multi3
# Integer artith with trapping overflow: absvsi2 absvdi2 absvti2
#  negvsi2 negvdi2 negvti2 mulvsi3 mulvti3 mulvti3
#  addvdi3 addvsi3 addvti3 subvdi3 subvsi3 subvti3
# Integer comparison: cmpdi2 cmpti2 ucmpdi2 ucmpti2


# Functions excluded:
# 80 bit complex float: divxc3 mulxc3
# 80 bit float: floatdixf floatundixf fixunsxfdi fixunsxfsi fixunsxfti
#   fixxfdi fixxfsi fixxfti posixf2 powixf2 powitf2 floattixf floatuntixf
# PPC: powitf2 fixtfdi fixunstfdi floatditf floatunditf divtc3
# gcc_personality_v0
# apple_versioning
# trampoline_setup
# enable_execute_stack
# clear_cache
# eprintf

# ARM helper functions defined by EABI
# Note that there are many other aeabi functions that get defined as aliases to
# functions included in other categories. This list only inlcludes functions
# that have their own separate implementations.
# We leave out the soft-float routines, since the soft-float routines above are
# also left out, and NaCl requires VFP (this will also help ensure we don't
# accidentally start depending on them):
# aeabi_dcmp aeabi_fcmp
AEABIFunctions := aeabi_div0 aeabi_idivmod aeabi_ldivmod \
	          aeabi_memcmp aeabi_memcpy aeabi_memmove aeabi_memset \
		  aeabi_uidivmod aeabi_uldivmod

NaClCommonFunctions := $(IdivFunctions) $(FPComplexFunctions) \
	               $(FPRoundToZeroFunctions) $(FPRoundToEvenFunctions) \
	               $(FPPowFunctions) $(PopCountFunctions)

FUNCTIONS.full-i386 := $(NaClCommonFunctions) $(OverflowFunctions)
FUNCTIONS.full-x86_64 := $(NaClCommonFunctions) $(Int128Functions)
FUNCTIONS.full-arm := $(NaClCommonFunctions) $(OverflowFunctions) $(AEABIFunctions)

# For now, do not use the assembly implementations because they haven't been
# ported to NaCl.
OPTIMIZED := 0

# We don't need to use visibility hidden on Linux.
VISIBILITY_HIDDEN := 0

nacltest-x86-32:
	test/builtins/Unit/nacltest.py $(TCROOT)/bin/i686-nacl-clang \
	 $(FUNCTIONS.full-i386)
nacltest-x86-64:
	test/builtins/Unit/nacltest.py $(TCROOT)/bin/x86_64-nacl-clang \
	 $(FUNCTIONS.full-x86_64)
nacltest-arm:
	test/builtins/Unit/nacltest.py $(TCROOT)/bin/arm-nacl-clang \
	 $(FUNCTIONS.full-arm)
