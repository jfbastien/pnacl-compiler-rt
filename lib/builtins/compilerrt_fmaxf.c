/*
 * Copyright (c) 2015 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "int_math.h"

float __compilerrt_fmaxf(float x, float y)
{
  if (crt_isnan(x))
    return y;
  if (crt_isnan(y))
    return x;
  return x > y ? x : y;
}
