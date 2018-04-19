#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_posv(
    char uplo, lapack_int n, lapack_int nrhs, float* A, lapack_int lda, float* B, lapack_int ldb )
{
    return LAPACKE_sposv( LAPACK_COL_MAJOR, uplo, n, nrhs, A, lda, B, ldb );
}

static lapack_int LAPACKE_posv(
    char uplo, lapack_int n, lapack_int nrhs, double* A, lapack_int lda, double* B, lapack_int ldb )
{
    return LAPACKE_dposv( LAPACK_COL_MAJOR, uplo, n, nrhs, A, lda, B, ldb );
}

static lapack_int LAPACKE_posv(
    char uplo, lapack_int n, lapack_int nrhs, std::complex<float>* A, lapack_int lda, std::complex<float>* B, lapack_int ldb )
{
    return LAPACKE_cposv( LAPACK_COL_MAJOR, uplo, n, nrhs, A, lda, B, ldb );
}

static lapack_int LAPACKE_posv(
    char uplo, lapack_int n, lapack_int nrhs, std::complex<double>* A, lapack_int lda, std::complex<double>* B, lapack_int ldb )
{
    return LAPACKE_zposv( LAPACK_COL_MAJOR, uplo, n, nrhs, A, lda, B, ldb );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_posv_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    using real_t = blas::real_type< scalar_t >;
    typedef long long lld;

    // get & mark input values
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();
    int64_t nrhs = params.nrhs.value();
    int64_t align = params.align.value();
    int64_t verbose = params.verbose.value();

    // mark non-standard output values
    params.ref_time.value();
    params.ref_gflops.value();
    params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    int64_t lda = roundup( max( 1, n ), align );
    int64_t ldb = roundup( max( 1, n ), align );
    size_t size_A = (size_t) lda * n;
    size_t size_B = (size_t) ldb * nrhs;

    std::vector< scalar_t > A_tst( size_A );
    std::vector< scalar_t > A_ref( size_A );
    std::vector< scalar_t > B_tst( size_B );
    std::vector< scalar_t > B_ref( size_B );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, A_tst.size(), &A_tst[0] );
    lapack::larnv( idist, iseed, B_tst.size(), &B_tst[0] );

    // diagonally dominant -> positive definite
    for (int64_t i = 0; i < n; ++i) {
        A_tst[ i + i*lda ] += n;
    }
    A_ref = A_tst;
    B_ref = B_tst;

    if (verbose >= 1) {
        printf( "\n"
                "A n=%5lld, lda=%5lld\n"
                "B n=%5lld, nrhs=%5lld, ldb=%5lld",
                (lld) n, (lld) lda,
                (lld) n, (lld) nrhs, (lld) ldb );
    }
    if (verbose >= 2) {
        printf( "A = " ); print_matrix( n, n, &A_tst[0], lda );
        printf( "B = " ); print_matrix( n, nrhs, &B_tst[0], lda );
    }

    // test error exits
    if (params.error_exit.value() == 'y') {
        assert_throw( lapack::posv( Uplo(0),  n, nrhs, &A_tst[0], lda, &B_tst[0], ldb ), lapack::Error );
        assert_throw( lapack::posv( uplo,    -1, nrhs, &A_tst[0], lda, &B_tst[0], ldb ), lapack::Error );
        assert_throw( lapack::posv( uplo,     n,   -1, &A_tst[0], lda, &B_tst[0], ldb ), lapack::Error );
        assert_throw( lapack::posv( uplo,     n, nrhs, &A_tst[0], n-1, &B_tst[0], ldb ), lapack::Error );
        assert_throw( lapack::posv( uplo,     n, nrhs, &A_tst[0], lda, &B_tst[0], n-1 ), lapack::Error );
    }

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::posv( uplo, n, nrhs, &A_tst[0], lda, &B_tst[0], ldb );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::posv returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    double gflop = lapack::Gflop< scalar_t >::posv( n, nrhs );
    params.gflops.value() = gflop / time;

    if (verbose >= 2) {
        printf( "A2 = " ); print_matrix( n, n, &A_tst[0], lda );
        printf( "B2 = " ); print_matrix( n, nrhs, &B_tst[0], ldb );
    }

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_posv( uplo2char(uplo), n, nrhs, &A_ref[0], lda, &B_ref[0], ldb );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_posv returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        params.ref_gflops.value() = gflop / time;

        if (verbose >= 2) {
            printf( "A2ref = " ); print_matrix( n, n, &A_ref[0], lda );
            printf( "B2ref = " ); print_matrix( n, nrhs, &B_ref[0], ldb );
        }

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( A_tst, A_ref );
        error += abs_error( B_tst, B_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_posv( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_posv_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_posv_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_posv_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_posv_work< std::complex<double> >( params, run );
            break;
    }
}
