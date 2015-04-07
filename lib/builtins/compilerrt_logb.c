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

/* From newlib's newlib/libm/common/s_logb.c */

#include "fdlibm_nacl.h"

double __compilerrt_logb(double x)
{
        __int32_t hx,lx,ix;

        EXTRACT_WORDS(hx,lx,x);
        hx &= 0x7fffffff;               /* high |x| */
        if(hx<0x00100000) {             /* 0 or subnormal */
            if((hx|lx)==0)  {
                double  xx;
                /* arg==0:  return -inf and raise divide-by-zero exception */
                INSERT_WORDS(xx,hx,lx); /* +0.0 */
                return -1./xx;  /* logb(0) = -inf */
                }
            else                        /* subnormal x */
                if(hx==0) {
                    for (ix = -1043; lx>0; lx<<=1) ix -=1;
                } else {
                    for (ix = -1022,hx<<=11; hx>0; hx<<=1) ix -=1;
                }
            return (double) ix;
        }
        else if (hx<0x7ff00000) return (hx>>20)-1023;   /* normal # */
        else if (hx>0x7ff00000 || lx)  return x;        /* x==NaN */
        else  return HUGE_VAL;  /* x==inf (+ or -) */
}
