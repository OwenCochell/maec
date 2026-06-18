/**
 * @file samp.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for sample rate conversion
 * @version 0.1
 * @date 2026-05-28
 *
 * @copyright Copyright (c) 2026
 *
 * This file contains modules for preforming resampling operations.
 * Each method has it's own costs/benefits that should be considered before use.
 */

#pragma once

#include <cmath>
#include <vector>

#include "audio_buffer.hpp"
#include "audio_module.hpp"
#include "const.hpp"
#include "dsp/interp.hpp"

struct InterpInfo {

    /// Number of backwards points
    std::size_t b_points = 0;

    /// Number of forward points
    std::size_t f_points = 0;

    static double value_behind(std::size_t index, const AudioBuffer& buff,
                               const std::vector<double>& tail) {

        // Are we trying to access data in the tail?

        if (index < 0) {
            return tail.at(tail.size() - index);
        }

        // Otherwise, just access the buffer

        return buff.at(index);
    }

    static double value_ahead(std::size_t index, const AudioBuffer& buff,
                              const AudioBuffer& forward_buff) {

        // We are reading data beyond the current buffer?

        if (index >= buff.size()) {

            // We need to access the forward buffer,
            // we need to subtract the current buffer size from the index

            return forward_buff.at(index - buff.size());
        }

        // Just access the buffer buffer

        return buff.at(index);
    }
};

template <double(Fn)(double, double, double)>
struct TwoInterp : public InterpInfo {
    /// We have no backwards points
    std::size_t b_points = 0;

    /// We only need one forward point
    std::size_t f_points = 1;

    static double interp_func(double between, std::size_t low,
                              const AudioBuffer& buff,
                              const AudioBuffer& forward_buff,
                              const std::vector<double>& /*unused*/) {

        // get the high value, linear interpolation only needs one ahead,
        // so we will never need to access the tail

        double high_val = value_ahead(low + 1, buff, forward_buff);

        // Perform linear interpolation

        return Fn(between, buff.at(low), high_val);
    }
};

template <double(Fn)(double, double, double, double, double)>
struct InterpFour : public InterpInfo {

    /// We need one backwards point
    std::size_t b_points = 1;

    /// We need two forward points
    std::size_t f_points = 2;

    static double interp_func(double between, std::size_t low,
                              const AudioBuffer& buff,
                              const AudioBuffer& forward_buff,
                              const std::vector<double>& tail) {

        // Get the values we need for interpolation

        double y0 = value_behind(low - 1, buff, tail);
        double y1 = value_ahead(low, buff, forward_buff);
        double y2 = value_ahead(low + 1, buff, forward_buff);
        double y3 = value_ahead(low + 2, buff, forward_buff);

        // Perform interpolation

        return Fn(between, y0, y1, y2, y3);
    }
};

template <double(Fn)(double, double, double, double, double, double, double)>
struct InterpSix : public InterpInfo {

    /// We need two backwards points
    std::size_t b_points = 2;

    /// We need four forward points
    std::size_t f_points = 4;

    static double interp_func(double between, std::size_t low,
                              const AudioBuffer& buff,
                              const AudioBuffer& forward_buff,
                              const std::vector<double>& tail) {

        // Get the values we need for interpolation

        double y0 = value_behind(low - 2, buff, tail);
        double y1 = value_behind(low - 1, buff, tail);
        double y2 = value_ahead(low, buff, forward_buff);
        double y3 = value_ahead(low + 1, buff, forward_buff);
        double y4 = value_ahead(low + 2, buff, forward_buff);
        double y5 = value_ahead(low + 3, buff, forward_buff);

        // Perform interpolation

        return Fn(between, y0, y1, y2, y3, y4, y5);
    }
};

///
// Interpolation Methods
///

using RoundInterp = TwoInterp<dsp::interp::round>;
using LinearInterp = TwoInterp<dsp::interp::linear>;
using Lagrange4Interp = InterpFour<dsp::interp::lagrange4>;
using Lagrange6Interp = InterpSix<dsp::interp::lagrange6>;

template <typename Fn>
class SampInterp : public AudioModule<> {
private:
    /// Target rate to resample to
    uint32_t samp = 44100;

    /// Current outgoing buffer index
    int index = 0;

    /// Current incoming buffer index
    double iindex = 0;

    /// Temporary buffer to contain incoming data
    AudioBuffer inp;

    /// Temporary buffer containing future data
    AudioBuffer forward_buff;

    /// Buffer containing past data
    std::array<double, Fn::b_points> tail{};

public:
    SampInterp() = default;

    void info_sync() override {

        // First, invoke the parent info sync method,
        // this will copy all the values over
        // (even the bad sample rate!)

        AudioModule<>::info_sync();

        // Update the samplerate to be our value

        this->get_info()->sample_rate = samp;
    }

    void process() override {

        // Determine the increment interval,
        // this is the rate we iterate over the values in the buffer

        const double inc = this->backward().get_info()->sample_rate / samp;

        // Reserve the outgoing buffer as it's not done automatically

        this->reserve();

        // Set the sample rate to be our configured value

        this->buff.set_samplerate(samp);

        // We need to iterate until we have filled the outgoing buffer

        while (this->index < this->get_info()->out_buffer) {

            // Are we out of values?

            if (this->inp.size() == 0 || this->iindex >= this->inp.size()) {

                // Reset the input index,
                // subtract from the original size

                this->iindex -= this->inp.size();

                // Move the future data into the current buffer

                this->inp = std::move(this->forward_buff);

                // We need new data, first meta-process backwards module

                this->backward().meta_process();

                // Grab the buffer

                this->forward_buff = std::move(this->backward().get_buffer());
            }

            // Determine the in-between value,
            // we just want the remainder

            double between = std::fmod(this->iindex, 1);

            // Get the start and stop indexes

            auto low = static_cast<std::size_t>(this->iindex);
            auto high = low + 1;

            // Invoke the the interpolation function,
            // we want to find the value between the two samples

            double res = Fn::interp_func(between, low, this->inp,
                                         this->forward_buff, this->tail);

            // Add the value to the output buffer

            this->buff.at(this->index++) = res;

            // Increment the input sample count

            this->iindex += inc;
        }
    }
};
