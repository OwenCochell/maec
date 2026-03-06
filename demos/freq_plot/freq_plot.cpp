/**
 * @file freq_plot.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Plots the frequency components in a signal
 * @version 0.1
 * @date 2026-03-05
 *
 * @copyright Copyright (c) 2026
 *
 * Given a signal,
 * we compute the Fourier Transform and plot the results.
 */

#include <matplot/freestanding/plot.h>
#include <matplot/matplot.h>

#include <array>
#include <cmath>
#include <dsp/const.hpp>
#include <dsp/ft.hpp>
#include <dsp/kernel.hpp>
#include <iostream>
#include <vector>

int main() {

    // Define the signal to transform

    std::array<double, 251> sig{};

    // Create a sinc kernel (frequency must be normalized to [0, 0.5])

    sinc_kernel(5000.0 / SAMPLE_RATE, sig.size(), sig.begin());

    // Signal parameters

    const double sra = SAMPLE_RATE;

    // Determine the size of the resulting transform

    std::size_t size = length_ft(sig.size());

    std::cout << "Output Size: " << size << "\n";

    // Define the structures for each signal

    std::vector<double> real(size);
    std::vector<double> imag(size);

    // Compute the DFT

    dft(sig.begin(), sig.size(), real.begin(), imag.begin());

    // Determine the dependent variable and compute magnitude

    std::vector<double> x_vals(size);
    std::vector<double> magnitude(size);

    for (std::size_t i = 0; i < size; ++i) {
        // Use input signal size (sig.size()), not DFT output size,
        // so that bin spacing = sample_rate / N (correct)
        x_vals[i] = index_freq(i, sig.size(), sra);
        magnitude[i] = std::sqrt((real[i] * real[i]) + (imag[i] * imag[i]));
    }

    // Plot the magnitude response

    matplot::plot(x_vals, magnitude);
    matplot::show();

    return 0;
}
