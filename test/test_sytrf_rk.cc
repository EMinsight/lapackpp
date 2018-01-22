#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

#if LAPACK_VERSION_MAJOR >= 3 && LAPACK_VERSION_MINOR >= 7  // >= 3.7

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_sytrf_rk(
    char uplo, lapack_int n, float* A, lapack_int lda, float* E, lapack_int* ipiv )
{
    return LAPACKE_ssytrf_rk( LAPACK_COL_MAJOR, uplo, n, A, lda, E, ipiv );
}

static lapack_int LAPACKE_sytrf_rk(
    char uplo, lapack_int n, double* A, lapack_int lda, double* E, lapack_int* ipiv )
{
    return LAPACKE_dsytrf_rk( LAPACK_COL_MAJOR, uplo, n, A, lda, E, ipiv );
}

static lapack_int LAPACKE_sytrf_rk(
    char uplo, lapack_int n, std::complex<float>* A, lapack_int lda, std::complex<float>* E, lapack_int* ipiv )
{
    return LAPACKE_csytrf_rk( LAPACK_COL_MAJOR, uplo, n, A, lda, E, ipiv );
}

static lapack_int LAPACKE_sytrf_rk(
    char uplo, lapack_int n, std::complex<double>* A, lapack_int lda, std::complex<double>* E, lapack_int* ipiv )
{
    return LAPACKE_zsytrf_rk( LAPACK_COL_MAJOR, uplo, n, A, lda, E, ipiv );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_sytrf_rk_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();
    int64_t align = params.align.value();

    // mark non-standard output values
    params.ref_time.value();
    // params.ref_gflops.value();
    // params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    int64_t lda = roundup( max( 1, n ), align );
    size_t size_A = (size_t) lda * n;
    size_t size_E = (size_t) (n);
    size_t size_ipiv = (size_t) (n);

    std::vector< scalar_t > A_tst( size_A );
    std::vector< scalar_t > A_ref( size_A );
    std::vector< scalar_t > E_tst( size_E );
    std::vector< scalar_t > E_ref( size_E );
    std::vector< int64_t > ipiv_tst( size_ipiv );
    std::vector< lapack_int > ipiv_ref( size_ipiv );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, A_tst.size(), &A_tst[0] );
    A_ref = A_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::sytrf_rk( uplo, n, &A_tst[0], lda, &E_tst[0], &ipiv_tst[0] );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::sytrf_rk returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    // double gflop = lapack::Gflop< scalar_t >::sytrf_rk( n );
    // params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_sytrf_rk( uplo2char(uplo), n, &A_ref[0], lda, &E_ref[0], &ipiv_ref[0] );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_sytrf_rk returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        // params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( A_tst, A_ref );
        error += abs_error( E_tst, E_ref );
        error += abs_error( ipiv_tst, ipiv_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_sytrf_rk( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_sytrf_rk_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_sytrf_rk_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_sytrf_rk_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_sytrf_rk_work< std::complex<double> >( params, run );
            break;
    }
}

#else

// -----------------------------------------------------------------------------
void test_sytrf_rk( Params& params, bool run )
{
    fprintf( stderr, "sysv_trf requires LAPACK >= 3.7\n\n" );
    exit(0);
}

#endif  // LAPACK >= 3.7
