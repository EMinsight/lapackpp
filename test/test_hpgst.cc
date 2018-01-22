#include "test.hh"
#include "lapack.hh"
#include "lapack_flops.hh"
#include "print_matrix.hh"
#include "error.hh"

#include <vector>

// -----------------------------------------------------------------------------
// simple overloaded wrappers around LAPACKE
static lapack_int LAPACKE_hpgst(
    lapack_int itype, char uplo, lapack_int n, float* AP, float* BP )
{
    return LAPACKE_sspgst( LAPACK_COL_MAJOR, itype, uplo, n, AP, BP );
}

static lapack_int LAPACKE_hpgst(
    lapack_int itype, char uplo, lapack_int n, double* AP, double* BP )
{
    return LAPACKE_dspgst( LAPACK_COL_MAJOR, itype, uplo, n, AP, BP );
}

static lapack_int LAPACKE_hpgst(
    lapack_int itype, char uplo, lapack_int n, std::complex<float>* AP, std::complex<float>* BP )
{
    return LAPACKE_chpgst( LAPACK_COL_MAJOR, itype, uplo, n, AP, BP );
}

static lapack_int LAPACKE_hpgst(
    lapack_int itype, char uplo, lapack_int n, std::complex<double>* AP, std::complex<double>* BP )
{
    return LAPACKE_zhpgst( LAPACK_COL_MAJOR, itype, uplo, n, AP, BP );
}

// -----------------------------------------------------------------------------
template< typename scalar_t >
void test_hpgst_work( Params& params, bool run )
{
    using namespace libtest;
    using namespace blas;
    typedef typename traits< scalar_t >::real_t real_t;
    typedef long long lld;

    // get & mark input values
    int64_t itype = params.itype.value();
    lapack::Uplo uplo = params.uplo.value();
    int64_t n = params.dim.n();

    // mark non-standard output values
    params.ref_time.value();
    // params.ref_gflops.value();
    // params.gflops.value();

    if (! run)
        return;

    // ---------- setup
    size_t size_AP = (size_t) (n*(n+1)/2);
    size_t size_BP = (size_t) (n*(n+1)/2);

    std::vector< scalar_t > AP_tst( size_AP );
    std::vector< scalar_t > AP_ref( size_AP );
    std::vector< scalar_t > BP( size_BP );

    int64_t idist = 1;
    int64_t iseed[4] = { 0, 1, 2, 3 };
    lapack::larnv( idist, iseed, AP_tst.size(), &AP_tst[0] );
    lapack::larnv( idist, iseed, BP.size(), &BP[0] );

    // diagonally dominant -> positive definite
    if (uplo == lapack::Uplo::Upper) {
        for (int64_t i = 0; i < n; ++i) {
            BP[ i + 0.5*(i+1)*i ] += n;
        }
    } else { // lower
        for (int64_t i = 0; i < n; ++i) {
            BP[ i + n*i - 0.5*i*(i+1) ] += n;
        }
    }

    // Factor BP
    int64_t info_trf = lapack::pptrf( uplo, n, &BP[0] );
    if (info_trf != 0) {
        fprintf( stderr, "lapack::pptrf returned error %lld\n", (lld) info_trf );
    }

    AP_ref = AP_tst;

    // ---------- run test
    libtest::flush_cache( params.cache.value() );
    double time = get_wtime();
    int64_t info_tst = lapack::hpgst( itype, uplo, n, &AP_tst[0], &BP[0] );
    time = get_wtime() - time;
    if (info_tst != 0) {
        fprintf( stderr, "lapack::hpgst returned error %lld\n", (lld) info_tst );
    }

    params.time.value() = time;
    // double gflop = lapack::Gflop< scalar_t >::hpgst( itype, n );
    // params.gflops.value() = gflop / time;

    if (params.ref.value() == 'y' || params.check.value() == 'y') {
        // ---------- run reference
        libtest::flush_cache( params.cache.value() );
        time = get_wtime();
        int64_t info_ref = LAPACKE_hpgst( itype, uplo2char(uplo), n, &AP_ref[0], &BP[0] );
        time = get_wtime() - time;
        if (info_ref != 0) {
            fprintf( stderr, "LAPACKE_hpgst returned error %lld\n", (lld) info_ref );
        }

        params.ref_time.value() = time;
        // params.ref_gflops.value() = gflop / time;

        // ---------- check error compared to reference
        real_t error = 0;
        if (info_tst != info_ref) {
            error = 1;
        }
        error += abs_error( AP_tst, AP_ref );
        params.error.value() = error;
        params.okay.value() = (error == 0);  // expect lapackpp == lapacke
    }
}

// -----------------------------------------------------------------------------
void test_hpgst( Params& params, bool run )
{
    switch (params.datatype.value()) {
        case libtest::DataType::Integer:
            throw std::exception();
            break;

        case libtest::DataType::Single:
            test_hpgst_work< float >( params, run );
            break;

        case libtest::DataType::Double:
            test_hpgst_work< double >( params, run );
            break;

        case libtest::DataType::SingleComplex:
            test_hpgst_work< std::complex<float> >( params, run );
            break;

        case libtest::DataType::DoubleComplex:
            test_hpgst_work< std::complex<double> >( params, run );
            break;
    }
}
