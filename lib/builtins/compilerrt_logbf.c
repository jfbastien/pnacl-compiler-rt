/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* From newlib's newlib/libm/common/sf_logb.c */

#include "fdlibm_nacl.h"

float __compilerrt_logbf(float x)
{
        __int32_t hx,ix;

        GET_FLOAT_WORD(hx,x);
        hx &= 0x7fffffff;
        if(FLT_UWORD_IS_ZERO(hx))  {
                float  xx;
                /* arg==0:  return -inf and raise divide-by-zero exception */
                SET_FLOAT_WORD(xx,hx);  /* +0.0 */
                return -1./xx;  /* logbf(0) = -inf */
                }
        if(FLT_UWORD_IS_SUBNORMAL(hx)) {
            for (ix = -126,hx<<=8; hx>0; hx<<=1) ix -=1;
            return (float) ix;
        }
        else if (FLT_UWORD_IS_INFINITE(hx)) return HUGE_VALF;   /* x==+|-inf */
        else if (FLT_UWORD_IS_NAN(hx)) return x;
        else return (float) ((hx>>23)-127);
}
