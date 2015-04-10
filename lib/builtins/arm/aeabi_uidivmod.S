//===-- aeabi_uidivmod.S - EABI uidivmod implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../assembly.h"

// struct { unsigned quot, unsigned rem}
//        __aeabi_uidivmod(unsigned numerator, unsigned denominator) {
//   unsigned rem, quot;
//   quot = __udivmodsi4(numerator, denominator, &rem);
//   return {quot, rem};
// }

        .syntax unified
        .p2align 2
DEFINE_COMPILERRT_FUNCTION(__aeabi_uidivmod)
        push    { lr }
        SFI_SP \
        sub     sp, sp, #4
        mov     r2, sp
        SFI_BL      SYMBOL_NAME(__udivmodsi4)
        ldr     r1, [sp]
        SFI_SP \
        add     sp, sp, #4
        pop     { lr }
        SFI_BX  lr
END_COMPILERRT_FUNCTION(__aeabi_uidivmod)
