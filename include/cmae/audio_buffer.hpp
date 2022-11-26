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
 * Another method of representing audio data is the 'interleaved' format,
 * where multiple channels are represented (squished) as one stream:
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
 * Another popular format is the sequential format, where the samples
 * are separated by channel:
 * 
 * [1,2,3,4,5,6,7,8,9]
 * 
 * Squished format is popular when outputting audio data,
 * be it a speaker, file, or other method.
 * However, this method can be difficult to work with,
 * as it is not immediately clear which samples belong to which channel.
 * Therefore, it is not recommended to use this format when working with audio data in the chain.
 * 
 * You can convert between these formats using 'squishers' (for squishing multiple channels into one stream)
 * and splitters (for splitting a single stream into multiple channels).
 * You can find more info on those operations elsewhere in the documentation TODO: Add link once implemented
 * 
 * By default, we keep our audio data in split format.
 * The size of the channels MUST be the same!
 * Otherwise, we will run into lot's of problems.
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

        /// Sample rate in Hertz
        double sample_rate = 4400;

    protected:

        /// The underlying vector of audio data
        std::vector<AudioChannel> buff;

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
         * [0] - 1, 2, 3,
         * [1] - 4, 5, 6,
         * [2] - 7, 8, 9,
         * 
         * Where [n] represents the nth channel, and the numbers
         * are the samples in each channel.
         * 
         * This iterator will return the following values:
         * 
         * 1, 2, 3, 4, 5, 6, 7, 8, 9
         * 
         * We talk about the index of this iterator in the documentation.
         * This iterator uses that value to determine which sample to return.
         * You can think of this as the index of the sample in the final squished vector.
         * Fro example, if I wanted to get sample '8' in channel 2 index 1,
         * then I would use index 7 (see squished vector above).
         * 
         * This iterator contains some useful helper methods
         * for determining the index of the iterator,
         * so you don't have to do the calculations yourself.
         * 
         * This iterator is useful if we need to apply the same operation to each channel,
         * and the order of each channel is important, or if we need the 'pure' audio data
         * without data from other channels mixed in.
         */
        class SeqIterator {

            public:

                /**
                 * Constructor
                 * 
                 * We need to know the AudioBuffer we are iterating over,
                 * and the position to start at.
                 * 
                 * @param buff The AudioBuffer we are iterating over
                 * @param pos The index to start at
                */
                SeqIterator(AudioBuffer *buff, int pos=0) { this->buff = buff; this->sample = pos; };

                /**
                 * @brief Returns the current sample
                 * 
                 * We do some operations to determine the current sample:
                 * 
                 * TODO: Fill this in here
                 * 
                 * @return long double The current sample
                 */
                long double operator *() const;

                /**
                 * @brief Increments the iterator
                 * 
                 * We increment the iterator by one sample.
                 * If we are at the end of the current channel,
                 * we move on to the next channel.
                 * 
                 * @return SeqIterator& A reference to the iterator
                 */
                const SeqIterator& operator++() {  sample++; return *this; }

                /**
                 * @brief Get the Channel we are on
                 * 
                 * This returns the channel we are on.
                 * 
                 * @return int Channel we are on
                 */
                int get_channel() const;

                /**
                 * @brief Set the Channel we are on
                 * 
                 * This will alter the index of the iterator
                 * to the beginning of the specified channel.
                 * 
                 * This is bi-directional!
                 * We can move to any channel in any direction at any time.
                 * 
                 * @param channel Channel to move to.
                 */
                void set_channel(int channel);

                /**
                 * @brief Get the current position
                 * 
                 * This returns the current index of the iterator,
                 * which is used to determine the current sample.
                 * 
                 * @return int The current index
                 */
                int get_index() const { return this->sample; }

                /**
                 * @brief Set the current index
                 * 
                 * This will alter the index of the iterator
                 * to a new sample.
                 * 
                 * Again, please note that the index is in relation to the squished vector!
                 * You will have to take into account the size and number of channels to determine 
                 * the sample you want.
                 * 
                 * It is recommended to use the helper methods to determine the index
                 * instead of doing the calculations yourself.
                 * 
                 * @param pos Position to set the index to
                 */
                void set_index(int pos) { this->sample = pos; }

                /**
                 * @brief Sets the position of this iterator
                 * 
                 * This method will use the given values to set the index
                 * to the given position.
                 * The index if the sample in the squished value,
                 * while the position is the location of the sample in relation
                 * to the channels and samples within them.
                 * 
                 * Using the example above, if our index is 7,
                 * then our position is channel 2, sample 1.
                 * 
                 * This method can set the index to the given position,
                 * which can be useful if you wish to jump to a specific place
                 * and don't want to do the calculations yourself.
                 * 
                 * Here is the equation for determining the index:
                 * 
                 * index = (channel * size) + sample
                 * 
                 * Where 'size' is the size of each channel.
                 * 
                 * @param channel Channel of the position
                 * @param sample Index of the position within the given channel
                 */
                void set_position(int channel, int sample);

                /**
                 * @brief Gets the position of this iterator
                 * 
                 * As stated above, the position is the location of the sample
                 * in relation to the channels and samples within them.
                 * 
                 * This method will return the position of the iterator 
                 * in the current channel.
                 * Using the above example, if your index is 7,
                 * then this method will return 1, as that is the index
                 * of the sample in the channel we are working with.
                 * 
                 * To get the channel we are working with, use GetChannel().
                 * This is needed if you want a full understanding of the position!
                 * 
                 * TODO: Is this too complicated?
                 * Maybe return a pair with channel and sample instead?
                 * Calling another function to get the channel might be too much.
                 * 
                 * @return int The position of the iterator in the current channel
                 */
                int get_position() const;

            private:

                /// Current sample we are on
                int sample=0;

                /// Audio Buffer we are iterating over
                AudioBuffer* buff;

        };

        /**
         * @brief An iterator that iterates over audio data in an interleaved manner
         * 
         * This iterator iterates over audio data in an interleaved manner,
         * meaning that it iterates over each sample in each channel that occurs at the same time
         * before moving on to the next sample.
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
         * 1, 4, 7, 2, 5, 8, 3, 6, 9
         * 
         * We talk about the index of this iterator in the documentation.
         * This iterator uses that value to determine which sample to return.
         * You can think of this as the index of the sample in the final squished vector.
         * Fro example, if I wanted to get sample '8' in channel 2 index 1,
         * then I would use position 5 (see squished vector above).
         * 
         * This iterator contains some useful helper methods
         * for determining the index of the iterator,
         * so you don't have to do the calculations yourself.
         * 
         * This iterator is useful if we need to apply the same operation to each channel,
         * and the order of each channel is not important.
         * This format is a very popular format for outputting audio data,
         * as many libraries represent audio data in this format.
         * 
         * TODO: See if we should add extra methods, like in SeqIterator...
         */
        class InterIterator {

            public:

                /**
                 * @brief Construct a new Inter Iterator object
                 * 
                 * We need to know the AudioBuffer to iterate over,
                 * and the starting position
                 * 
                 * @param buff AudioBuffer we are iterating over
                 * @param pos Starting position
                 */
                InterIterator(AudioBuffer *buff, int pos=0) { this->buff=buff; this->sample=pos; }

                /**
                 * @brief Returns the current sample
                 * 
                 * We do some operations to determine the current sample:
                 * 
                 * TODO: Fill this in here
                 * 
                 * @return long double The current sample
                 */
                long double operator *() const;

                /**
                 * @brief Increments the iterator
                 * 
                 * We move on to the next sample.
                 * If we have iterated over each sample
                 * in each channel that occurs at the same time,
                 * then we move on to the next sample.
                 * 
                 * @return const SeqIterator& 
                 */
                const InterIterator& operator++() {  this->sample++; return *this; }

                /**
                 * @brief Gets the index of this iterator
                 * 
                 * This returns the current index of the iterator,
                 * which is used to retrieve each sample.
                 * 
                 * @return int Current index of this iterator
                 */
                int get_index() { return this->sample; }

                /**
                 * @brief Sets the index of this iterator
                 * 
                 * Sets the index to the given value.
                 * 
                 * Again, please note that the index is in relation to the squished vector!
                 * You will have to take into account the size and number of channels to determine 
                 * the sample you want.
                 * 
                 * It is recommended to use the helper methods to determine the index
                 * instead of doing the calculations yourself.
                 * 
                 * @param pos Position to set the iterator to
                 */
                void set_index(int pos) { this->sample = pos; }

            private:

                /// The current sample we are on
                int sample = 0;

                /// Buffer we are iterating over
                AudioBuffer *buff;

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
         * @brief Gets a channel in this buffer
         * 
         * Returns a channel in this buffer for use.
         * You can alter and edit this channel as you see fit,
         * but please remember the size rule!
         * 
         * @param pos Channel number to retrieve
         * @return AudioChannel AudioChannel at the given position
         */
        AudioChannel* at(int pos) { return &(this->buff.at(pos)); }

        /**
         * @brief Returns the number of channels in this buffer
         * 
         * @return int Number of channels
         */
        int get_channel_count() { return this->buff.size(); }

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

        /**
         * @brief Gets the start sequential iterator of this buffer
         * 
         * Returns the start iterator for proper sequential sample iteration.
         * We iterate over each sample in each channel sequentially,
         * meaning that we iterate over each channel in order until we move onto the next.
         * See the documentation for the SeqIterator class for more info.
         * 
         * This iterator is useful for iterating over the raw audio data,
         * without regard to the underlying channels.
         * 
         * @return AudioBuffer::SeqIterator 
         */
        AudioBuffer::SeqIterator sbegin() { return AudioBuffer::SeqIterator(this); }

        /**
         * @brief Gets the end sequential iterator of this buffer
         * 
         * Returns the end iterator for sequential iteration.
         * This is useful for determining when to stop iterating over samples.
         * 
         * @return AudioBuffer::SeqIterator 
         */
        AudioBuffer::SeqIterator send() { return AudioBuffer::SeqIterator(this, this->buff[0].size() * this->buff.size()); }

        /**
         * @brief Gets the start interleaved iterator of this buffer
         * 
         * Returns the start iterator for proper interleaved sample iteration.
         * We iterate over each sample in each channel in an interleaved manner,
         * meaning tht we iterate over each sample that occurs at the same time in order
         * of channels before moving onto the next sample.
         * 
         * This iterator is useful for iterating over the raw audio data,
         * without regard to the underlying channels.
         * 
         * @return AudioBuffer::InterIterator 
         */
        AudioBuffer::InterIterator ibegin() { return AudioBuffer::InterIterator(this); }

        /**
         * @brief Gets the end interleaved iterator of this buffer
         * 
         * Returns the end iterator for interleaved iteration.
         * This is useful for determining when to stop iterating over samples.
         * 
         * @return AudioBuffer::InterIterator 
         */
        AudioBuffer::InterIterator iend() { return AudioBuffer::InterIterator(this, this->buff[0].size() * this->buff.size()); }

};
