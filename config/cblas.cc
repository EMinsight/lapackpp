// Copyright (c) 2017-2020, University of Tennessee. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#include <stdio.h>

#if defined(HAVE_ESSL)
    #include <essl.h>
#elif defined(HAVE_MKL)
    #include <mkl_cblas.h>
#elif defined(HAVE_ACCELERATE)
    // On macOS, the official way to include cblas is via Accelerate.h.
    // Unfortunately with Xcode 10.3 and GNU g++ 9.3, that doesn't compile.
    // If we can find cblas.h, use it, otherwise use Accelerate.h.
    #ifdef HAVE_ACCELERATE_CBLAS_H
        #include <cblas.h>
    #else
        #include <Accelerate/Accelerate.h>
    #endif
#else
    #ifdef __cplusplus
        // Some ancient cblas.h don't include extern C. It's okay to nest.
        extern "C" {
        #include <cblas.h>
        }
    #else
        #include <cblas.h>
    #endif
#endif

int main()
{
    int n = 5;
    double x[] = { 1, 2, 3, 4, 5 };
    double y[] = { 5, 4, 3, 2, 1 };
    double result = cblas_ddot( n, x, 1, y, 1 );
    bool okay = (result == 35);
    printf( "%s\n", okay ? "ok" : "failed" );
    return ! okay;
}
