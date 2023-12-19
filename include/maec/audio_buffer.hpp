/**
 * @file audio_buffer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 * This file contains components for working
 * with Buffers that hold audio.
 * We provide some components for converting
 * arbitrary data into different formats.
 */

#pragma once

#include <algorithm>
#include <memory>

#include "dsp/buffer.hpp"

/// A typedef representing an AudioBuffer
using AudioBuffer = Buffer<long double>;

/// Alias for a unique pointer to an AudioBuffer
using BufferPointer = std::unique_ptr<AudioBuffer>;

/**
 * @brief Components that squish and split audio buffers.
 *
 * Audio buffers are great for representing audio information
 * in the MAEC environment.
 *
 * However, if we wish to translate this data to another
 * audio library (i.e, ALSA), these buffers are NOT compatable.
 * Therefore, it is necessary to offer methods to convert AudioBuffers
 * to compatable structures and vice versa.
 *
 * A 'squishier' is a component that takes multiple channels and
 * converts it into a single data stream.
 * For example, audio data with three channels can be squished
 * by representing the data in an interleaved format.
 * Most squishers will take an audio buffer and will return a compatable
 * data structure, such as a vector.
 *
 * A 'splitter' is a component that takes a single data stream and
 * converts it into multiple channels.
 * For example, audio data in interleaved format
 * can be split into multiple separate channels.
 * Most squishers will take a vector and convert it into an AudioBuffer.
 *
 * Squishers and splitters support format conversion,
 * which means that the format of the incoming container (be it long double, float, int, ect)
 * can be converted into another format during the copy operation.
 * This saves us some time, as otherwise we would have do two copy operations.
 * We can just go ahead and get both out of the way in one operation.
 */

/**
 * @brief Converts a given AudioBuffer into a squished interleaved vector
 * 
 * We take the data contained in the audio buffer and squish it.
 * The final vector will be in interleaved format.
 * 
 * We require an iterator that is the destination
 * for this audio data.
 * 
 * @param buff AudioBuffer to work with
 * @param iter Input iterator of the output vector
 */
template <typename It, typename Func>
void squish_inter(AudioBuffer* buff, const It& iter, Func oper) { std::transform(buff->ibegin(), buff->iend(), iter, oper); }

/**
 * @brief Converts a given AudioBuffer into a squished sequential vector
 * 
 * We take the data contained in the audio buffer and squish it.
 * The final vector will be in sequential format.
 * 
 * We require the iterator that is the destination
 * for this audio data.
 * 
 * @param buff AudioBuffer to work with
 * @param iter Input iterator of the output vector
 */
template <typename It, typename Func>
void squish_seq(AudioBuffer* buff, const It& iter, Func oper) {
    std::transform(buff->ibegin(), buff->send(), iter, oper);
}

/**
 * @brief Does nothing!
 *
 * This squishier does nothing!
 * This can be great if you don't want any modules
 * to do squishing operations.
 *
 * @param buff AudioBuffer to ignore
 * @param iter Input iterator to ignore
 */
template <typename It, typename Func>
void squish_null(AudioBuffer* buff, const It& iter, Func oper) {}  // NOLINT(clang-diagnostic-unused-parameter)

/**
 * @brief Components for converting audio data
 * 
 * This section contains components for converting audio data.
 * These are used to convert audio data from one format to another.
 * Most audio libraries do not work well with the internal maec audio format, long double,
 * so it is necessary to provide components for transforming between the two.
 * 
 * It is recommended to use these functions with the squishers/splitters
 * defined above, which will automatically iterate over the provided array
 * and convert it into the necessary data structure.
 * 
 * The naming convention goes like this:
 * 
 * For converting the maec format (mf) into another:
 * 
 * [OTHER] mf_[OTHER](long double val)
 * 
 * For converting other format into mf:
 * 
 * long double [OTHER]_mf([OTHER] val)
 * 
 * Where [OTHER] is the name of the other format to work with.
 * For example, converting mf to int and vice versa:
 * 
 * mf_int(long double val)
 * int_mf(int val)
 * 
 * It is also worth mentioning that 'null' means the function will do nothing.
 * For example, mf_null() will just return what is given.
 */

// mf -> other

float mf_float(long double val);

long double mf_null(long double val);

int16_t mf_int16(long double val);

// other -> mf

long double int16_mf(int16_t val);

template<typename T>
int16_t char_int16(T byts) {

    // Define the type:

    int16_t val = 0;

    // Read the bytes and store in value:

    val = static_cast<int16_t>((static_cast<int16_t>(data[1]) << 8) |
                               static_cast<unsigned char>(data[0]));

    // Return the final value:

    return val;
}
