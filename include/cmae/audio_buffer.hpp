/**
 * @file audio_buffer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <vector>

/// A Typedef representing a buffer of audio samples.
typedef std::vector<long double> AudioChannel;

/**
 * @brief Class for holding audio data
 * 
 * This class allows for arbitrary audio data to be stored,
 * worked with, and recalled for later use.
 * 
 * We will support some common buffer operations,
 * and will provide a standardized method for accessing
 * and working with audio data.
 * TODO: Add more once implemented
 * 
 * Please note, that this class contains vectors that represents different channels
 * of audio data, allowing for multi-channel audio to be stored and worked with.
 * We try to encapsulate this data as much as possible, and provide easy entry points into it.
 * These audio channels are simply vectors of long doubles.
 * 
 * Multi channel audio can be represented in a number of ways.
 * Conventionally, it can be represented as a vector of vectors,
 * with each sub-vector representing a channel.
 * We will call this the 'split' format, as each channel is split into its own vector.
 * 
 * Suppose we have the following audio data:
 * 
 * [1] - 1,2,3
 * [2] - 4,5,6
 * [3] - 7,8,9
 * 
 * Where [n] is the nth channel, and the numbers are the samples.
 * Each channel is concurrent, meaning that each sample is encountered 
 * in each channel at the same time.
 * To represent this data using in a split format, we would have:
 * 
 * [
 *  [1,2,3],
 *  [4,5,6],
 *  [7,8,9]
 * ]
 * 
 * Where each [] represents a vector. Again, numbers are samples.
 * To access the 2nd sample of the 3rd channel, we would index the vector as follows:
 * 
 * data[2][1]
 * 
 * Where 'data' is the audio in split format,
 * i.e the vector of vectors.
 * 
 * Another method of representing audio data is the 'squished' format,
 * where multiple channels are represented as one stream:
 * 
 * [1,4,7,2,5,8,3,6,9]
 * 
 * Note that the sample in each channel from the same time
 * is represented before samples from later times.
 * To access the 2nd sample of the 3rd channel, we would index the vector as follows:
 * 
 * data[7]
 * 
 * Where 'data' is the audio in squished format,
 * i.e the vector of samples.
 * Their are many ways to represent squished audio data!
 * The most common way to represent squished audio data is the
 * interleaved format, where the samples that occur at the same index
 * in each channel are grouped together in the order channels are encountered.
 * The example above is in interleaved format.
 * Another popular format is the contiguous format, where the samples
 * are separated by channel:
 * 
 * [1,2,3,4,5,6,7,8,9]
 * 
 * This format is popular when outputting audio data,
 * be it a speaker, file, or other method.
 * However, this method can be difficult to work with,
 * as it is not immediately clear which samples belong to which channel.
 * Therefore, it is not recommended to use this format when working with audio data in the chain.
 * 
 * You can convert between these formats using 'squishers' (for squishing multiple channels into one stream)
 * and splitters (for splitting a single stream into multiple channels).
 * You can find more info on those operations elsewhere in the documentation TODO: Add link once implemented
 * 
 * Sometimes it is useful to access the raw data
 * without any regard to channels.
 * This class offers some methods for accessing the raw audio data
 * without having to worry about the complexity of channels.
 * 
 * Some of these iterators are:
 * 
 * sequential_iterator - Iterates over each channel sequentially (see below)
 * interlaced_iterator - Iterates over each channel in an interleaved manner (again, see below)
 * group_iterator - Iterates over each channel and provdes a group of samples from each channel (please, see below)
 * 
 * Each iterator can be useful for different reasons and purposes.
 * Please understand the differences between each iterator and how
 * each operation could be useful!
 * 
 * Finally, this class contains some info about the audio data itself,
 * such as sample rate, format (TODO: Figure this out),
 * number of samples, weather we are interlaced, ect.
 * 
 */
class AudioBuffer {

    private:

        /// The underlying vector of audio data
        std::vector<AudioChannel> buff;

