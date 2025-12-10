/**
 * @file fftw_dempocpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Sends data through FFTW for demo purposes
 * @version 0.1
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 * This demo is designed to send data through FFTW,
 * which can be used to compare results with other FFT algorithms
 */

#include <iostream>

#ifdef FFTW

#include <complex.h>
#include <fftw3.h>

#include <complex>
#include <iomanip>
#include <vector>

int main(int argc, char** argv) {

    // Some dummy testing:

    std::vector<long double> ft_data2 = {
        -9.6733407586,   62.9653550514,   74.4000694857,   99.1967369526,
        93.4037032815,   57.3335037397,   101.7345720310,  18.1861254434,
        14.4572532371,   -16.1701715992,  -90.1191210720,  65.5052860315,
        -50.6822660903,  85.6097778570,   64.9082173196,   20.7108693629,
        80.0814240580,   105.6846021807,  65.2129539715,   -85.4664831284,
        -11.1276864500,  -99.0217522157,  -104.1126140924, -12.2773308137,
        100.2898453047,  -82.4167709429,  103.5442857695,  -81.0976502582,
        -109.1820435246, 30.8008533158,   29.1963157524,   -11.3502179318,
        -6.2240021300,   -38.8990464614,  -52.9055938733,  48.6190705661,
        50.3520832266,   -62.9548072614,  -69.0177881626,  104.6779683739,
        39.5905650836,   -101.7229133271, 43.2043503664,   47.9158760699,
        -56.6139623555,  -21.0172069627,  -31.0987360694,  -16.0686666005,
        -31.4280055848,  -76.0297646764,  -9.3888213890,   82.1388450892,
        -25.3763959200,  -14.4709267300,  -51.6942710109,  -19.6178678761,
        -3.1049798902,   -24.6729543717,  11.6011396251,   -55.8205348801,
        -46.1782259805,  -75.5780087238,  -95.4761768191,  3.0709157917};

    // std::complex<double> *cmp =
    // reinterpret_cast<std::complex<double>*>(ft_data2.data());

    // Iterate over numbers:

    for (int i = 0; i < ft_data2.size() / 2; ++i) {

        // std::cout << *(cmp+i) << std::endl;
    }

    int N = 64;

    // Create vectors for data:

    // std::vector<std::complex<double>> in = {std::complex<double>(-15.189251,
    // 0.000000), std::complex<double>(11.704489, -7.808442),
    // std::complex<double>(-0.090070, 10.845936),
    // std::complex<double>(-11.697811, -5.706792),
    // std::complex<double>(-2.788215, 7.507124), std::complex<double>(8.873675,
    // -10.790867), std::complex<double>(-5.838451, 2.531785),
    // std::complex<double>(4.630811, -6.716717), std::complex<double>(0.936014,
    // -15.374274), std::complex<double>(15.496844, -3.780477),
    // std::complex<double>(6.904307, -2.551331),
    // std::complex<double>(-4.702844, 12.109802),
    // std::complex<double>(-4.071020, 15.037189),
    // std::complex<double>(7.972179, -6.781337), std::complex<double>(0.624014,
    // -5.571347), std::complex<double>(4.138358, -0.394291),
    // std::complex<double>(-1.130739, 0.000000), std::complex<double>(4.138358,
    // 0.394291), std::complex<double>(0.624014, 5.571347),
    // std::complex<double>(7.972179, 6.781337), std::complex<double>(-4.071020,
    // -15.037189), std::complex<double>(-4.702844, -12.109802),
    // std::complex<double>(6.904307, 2.551331),
    // std::complex<double>(15.496844, 3.780477),
    // std::complex<double>(0.936014, 15.374274),
    // std::complex<double>(4.630811, 6.716717), std::complex<double>(-5.838451,
    // -2.531785), std::complex<double>(8.873675, 10.790867),
    // std::complex<double>(-2.788215, -7.507124),
    // std::complex<double>(-11.697811, 5.706792),
    // std::complex<double>(-0.090070, -10.845936),
    // std::complex<double>(11.704489, 7.808442)}; std::vector<double> in =
    // {-81.0611399999996160709, -32.4312400000002611124,
    // -38.8382199999997827569,
    // -111.7422700000002612, 52.9393500000000637312, 61.0861000000000130931,
    // -16.2175099999997593923, -10.9511700000003418193,
    // -61.7699399999997511741, -69.2286100000000882773,
    // 0.421569999999996698238, 37.6517800000001357057, 80.4401700000000488971,
    // -2.04557000000011644271, -85.9569899999997715442, 2.72508999999991183543,
    // -66.3554300000000642709, -2.34488999999995559407,
    // -40.3914900000000819981, -3.64987999999990044789,
    // -17.7549800000004358823, 71.0510400000002124293, -48.0264400000000275734,
    // 110.045180000000132392, -105.146259999999952539,
    // -56.3275699999998687544, 95.8438100000005266874, 64.2721399999995513172,
    // -43.4969999999996350254, -16.9237900000002831694,
    // -109.863809999999695023, 84.7972800000000368273, 77.4762800000004462145, 57.2437800000003134647,
    // -110.995029999999789382, -55.1876299999999012057,
    // -95.6075800000002865298, 75.4721999999997809339, -12.7008200000005678967,
    // -36.9976999999999474796, -49.1758900000000834345,
    // -71.2256300000002166503, 108.823060000000235506,
    // -110.420349999999915749, 90.9002899999999186831,
    // -105.065209999999517897, 45.9153499999996984778, -1.05934999999953613926,
    // -92.979900000000370891, -44.7161299999997329906};
    std::vector<std::complex<double>> out;

    // Reserve output vector:

    out.reserve(N);

    // Create the plan:

    // fftwl_plan p = fftwl_plan_dft_1d(N,
    // reinterpret_cast<fftwl_complex*>(in.data()),
    // reinterpret_cast<fftwl_complex*>(out.data()), FFTW_FORWARD,
    // FFTW_ESTIMATE);
    fftwl_plan p = fftwl_plan_dft_r2c_1d(
        N, ft_data2.data(), reinterpret_cast<fftwl_complex*>(out.data()),
        FFTW_ESTIMATE);
    // fftwl_plan p = fftwl_plan_dft_1d(N,
    // reinterpret_cast<fftwl_complex*>(cmp),
    // reinterpret_cast<fftwl_complex*>(out.data()), FFTW_FORWARD,
    // FFTW_ESTIMATE);

    // Execute:

    fftwl_execute(p);

    // Iterate over out data:

    for (int i = 0; i < N / 2 + 1; ++i) {

        // Output data

        std::cout << std::setprecision(17) << out[i] << std::endl;
    }
}

#else

int main(int argc, char** argv) {

    std::cout << "FFTW Is not installed!" << std::endl;
}

#endif
