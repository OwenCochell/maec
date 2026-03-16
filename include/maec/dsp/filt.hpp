/**
 * @file filt.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various components and definitions for signal filters
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 * This file contains various components for working with filters.
 * Mainly, we define the filter parameters that all maec filters will utilize,
 * Most likely, you will want to explore filter implementations (such as FIR and
 * IIR filters) which are defined elsewhere in maec.
 */

#include "dsp/const.hpp"

namespace dsp::filt {

struct FilterParams {

    /// Type of filter
    consts::FilterType type = consts::FilterType::LowPass;

    /// Start frequency (for low pass, band pass, and band reject)
    double start_freq = 0;

    /// Stop frequency (for high pass, band pass, and band reject)
    double stop_freq = 0;

    /// Sample rate in hertz the filter will be operating at
    double sra = consts::SAMPLE_RATE;
};

}  // namespace dsp::filt
