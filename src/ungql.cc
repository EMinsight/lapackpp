#include "lapack.hh"
#include "lapack_fortran.h"

#include <vector>

namespace lapack {

using blas::max;
using blas::min;
using blas::real;

// -----------------------------------------------------------------------------
/// @ingroup geqlf
int64_t ungql(
    int64_t m, int64_t n, int64_t k,
    std::complex<float>* A, int64_t lda,
    std::complex<float> const* tau )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(m) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(k) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(lda) > std::numeric_limits<blas_int>::max() );
    }
    blas_int m_ = (blas_int) m;
    blas_int n_ = (blas_int) n;
    blas_int k_ = (blas_int) k;
    blas_int lda_ = (blas_int) lda;
    blas_int info_ = 0;

    // query for workspace size
    std::complex<float> qry_work[1];
    blas_int ineg_one = -1;
    LAPACK_cungql( &m_, &n_, &k_, A, &lda_, tau, qry_work, &ineg_one, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    blas_int lwork_ = real(qry_work[0]);

    // allocate workspace
    std::vector< std::complex<float> > work( lwork_ );

    LAPACK_cungql( &m_, &n_, &k_, A, &lda_, tau, &work[0], &lwork_, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

// -----------------------------------------------------------------------------
/// Generates an m-by-n matrix Q with orthonormal columns,
/// which is defined as the last n columns of a product of k elementary
/// reflectors of order m, as returned by `lapack::geqlf`:
///
///     \f[ Q = H(k) \dots H(2) H(1). \f]
///
/// Overloaded versions are available for
/// `float`, `double`, `std::complex<float>`, and `std::complex<double>`.
/// For real matrices, this is an alias for `lapack::orgql`.
///
/// @param[in] m
///     The number of rows of the matrix Q. m >= 0.
///
/// @param[in] n
///     The number of columns of the matrix Q. m >= n >= 0.
///
/// @param[in] k
///     The number of elementary reflectors whose product defines the
///     matrix Q. n >= k >= 0.
///
/// @param[in,out] A
///     The m-by-n matrix A, stored in an lda-by-n array.
///     On entry, the (n-k+i)-th column must contain the vector which
///     defines the elementary reflector H(i), for i = 1, 2, ..., k, as
///     returned by `lapack::geqlf` in the last k columns of its array
///     argument A.
///     On exit, the m-by-n matrix Q.
///
/// @param[in] lda
///     The first dimension of the array A. lda >= max(1,m).
///
/// @param[in] tau
///     The vector tau of length k.
///     tau(i) must contain the scalar factor of the elementary
///     reflector H(i), as returned by `lapack::geqlf`.
///
/// @retval = 0: successful exit
///
/// @ingroup geqlf
int64_t ungql(
    int64_t m, int64_t n, int64_t k,
    std::complex<double>* A, int64_t lda,
    std::complex<double> const* tau )
{
    // check for overflow
    if (sizeof(int64_t) > sizeof(blas_int)) {
        throw_if_( std::abs(m) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(n) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(k) > std::numeric_limits<blas_int>::max() );
        throw_if_( std::abs(lda) > std::numeric_limits<blas_int>::max() );
    }
    blas_int m_ = (blas_int) m;
    blas_int n_ = (blas_int) n;
    blas_int k_ = (blas_int) k;
    blas_int lda_ = (blas_int) lda;
    blas_int info_ = 0;

    // query for workspace size
    std::complex<double> qry_work[1];
    blas_int ineg_one = -1;
    LAPACK_zungql( &m_, &n_, &k_, A, &lda_, tau, qry_work, &ineg_one, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    blas_int lwork_ = real(qry_work[0]);

    // allocate workspace
    std::vector< std::complex<double> > work( lwork_ );

    LAPACK_zungql( &m_, &n_, &k_, A, &lda_, tau, &work[0], &lwork_, &info_ );
    if (info_ < 0) {
        throw Error();
    }
    return info_;
}

}  // namespace lapack
