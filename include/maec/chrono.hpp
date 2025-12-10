/**
 * @file chrono.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various tools for working with time
 * @version 0.1
 * @date 2023-02-09
 *
 * @copyright Copyright (c) 2023
 *
 * This file contains components components for working with time.
 * Timekeeping is interesting in MAEC for many reasons!
 * Therefore, we define some components to help with this.
 *
 * One important thing to keep track of is chain time,
 * that being the time in relation to the chain.
 * TODO: Elaborate
 *
 * Of course, we contain methods for getting 'wall time',
 * that being the absolute time, independent of chain time.
 * Please note, that we do not work with absolute time (the current time)!
 * We work with time intervals, which alone have no relevance.
 *
 * We measure time values in nanoseconds.
 */

#include <chrono>
#include <ctime>

#include "const.hpp"

/**
 * @brief Gets the current time value
 *
 * This function returns a time value that is not tied to the current time!
 * Meaning, that you can't derive the current 'wall clock' time from this value
 * alone. You can make a conclusion about a time interval by comparing the value
 * returned by this method to another, which is the purpose of this method: to
 * measure very small intervals of time.
 *
 * We use the C++ chrono library under the hood.
 *
 * @return Current time value
 */
int64_t get_time();

/**
 * @brief Handles timekeeping on a chain
 *
 * This class keeps track of time relative to the module chain,
 * NOT the current wall time.
 * Chain time does not necessarily correlate to the current wall time, but it
 * might sometimes.
 *
 * For example, let's say we are sampling audio at 100 frames per second.
 * Each frame would represent 1 / 100 seconds each.
 * If we keep a counter of how many frames are processed, then we can
 * understand where we are from the beginning.
 *
 * This system allows us to do accurate timekeeping.
 * For example, what if the audio is consumed faster than expected,
 * let's say at 200 frames per second.
 * If we used wall time, then any effects dependent on timekeeping will
 * inaccurate, they will be 'slower', as the wall time delta will be less. Same
 * goes for if we are processing audio slower than expected, say 50 frames per
 * second. This will lead to audio effects that are very fast, as the wall time
 * delta will be longer.
 *
 * This system also allows for accurate audio generation in situations where
 * output may not be constant or has a lot of lag. The output may lag, but the
 * audio information being generated will be accurate.
 *
 * This class requires the current sampling rate, and it requires that a counter
 * is kept. This can be as simple as setting it to a value after an entire
 * buffer is processed, to as fine tuned as incrementing it every time a single
 * frame is processed.
 *
 * Optionally, we can be made aware of the number of channel's in the audio data
 * we are processing. This allows us to increment the current frame only when a
 * certain number of samples have been processed, thus allowing for accurate,
 * synchronized timekeeping on all channels. It is HIGHLY recommended to iterate
 * over audio in interleaved format if you will be using this feature!
 *
 */
class ChainTimer {

private:
    /// The number of channels to consider
    int channels = 1;

    /// Current sample
    int sample = 0;

    /// Nanoseconds per frame
    int64_t nano_frame = NANO / SAMPLE_RATE;

public:
    ChainTimer() = default;

    /**
     * @brief Resets this timer to a starting state
     *
     * We reset all variables to their default values.
     *
     */
    void reset();

    /**
     * @brief Sets the samplerate to the given value
     *
     * We utilize the samplerate, that being the number
     * of sample per second,
     * to determine the number of nanoseconds per frame,
     * which allows for accurate timekeeping.
     *
     * Please note, we do not save this samplerate!
     * We simply use it to determine the NPF.
     *
     * @param samplerate Number of samples per second
     */
    void set_samplerate(int samplerate) {
        this->nano_frame = NANO / samplerate;
    }

    /**
     * @brief Gets the samplerate of this timer
     *
     * We calculate an approximate samplerate using the NPF.
     *
     * @return double Current approximate samplerate
     */
    int get_samplerate() const {
        return static_cast<int>(NANO / this->nano_frame);
    }

    /**
     * @brief Sets the Nano Seconds Per Frame (NPF)
     *
     * This value determines the number of nanoseconds in a given frame.
     * It is recommended to set this value using the sample rate in samples per
     * second! But, if need be, you can set this value manually.
     *
     * @param npf Nano Seconds Per Frame
     */
    void set_npf(int64_t npf) { this->nano_frame = npf; }

    /**
     * @brief Gets the Nano Seconds Per Frame (NPF)
     *
     * @return double Nano Seconds Per Frame
     */
    int64_t get_npf() const { return this->nano_frame; }

    /**
     * @brief Sets the number of channels
     *
     * We use this value to determine when a frame has passed.
     * We aim to only increment the time after a frame has passed,
     * as each channel should occur at the same time.
     *
     * @param chan Number of channels
     */
    void set_channels(int chan) { this->channels = chan; }

    /**
     * @brief Gets the number of channels
     *
     * @return int Number of channels
     */
    int get_channels() const { return this->channels; }

    /**
     * @brief Gets the current time
     *
     * We use the number of frames to determine the current time.
     *
     * TODO: This method might not be as efficient as it could be!
     *
     * @return int64_t Current time in nanoseconds
     */
    int64_t get_time() const {
        return static_cast<int64_t>((this->sample / this->channels) *
                                    this->nano_frame);
    }

    /**
     * @brief Gets the current time for the given values
     *
     * We compute the elapsed time using the given
     * sample and channel number.
     * This can be useful if you wish to compute the elapsed
     * time independent of the current sample count.
     *
     * @param sample Number of samples
     * @param channel Number of channels
     * @return int64_t Elapsed time in nanoseconds
     */
    int64_t get_time(int sample, int channel) const {
        return static_cast<int64_t>(sample / channel) * this->nano_frame;
    }

    /**
     * @brief Gets the current time for the given sample count
     *
     * We compute the elapsed time using the given
     * sample count.
     * This can be useful if you wish to compute the elapsed
     * time independent of the current sample count.
     *
     * @param sample Number of samples
     * @return int74_t Elapsed time in nanoseconds
     */
    int64_t get_time(int sample) const {
        return static_cast<int64_t>(sample) * this->nano_frame;
    }

    /**
     * @brief Sets the sample to the given value
     *
     * The sample is the current position in the audio stream.
     * A sample is NOT related to channels!
     *
     * TODO: Elaborate once we get central docs going
     *
     * @param samp
     */
    void set_sample(int samp) { this->sample = samp; }

    /**
     * @brief Increments the sample
     *
     * This method increments the sample by one.
     * You can use this method to keep the number of samples
     * processed synced in your module.
     *
     */
    void inc_sample() { ++(this->sample); }

    /**
     * @brief Adds the given value to the current sample count
     *
     * This is useful if we wish to update the sample number
     * in chunks instead of one at a time.
     *
     * @param val Value to add to sample number
     */
    void add_sample(int val) { this->sample += val; }

    /**
     * @brief Gets the current sample
     *
     * We return the current sample in this timer.
     *
     * @return int Current sample
     */
    int get_sample() const { return this->sample; }
};
