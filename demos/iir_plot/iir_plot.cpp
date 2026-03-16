/**
 * @file iir_plot.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Plots the frequency response of an IIR filter
 * @version 0.1
 * @date 2026-03-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <matplot/freestanding/plot.h>
#include <matplot/matplot.h>

#include <complex>
#include <vector>

#include "dsp/iir.hpp"

int main() {

    namespace mp = matplot;

    // Sample rate for the filter

    const double sra = 44100.0;

    // Frequency step for plotting
    const double step = 100.0;

    // Coefficients for the filter

    std::vector<double> a_coes{0};
    std::vector<double> b_coes{0};

    // Determine the number of frequency values to plot

    std::size_t size = static_cast<std::size_t>((sra / 2) / step) + 1;

    std::vector<double> x_vals;
    std::vector<double> magnitude_db;
    std::vector<double> phase_deg;

    // Iterate over the frequencies to plot

    for (std::size_t i = 0; i < size; ++i) {

        // Compute the frequency in hertz for this index

        x_vals.push_back(i * step);

        // Compute the frequency response of the filter at this frequency

        std::complex<double> response = dsp::iir::freq_response(
            dsp::iir::freq_map(x_vals.back(), sra), a_coes, b_coes);

        // Compute the magnitude in decibels

        double mag = std::norm(response);

        // Phase is undefined near zero magnitude; use NaN to avoid noisy

        if (mag <= 1e-12) {
            phase_deg.push_back(std::numeric_limits<double>::quiet_NaN());
        } else {
            phase_deg.push_back(std::arg(response) *
                                (180.0 / std::numbers::pi_v<double>));
        }

        mag = std::max(mag, 1e-12);
        magnitude_db.push_back(20.0 * std::log10(mag));
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
