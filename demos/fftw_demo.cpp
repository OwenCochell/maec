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
#include <vector>
#include <complex.h>
#include <complex>
#include <fftw3.h>

int main(int argc , char** argv) {

    int N = 32;

    // Create vectors for data:

    std::vector<std::complex<long double>> in = {std::complex<long double>(-15.189251, 0.000000), std::complex<long double>(11.704489, -7.808442), std::complex<long double>(-0.090070, 10.845936), std::complex<long double>(-11.697811, -5.706792), std::complex<long double>(-2.788215, 7.507124), std::complex<long double>(8.873675, -10.790867), std::complex<long double>(-5.838451, 2.531785), std::complex<long double>(4.630811, -6.716717), std::complex<long double>(0.936014, -15.374274), std::complex<long double>(15.496844, -3.780477), std::complex<long double>(6.904307, -2.551331), std::complex<long double>(-4.702844, 12.109802), std::complex<long double>(-4.071020, 15.037189), std::complex<long double>(7.972179, -6.781337), std::complex<long double>(0.624014, -5.571347), std::complex<long double>(4.138358, -0.394291), std::complex<long double>(-1.130739, 0.000000), std::complex<long double>(4.138358, 0.394291), std::complex<long double>(0.624014, 5.571347), std::complex<long double>(7.972179, 6.781337), std::complex<long double>(-4.071020, -15.037189), std::complex<long double>(-4.702844, -12.109802), std::complex<long double>(6.904307, 2.551331), std::complex<long double>(15.496844, 3.780477), std::complex<long double>(0.936014, 15.374274), std::complex<long double>(4.630811, 6.716717), std::complex<long double>(-5.838451, -2.531785), std::complex<long double>(8.873675, 10.790867), std::complex<long double>(-2.788215, -7.507124), std::complex<long double>(-11.697811, 5.706792), std::complex<long double>(-0.090070, -10.845936), std::complex<long double>(11.704489, 7.808442)};
    std::vector<std::complex<long double>> out;

    // Reserve output vector:

    out.reserve(N);

    // Create the plan:

    fftwl_plan p = fftwl_plan_dft_1d(N, reinterpret_cast<fftwl_complex*>(in.data()), reinterpret_cast<fftwl_complex*>(out.data()), FFTW_FORWARD, FFTW_ESTIMATE);

    // Execute:

    fftwl_execute(p);

    // Iterate over out data:

    for(int i = 0; i < N; ++i) {

        // Output data

        std::cout << out[i] << std::endl;
    }
}