        /// Sample rate in Hertz
        double sample_rate = 4400;

        /// Boolean determining if we are interlaced
        bool interlaced = false;

    public:

        /**
         * @brief An iterator that iterates over audio data sequentially
         * 
         * This iterator iterates over the audio data sequentially,
         * meaning that it gets iterates over all audio data in a channel 
         * before moving on to the next channel, in order of channels.
         * 
         * For example, if we have the following audio data:
         * 
         * [1] - 1, 2, 3,
         * [2] - 4, 5, 6,
         * [3] - 7, 8, 9,
         * 
         * Where [n] represents the nth channel, and the numbers
         * are the samples in each channel.
         * 
         * This iterator will return the following values:
         * 
         * 1, 2, 3, 4, 5, 6, 7, 8, 9
         * 
         * This iterator is useful if we need to apply the same operation to each channel,
         * and the order of each channel is important, or if we need the 'pure' audio data
         * without data from other channels mixed in.
         */
        class seq_value_iterator {

        };

        /**
         * @brief Construct a new Audio Buffer object
         * 
         */
        AudioBuffer() {}

        /**
         * @brief Construct a new Audio Buffer object and preallocates it
         * 
         * @param size Size of the buffer
         */
        AudioBuffer(int size);

        /**
         * @brief Set the Sample Rate of this buffer
         * 
         * Sets the sample rate for this AudioBuffer.
         * 
         * Please note, we will not be doing checks of any kind
         * to ensure this is accurate!
         * Any component using this buffer will most likely 
         * use this sample rate at face value.
         * With tha t being said,
         * please only set the sample rate to accurate values.
         * If your component has nothing to do with
         * resampling, then please do not change this value.
         * 
         * @param rate New sample rate to set
         */
        void set_samplerate(double rate) { this->sample_rate = rate; }

        /**
         * @brief Get the samplerate of this buffer
         * 
         * Returns the reported sample rate of this buffer.
         * It can be assumed that the reported sample rate 
         * is accurate to the audio buffer.
         * 
         * @return double The current samplerate
         */
        double get_samplerate() { return this->sample_rate; }

        /**
         * @brief Pre-allocates the buffer to a certain size
         * 
         * This tells the underlying vector to allocate memory
         * before hand, which leads to faster performance
         * when writing (and reading, but to a lesser degree...),
         * so it is recommended to allocate the vector before any operations.
         * 
         * See: https://cplusplus.com/reference/vector/vector/reserve/
         * For more info.
         * 
         * @param size Size to pre-allocate
         */
        void reserve(int size);

        /**
         * @brief Shrinks the vector to it's current size
         * 
         * This tells the underlying vector to un-allocate 
         * it's memory to fit it's current contents.
         * This can save some memory, if we have more memory allocated then values in our buffer.
         * 
         * See: https://cplusplus.com/reference/vector/vector/shrink_to_fit/
         * For more info.
         * 
         */
        void shrink();



        /**
         * @brief Gets the start channel iterator of this buffer
         * 
         * Returns the start iterator for proper channel iteration.
         * A channel iterator is an iterator pointing to an AudioChannel type,
         * which is a vector of long doubles.
         * 
         * If you need access to each channel for iteration, 
         * then this iterator is for you!
         * However, if you wish to access the raw audio data
         * without any regard to the underlying channels,
         * then you probably want to use the TODO: Fill in methods once written!
         * 
         * @return std::vector<long double>::iterator 
         */
        std::vector<AudioChannel>::iterator chbegin() { return this->buff.begin(); }

        /**
         * @brief Gets the end channel iterator of this buffer
         * 
         * Returns the end iterator for proper channel iteration.
         * This is useful for determining when to stop iterating over channels.
         * 
         * As stated above, this iterator is for iterating over audio channels!
         * If you wish to work with audio data without regard to channels,
         * then this is not the iterator for you!
         * 
         * @return std::vector<long double>::iterator 
         */
        std::vector<AudioChannel>::iterator chend() { return this->buff.end(); }

};
