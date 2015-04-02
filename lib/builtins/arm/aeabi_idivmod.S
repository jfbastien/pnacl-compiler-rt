//===-- aeabi_idivmod.S - EABI idivmod implementation ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../assembly.h"

// struct { int quot, int rem} __aeabi_idivmod(int numerator, int denominator) {
//   int rem, quot;
//   quot = __divmodsi4(numerator, denominator, &rem);
//   return {quot, rem};
// }

        .syntax unified
        .p2align 2
DEFINE_COMPILERRT_FUNCTION(__aeabi_idivmod)
        push    { lr }
        SFI_SP \
        sub     sp, sp, #4
        mov     r2, sp
        SFI_BL  SYMBOL_NAME(__divmodsi4)
        ldr     r1, [sp]
        SFI_SP \
        add     sp, sp, #4
        pop     { lr }
        SFI_BX  lr
END_COMPILERRT_FUNCTION(__aeabi_idivmod)
