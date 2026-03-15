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

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <dsp/const.hpp>
#include <dsp/ft.hpp>
#include <dsp/kernel.hpp>
#include <iostream>
#include <limits>
#include <numbers>
#include <vector>

int main() {

    namespace mp = matplot;

    // Define the signal to transform

    constexpr std::size_t signal_size = 51;
    std::array<double, signal_size> sig{};

    // Create a sinc kernel (frequency must be normalized to [0, 0.5])

    constexpr double cutoff_hz = 5000.0;
    dsp::kern::sinc_kernel(cutoff_hz / dsp::consts::SAMPLE_RATE, sig.size(),
                           sig.begin());

    // Signal parameters

    const double sra = dsp::consts::SAMPLE_RATE;

    // Determine the size of the resulting transform

    std::size_t size = dsp::ft::length_ft(sig.size());

    std::cout << "Output Size: " << size << "\n";

    // Define the structures for each signal

    std::vector<std::complex<double>> output(size);

    // Compute the DFT

    dsp::ft::dft(sig.begin(), sig.size(), output.begin());

    // Build a one-sided magnitude and phase spectrum.
    // Skip bin 0 so we can use a logarithmic x-axis (log(0) is undefined).

    std::vector<double> x_vals;
    std::vector<double> magnitude_db;
    std::vector<double> phase_deg;

    x_vals.reserve(size > 0 ? size - 1 : 0);
    magnitude_db.reserve(size > 0 ? size - 1 : 0);
    phase_deg.reserve(size > 0 ? size - 1 : 0);

    constexpr double eps = 1e-12;
    constexpr double one_sided_scale = 2.0;
    constexpr double amplitude_to_db = 20.0;
    constexpr double rad_to_deg = 180.0 / std::numbers::pi_v<double>;
    const double nan = std::numeric_limits<double>::quiet_NaN();

    for (std::size_t i = 1; i < size; ++i) {
        x_vals.push_back(dsp::ft::index_freq(i, sig.size(), sra));

        double mag = std::norm(output[i]);

        // One-sided correction for real-valued input.
        // Keep DC (i=0, skipped above) and Nyquist (when present) unscaled.
        const bool is_nyquist = (sig.size() % 2 == 0) && (i == size - 1);
        if (!is_nyquist) {
            mag *= one_sided_scale;
        }

        // Phase is undefined near zero magnitude; use NaN to avoid noisy
        // spikes.
        if (mag <= eps) {
            phase_deg.push_back(nan);
        } else {
            phase_deg.push_back(std::arg(output[i]) * rad_to_deg);
        }

        mag = std::max(mag, eps);
        magnitude_db.push_back(amplitude_to_db * std::log10(mag));
    }

    // Plot using conventions commonly used for frequency-response
    // visualization.

    constexpr std::size_t figure_width = 1000;
    constexpr std::size_t figure_height = 800;
    constexpr double line_width = 2.0;

    auto fig = mp::figure(true);
    fig->size(figure_width, figure_height);
    fig->name("Frequency Response");
    fig->color("w");

    mp::subplot(2, 1, 1);
    auto mag_line = mp::semilogx(x_vals, magnitude_db);
    mag_line->line_width(line_width);
    mp::grid(mp::on);
    mp::title("One-Sided Magnitude Spectrum");
    mp::xlabel("Frequency (Hz)");
    mp::ylabel("Magnitude (dB)");

    mp::subplot(2, 1, 2);
    auto phase_line = mp::semilogx(x_vals, phase_deg);
    phase_line->line_width(line_width);
    mp::grid(mp::on);
    mp::title("Phase Response");
    mp::xlabel("Frequency (Hz)");
    mp::ylabel("Phase (degrees)");
    mp::show();

    return 0;
}
