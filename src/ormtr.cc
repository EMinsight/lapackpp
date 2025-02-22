// Copyright (c) 2017-2023, University of Tennessee. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#include "lapack.hh"
#include "lapack_internal.hh"
#include "lapack/fortran.h"
#include "NoConstructAllocator.hh"

#include <vector>

namespace lapack {

using blas::max;
using blas::min;
using blas::real;

// -----------------------------------------------------------------------------
/// @ingroup heev_computational
int64_t ormtr(
    lapack::Side side, lapack::Uplo uplo, lapack::Op trans, int64_t m, int64_t n,
    float const* A, int64_t lda,
    float const* tau,
    float* C, int64_t ldc )
{
    // for real, map ConjTrans to Trans
    if (trans == Op::ConjTrans)
        trans = Op::Trans;

    char side_ = to_char( side );
    char uplo_ = to_char( uplo );
    char trans_ = to_char( trans );
    lapack_int m_ = to_lapack_int( m );
    lapack_int n_ = to_lapack_int( n );
    lapack_int lda_ = to_lapack_int( lda );
    lapack_int ldc_ = to_lapack_int( ldc );
    lapack_int info_ = 0;

    // query for workspace size
    float qry_work[1];
    lapack_int ineg_one = -1;
    LAPACK_sormtr(
        &side_, &uplo_, &trans_, &m_, &n_,
        A, &lda_,
        tau,
        C, &ldc_,
        qry_work, &ineg_one, &info_
    );
    if (info_ < 0) {
        throw Error();
    }
    lapack_int lwork_ = real(qry_work[0]);

    // allocate workspace
    lapack::vector< float > work( lwork_ );

    LAPACK_sormtr(
        &side_, &uplo_, &trans_, &m_, &n_,
        A, &lda_,
        tau,
        C, &ldc_,
        &work[0], &lwork_, &info_
    );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

// -----------------------------------------------------------------------------
/// @see lapack::unmtr
/// @ingroup heev_computational
int64_t ormtr(
    lapack::Side side, lapack::Uplo uplo, lapack::Op trans, int64_t m, int64_t n,
    double const* A, int64_t lda,
    double const* tau,
    double* C, int64_t ldc )
{
    // for real, map ConjTrans to Trans
    if (trans == Op::ConjTrans)
        trans = Op::Trans;

    char side_ = to_char( side );
    char uplo_ = to_char( uplo );
    char trans_ = to_char( trans );
    lapack_int m_ = to_lapack_int( m );
    lapack_int n_ = to_lapack_int( n );
    lapack_int lda_ = to_lapack_int( lda );
    lapack_int ldc_ = to_lapack_int( ldc );
    lapack_int info_ = 0;

    // query for workspace size
    double qry_work[1];
    lapack_int ineg_one = -1;
    LAPACK_dormtr(
        &side_, &uplo_, &trans_, &m_, &n_,
        A, &lda_,
        tau,
        C, &ldc_,
        qry_work, &ineg_one, &info_
    );
    if (info_ < 0) {
        throw Error();
    }
    lapack_int lwork_ = real(qry_work[0]);

    // allocate workspace
    lapack::vector< double > work( lwork_ );

    LAPACK_dormtr(
        &side_, &uplo_, &trans_, &m_, &n_,
        A, &lda_,
        tau,
        C, &ldc_,
        &work[0], &lwork_, &info_
    );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

}  // namespace lapack
