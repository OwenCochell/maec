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

#include <utility>
#include <vector>
#include <iterator>
#include <algorithm>
#include <memory>

#include "const.hpp"
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
template<typename It, typename Func>
void squish_null(AudioBuffer* buff, const It& iter, Func oper) {}

float mf_float(long double val);

long double mf_null(long double val);
