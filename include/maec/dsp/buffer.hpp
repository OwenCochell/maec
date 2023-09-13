/**
 * @file buffer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Buffers and buffer operations
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 * This header contains buffer classes and operations
 * for representing data to be processed.
 * We include support for multi-dimensional signals,
 * iterating over multi-dimensional data,
 * useful buffer implementations, etc.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "const.hpp"

/**
 * @brief Base class for maec Iterators
 *
 * This class implements some useful operations and features
 * that are required for maec iterators.
 * maec iterators are special components that all maec
 * classes MUST implement and utilize!
 * We implement random access iterators,
 * so all of the operators are implemented and working.
 *
 * Any iterators using this class are STL compatable!
 * You can pass any iterators to STL methods that expect them.
 * One useful application of this is the algorithms library,
 * which defines many operations that are useful for working with arrays and
 * sequences.
 *
 * This is a framework class!
 * It will not work properly on it's own.
 * We use upside down inheritance to add this functionality without virtual
 * functions!
 *
 * @tparam C The class that derives from this class.
 * @tparam T The typename this iterator will iterate over
 */
template <class C, typename T>
class BaseMAECIterator {

   private:
    /// Current pointer we are on
    T* point = nullptr;

    /// The current sample we are on
    int index = 0;

   protected:
    /**
     * @brief Sets the pointer
     *
     * This will set the pointer to the underlying sample.
     * This should be called by the child class when they have a pointer
     * to work with.
     *
     * @param ptr Pointer to current sample
     */
    void set_pointer(T* ptr) { point = ptr; }

   public:
    // Tags for identifying this iterator

    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    /**
     * @brief Sets the index to the given value.
     *
     * This will set the index to the given value,
     * and determine the current pointer.
     * How this is done differs from iterator to iterator.
     * This is done upon setting the index for performance reasons,
     * meaning that the pointer is determined once on index change
     * and is then kept for future use.
     *
     * Please remember, this index is the position in relation
     * to the squished audio data!
     * The squished format differs between iterators.
     *
     * For example, assume the squished audio data is this:
     *
     * 1,2,3,4,5,6,7
     *
     * If you want value '3', then you want index 2.
     *
     * We highly recommend using the helper methods in
     * the child iterator classes to help with seeking
     * so specific values.
     *
     * @param index New index to set
     */
    void set_index(int iindex) {
        this->index = iindex;
        static_cast<C*>(this)->resolve_pointer();
    }

    /**
     * @brief Gets the current index
     *
     * Returns the current index of the iterator.
     * Please keep in mind, the index is in relation
     * to the squished buffer!
     *
     * @return int Current index
     */
    int get_index() const { return this->index; }

    /**
     * @brief Pre-increments the iterator
     *
     * @return C&
     */
    C& operator++() {
        this->set_index(this->get_index() + 1);
        return static_cast<C&>(*this);
    }

    /**
     * @brief Post-increments the iterator
     *
     * @return C
     */
    C operator++(int) {
        C tmp = static_cast<C&>(*this);
        ++(*this);
        return tmp;
    }

    /**
     * @brief Pre-decrements the iterator
     *
     * @return C&
     */
    C& operator--() {
        this->set_index(this->get_index() - 1);
        return static_cast<C&>(*this);
    }

    /**
     * @brief Post-decrements the iterator
     *
     * @return C
     */
    C operator--(int) {
        C tmp = static_cast<C&>(*this);
        --(*this);
        return tmp;
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * @param num Number to add to current index
     * @return C& This iterator
     */
    C& operator+=(const int& num) {
        this->set_index(this->get_index() + num);
        return static_cast<C&>(*this);
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * Set plus operation for unsigned int
     *
     * @param num Number to add to current index
     * @return C& This iterator
     */
    C& operator+=(const unsigned int& num) {
        this->set_index(this->get_index() + num);
        return static_cast<C&>(*this);
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * Set plus operation for fixed size int64_t
     *
     * @param num Number to add to current index
     * @return C& This iterator
     */
    C& operator+=(const int64_t& num) {
        this->set_index(this->get_index() + static_cast<int>(num));
        return static_cast<C&>(*this);
    }

    /**
     * @brief Subtracts the given number from this iterator
     *
     * @param num Number to add to the current index
     * @return C& This iterator
     */
    C& operator-=(const int& num) {
        this->set_index(this->get_index() - num);
        return static_cast<C&>(*this);
    }

    /**
     * @brief Creates a new iterator by adding the given number to our index
     *
     * @param num Number to add to the current index
     * @return C A new iterator with the new index
     */
    C operator+(const int& num) {
        C tmp = static_cast<C&>(*this);
        tmp += num;
        return tmp;
    }

    /**
     * @brief Creates a new iterator by adding the given number to our index
     *
     * Plus operation for unsigned int
     *
     * @param num Number to add to the current index
     * @return C A new iterator with the new index
     */
    C operator+(const unsigned int& num) {
        C tmp = static_cast<C&>(*this);
        tmp += num;
        return tmp;
    }

    /**
     * @brief Creates a new iterator by adding the given number to our index
     *
     * Plus operation for fixed size int64_t
     *
     * @param num Number to add to the current index
     * @return C A new iterator with the new index
     */
    C operator+(const int64_t& num) {
        C tmp = static_cast<C&>(*this);
        tmp += num;
        return tmp;
    }

    /**
     * @brief Creates a new iterator by adding ourselves to the given iterator
     *
     * We simply take the index of the given iterator
     * and add it to ours.
     *
     * @param iter Iterator to add
     * @return C A new iterator with the new index
     */
    C operator+(const C& iter) {
        C tmp = static_cast<C&>(*this);
        tmp += iter.get_index();
        return tmp;
    }

    /**
     * @brief Creates a new iterator by subtracting the given number from our
     * index
     *
     * @param num Number to subtract our index from
     * @return C A new iterator with the new index
     */
    C operator-(const int& num) {
        C tmp = static_cast<C&>(*this);
        tmp -= num;
        return tmp;
    }

    /**
     * @brief Creates a new iterator by subtracting the given iterator from
     * ourselves
     *
     * We simply take the index of the given iterator and subtract it from ours.
     *
     * @param iter Iterator to subtract
     * @return C A new iterator with the new index
     */
    C operator-(const C& iter) {
        C tmp = static_cast<C&>(*this);
        tmp -= iter.get_index();
        return tmp;
    }

    /**
     * @brief Converts this iterator to an integer
     *
     * We simply return our current index.
     *
     * @return int The current index
     */
    operator int() const {  // NOLINT - Casting to other types for iteration impossible if explicit
        return this->get_index();
    }

    /**
     * @brief Determines if the given iterator is equivalent
     *
     * We check if they are pointing to the same index.
     *
     * @param a Iterator A
     * @return true If the two iterators are equivalent
     * @return false If the two iterators are not equivalent
     */
    bool operator==(const C& cmp) {
        return this->get_index() == cmp.get_index();
    }

    /**
     * @brief Determines if the given iterator is not equivalent
     *
     * We check if the iterator is pointing to a different index
     *
     * @param a Iterator to check
     * @return true If the two iterators are not equivalent
     * @return false If the two iterators are equivalent
     */
    bool operator!=(const C& cmp) { return !(*this == cmp); }

    /**
     * @brief Determines if the we are less than the given iterator
     *
     * @param second Iterator to check
     * @return true If we are less than the given iterator
     * @return false If we are equal or greater than the given iterator
     */
    bool operator<(const C& second) {
        return this->get_index() < second.get_index();
    }

    /**
     * @brief Determines if we are greater than the given iterator
     *
     * @param second Iterator to check
     * @return true If we are greater than the given iterator
     * @return false If we are less than or equal to the given iterator
     */
    bool operator>(const C& second) {
        return this->get_index() > second.get_index();
    }

    /**
     * @brief Determines if we are less than or equal to the given iterator
     *
     * @param second Iterator to check
     * @return true If we are less than or equal to the given iterator
     * @return false If we are greater than the given iterator
     */
    bool operator<=(const C& second) {
        return this->get_index() <= second.get_index();
    }

    /**
     * @brief Determines if we are greater than or equal to the given iterator
     *
     * @param second Iterator to check
     * @return true If we are greater than or equal to the given iterator
     * @return false If we are less than or equal to the given iterator
     */
    bool operator>=(const C& second) {
        return this->get_index() >= second.get_index();
    }

    /**
     * @brief Subscripting operator
     *
     * We basically just set the index to the given value,
     * and return the current value.
     *
     * @param val New index to set
     * @return reference Current value
     */
    reference operator[](int val) {
        this->set_index(val);
        return *(this->point);
    }

    /**
     * @brief Gets the current sample
     *
     * @return reference
     */
    reference operator*() const { return *(this->point); }

    /**
     * @brief Gets the current pointer
     *
     * @return pointer
     */
    pointer operator->() const { return this->base(); }

    /**
     * @brief Gets the pointer to the current sample
     *
     * @return pointer
     */
    pointer base() const { return this->point; }
};

/**
 * @brief Class for holding signal data
 *
 * This class allows for arbitrary signal data to be stored,
 * worked with, and recalled for later use.
 *
 * We will support some common buffer operations,
 * and will provide a standardized method for accessing
 * and working with signal data.
 *
 * This class has the ability to represent multi-channel signal data!
 * This can also be interpreted as a collection or rows and columns,
 * with each channel being a row and sample in each channel being a column.
 * Under the hood, this is implemented as a single array.
 * You can access this array if you like,
 * but we also provide methods and iterators to offer easy access into this data.
 * You could also implement this buffer as a type of matrix pretty easily.
 * 
 * Multi channel signals can be represented in a number of ways.
 * Conventionally, it can be represented as a vector of vectors,
 * with each sub-vector representing a channel.
 * We will call this the 'split' format, as each channel is split into its own
 * vector.
 *
 * Suppose we have the following multi-channel signal:
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
 * To access the 2nd sample of the 3rd channel, we would index the vector as
 * follows:
 *
 * data[2][1]
 *
 * Where 'data' is the signal in split format,
 * i.e the vector of vectors.
 *
 * Another method of representing signal data is the 'interleaved' format,
 * where multiple channels are represented (squished) as one stream:
 *
 * [1,4,7,2,5,8,3,6,9]
 *
 * Note that the sample in each channel from the same time
 * is represented before samples from later times.
 * To access the 2nd sample of the 3rd channel, we would index the vector as
 * follows:
 *
 * data[7]
 *
 * Where 'data' is the signal in squished format,
 * i.e the vector of samples.
 * Their are many ways to represent squished signal data!
 * The most common way to represent squished signal data is the
 * interleaved format, where the samples that occur at the same index
 * in each channel are grouped together in the order channels are encountered.
 * The example above is in interleaved format.
 * Another popular format is the sequential format, where the samples
 * are separated by channel:
 *
 * [1,2,3,4,5,6,7,8,9]
 *
 * You can convert between these formats using 'squishers' (for squishing
 * multiple channels into one stream) and splitters (for splitting a single
 * stream into multiple channels). You can find more info on those operations
 * elsewhere in the documentation TODO: Add link once implemented*
 *
 * Sometimes it is useful to access the raw data
 * without any regard to channels.
 * This class offers some methods for accessing the raw signal data
 * without having to worry about the complexity of channels.
 *
 * Some of these iterators are:
 *
 * sequential_iterator - Iterates over each channel sequentially (see below)
 * interlaced_iterator - Iterates over each channel in an interleaved manner (again, see below) 
 * group_iterator - Iterates over each channel and provdes a
 * group of samples from each channel (please, see below)
 *
 * Each iterator can be useful for different reasons and purposes.
 * Please understand the differences between each iterator and how
 * each operation could be useful!
 *
 * Finally, this class contains some info about the signal data itself,
 * specifically the channel count and sample rate.
 * If these are not utilized, these can be ignored or not set.
 *
 * TODO:
 *
 * Function names for iterators are kinda weird?
 * Like why get_position instead of get_sample()?
 *
 * Need to really hash out terminology here.
 *
 * Also implement reverse constant iterators?
 *
 */
template <typename T>
class Buffer {

   public:
    /**
     * @brief An iterator that iterates over signal data sequentially
     * 
     * This iterator iterates over the signal data sequentially,
     * meaning that it gets iterates over all signal data in a channel
     * before moving on to the next channel, in order of channels.
     * 
     * For example, if we have the following signal data:
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
     * You can think of this as the index of the sample in the final squished
     * vector. Fro example, if I wanted to get sample '8' in channel 2 index 1,
     * then I would use index 7 (see squished vector above).
     * 
     * This iterator contains some useful helper methods
     * for determining the index of the iterator,
     * so you don't have to do the calculations yourself.
     * 
     * This iterator is useful if we need to apply the same operation to each
     * channel, and the order of each channel is important, or if we need the
     * 'pure' signal data without data from other channels mixed in.
     */
    template <typename V>
    class SeqIterator : public BaseMAECIterator<SeqIterator<V>, V> {

       public:
        /**
         * @brief Default constructor for this iterator
         *
         * ! WARNING !
         *
         * Using this method will create an iterator
         * with an invalid configuration!
         * THIS WILL LEAD TO BREAKAGE!
         *
         * It is only recommended to use this if you
         * want to create a dummy iterator TO BE OVERRIDEN LATER!
         *
         */
        SeqIterator() =default;

        /**
         * Constructor
         *
         * We need to know the Buffer we are iterating over,
         * and the position to start at.
         *
         * @param buff The Buffer we are iterating over
         * @param pos The index to start at
         */
        explicit SeqIterator(Buffer* buff, int pos = 0) {
            this->buff = buff;
            this->set_index(pos);
        }

        /**
         * @brief Get the Channel we are on
         *
         * This returns the channel we are on.
         *
         * @return int Channel we are on
         */
        int get_channel() const {

#ifdef _DEBUG
            // This section is specific to MSVC debug mode,
            // as without the modulo we get safe iterator errors,
            // causing broken functionality.
            // MSVC release mode and all other compilers don't have an issue
            // with this line
            return static_cast<int>(this->get_index() /
                                    this->buff->size()) %
                   this->buff->channels();
#else
            return static_cast<int>(this->get_index() / this->buff->size());
#endif
        }

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
        void set_channel(int channel) {
            this->set_index(channel * this->buff->size());
        }

        /**
         * @brief Determines the pointer for this iterator
         *
         * This method will automatically determine the correct pointer
         * for this iterator at the current index.
         *
         * This method is automatically called where appropriate,
         * but you can call this method anytime if you wish
         * to re-determine the underlying pointer in use by this iterator.
         *
         * This method keeps the pointer for future use.
         * When the value is requested,
         * we simply return this value.
         * This allows us to determine this value once upon index change
         * and return it many times.
         *
         */
        void resolve_pointer() {
            this->set_pointer(
                this->buff->buff.data() + (this->get_channel() + this->buff->channels() * this->get_sample()));
        }

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
        void set_position(int channel, int sample) {
            this->set_index(channel * this->buff->size() + sample);
        }

        /**
         * @brief Gets the sample of this iterator
         * 
         * As stated above, this value is the location of the sample
         * in relation to the channels and samples within them.
         * 
         * This method will return the sample of the iterator
         * in the current channel.
         * Using the above example, if your index is 7,
         * then this method will return 1, as that is the index
         * of the sample in the channel we are working with.
         * 
         * To get the channel we are working with, use get_channel().
         * This is needed if you want a full understanding of the sample!
         * 
         * @return int The sample of the iterator in the current channel
         */
        int get_sample() const { return static_cast<int>(this->get_index() % this->buff->size()); }

       private:
        /// Audio Buffer we are iterating over
        Buffer<T>* buff = nullptr;
    };

    /**
     * @brief An iterator that iterates over signal data in an interleaved manner
     * 
     * This iterator iterates over signal data in an interleaved manner,
     * meaning that it iterates over each sample in each channel that occurs at
     * the same time before moving on to the next sample.
     * 
     * For example, if we have the following signal:
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
     * You can think of this as the index of the sample in the final squished
     * vector. Fro example, if I wanted to get sample '8' in channel 2 index 1,
     * then I would use index 5 (see squished vector above).
     * 
     * This iterator contains some useful helper methods
     * for determining the index of the iterator,
     * so you don't have to do the calculations yourself.
     * 
     * This iterator is useful if we need to apply the same operation to each
     * channel, and the order of each channel is not important. This format is a
     * very popular format for outputting signal data, as many libraries
     * represent signals in this format.
     * 
     * TODO: See if we should add extra methods, like in SeqIterator...
     */
    template <typename V>
    class InterIterator : public BaseMAECIterator<InterIterator<V>, V> {

       public:
        /**
         * @brief Default constructor for this iterator
         *
         * ! WARNING !
         *
         * Using this method will create an iterator
         * with an invalid configuration!
         * THIS WILL LEAD TO BREAKAGE!
         *
         * It is only recommended to use this if you
         * want to create a dummy iterator TO BE OVERRIDEN LATER!
         *
         */
        InterIterator() =default;

        /**
         * @brief Construct a new Inter Iterator object
         *
         * We need to know the Buffer to iterate over,
         * and the starting position
         *
         * @param buff Buffer we are iterating over
         * @param pos Starting position
         */
        explicit InterIterator(Buffer* buff, int pos = 0) : buff(buff) {
            this->set_index(pos);
        }

        /**
         * @brief Determines the pointer for this iterator
         *
         * This method will automatically determine the correct pointer
         * for this iterator at the current index.
         *
         * This method is automatically called where appropriate,
         * but you can call this method anytime if you wish
         * to re-determine the underlying pointer in use by this iterator.
         *
         * This method keeps the pointer for future use.
         * When the value is requested,
         * we simply return this value.
         * This allows us to determine this value once upon index change
         * and return it many times.
         *
         */
        void resolve_pointer() { this->set_pointer((this->buff->buff.data() + this->get_index())); }

        /**
         * @brief Gets the current channel we are on
         *
         * Gets the current channel number we are on.
         *
         * @return int The current channel
         */
        int get_channel() const {
            return this->get_index() % this->buff->channels();
        }

        /**
         * @brief Get the current sample we are on
         *
         * Gets the current sample we are on.
         *
         * @return int Current sample
         */
        int get_sample() const {
            return static_cast<int>(this->get_index() /
                                    this->buff->channels());
        }

        /**
         * @brief Sets the sample we are on
         * 
         * This will alter the index to the start of the current sample.
         * 
         * It is probably easier to show rather than tell.
         * Consider this signal:
         * 
         * [1]: 1, 2, 3
         * [2]: 4, 5, 6
         * [3]: 7, 8, 9
         * 
         * And here is that data in interleaved format:
         * 
         * 1, 4, 7, 2, 5, 8, 3, 6, 9
         *
         * If you wish to seek to sample 2,
         * then we will set the index to 3.
         * If you wish to seek to sample 3,
         * then we will set the index to 6.
         * 
         * As you can see, each 'section' is the Nth sample
         * of each channel in order.
         * So the 2nd section of the interleaved format will be these values:
         * 
         * 2, 5, 8
         * 
         * Here is the formula for determining this index:
         * 
         * index = sample * channels
         * 
         * Where channels is the number of channels.
         * 
         * @param sample Sample to set this iterator to
         */
        void set_sample(int sample) {
            this->set_index(this->buff->channels() * sample);
        }

        /**
         * @brief Sets the position of this iterator
         *
         * This method will use the given value to determine
         * the position of this iterator.
         * If the index is the location in the squished data,
         * then the position is the location in relation to
         * the channels and the samples within them.
         *
         * You can use this method to seek to any channel and sample
         * in the squished data if you don't want to do the calculations
         * yourself.
         *
         * For example, if we seek to channel 2 sample 2,
         * we will end up at index 4, with value '5'.
         *
         * Here is the formula for determining the index:
         *
         * index = sample * channels + channel
         *
         * Where channels are the number of channels.
         *
         * @param channel Channel to seek to
         * @param sample Sample to seek to
         */
        void set_position(int channel, int sample) {
            this->set_index(this->buff->channels() * sample + channel);
        }

       private:
        /// Buffer we are iterating over
        Buffer<T>* buff = nullptr;
    };

    /**
     * @brief No default constructor!
     * 
     * We require at least the channel number and buffer size be specified
     *
     */
    Buffer() = delete;

    /**
     * @brief Construct a new Audio Buffer object
     *
     * @param size The size of each channel, AKA the number of samples per channel
     * @param channels The number of channels in this buffer, by default 1
     */
    explicit Buffer(int size, int channels = 1) : csize(size), nchannels(channels) {

        // Reserve the buffer with the given info:

        this->reserve();
    }

    /**
     * @brief Construct a new Audio Buffer object
     *
     * We will utilize the given channel count and size,
     * and configure ourselves as necessary.
     * We will set the vector contents as provided,
     * so it is recommended to provide your multi-channel
     * data in interleaved format.
     * 
     * @param vect Vector of samples
     * @param channels Number of channels
     */
    explicit Buffer(const std::vector<T>& vect, int channels = 1) : buff(vect) {

        // First, set the size and channels:

        this->set_channels(channels);
        this->set_size(vect.size() / channels);

        // Next, reserve:

        this->reserve();
    }

    /**
     * @brief Construct a new Audio Buffer object
     *
     * We create an Buffer via the given vector.
     * This vector is two dimensional,
     * meaning that it is a vector of channels
     * that contain samples.
     * Each sub-vector MUST be the same size!
     *
     * We automatically determine the channels number and size this
     * Buffer using the given vector.
     *
     * @param vect Vector of channels
     */
    explicit Buffer(const std::vector<std::vector<T>>& vect) {

        // First, set the number of channels and channel size

        this->set_channels(vect.size());
        this->set_size(vect[0].size());

        // Reserve the data:

        this->reserve();

        // Iterate over the number of samples:

        for (int i = 0; i < this->size(); ++i) {

            // Iterate over the number of channels:

            for (int j = 0; j < this->channels(); ++j) {

                // Push the value to the back of the buffer:

                this->buff.push_back(vect[j][i]);
            }
        }
    }

    /**
     * @brief Set the Sample Rate of this buffer
     *
     * Sets the sample rate for this Buffer.
     *
     * Please note, we will not be doing checks of any kind
     * to ensure this is accurate!
     * Any component using this buffer will most likely
     * use this sample rate at face value.
     * With that being said,
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
     * is accurate to the contained signal.
     *
     * @return double The current samplerate
     */
    double get_samplerate() const { return this->sample_rate; }

    /**
     * @brief Gets the size of this buffer.
     *
     * We report the size of the individual channels,
     * not the number of channels or total size;
     *
     * @return Size of each channel
     */
    std::size_t size() const { return this->csize; }

    /**
     * @brief Sets the size of each individual channel
     *
     * We set the size of the individual channels.
     *
     * @param nsize Size of individual channels
     */
    void set_size(std::size_t nsize) { this->csize = nsize; }

    /**
     * @brief Gets the number of channels in this buffer
     *
     * We report the number of channels,
     * not the channel size or total size.
     *
     * @return Number of channels
     */
    std::size_t channels() const { return this->nchannels; }

    /**
     * @brief Sets the number of channels
     *
     * @param nchannels Number of channels to set
     */
    void set_channels(std::size_t nchannels) { this->nchannels = nchannels; }

    /**
     * @brief Gets the total size of the buffer.
     * 
     * We determine this value by multiplying the number of channels
     * and the channel size.
     * 
     * @return Total buffer size
     */
    std::size_t total_size() const { return this->csize * this->nchannels; }

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
     * We utilize the channel number and channel size for this operation.
     * Usually, this operation is preformed automatically.
     */
    void reserve() { buff.reserve(this->total_size()); }

    /**
     * @brief Shrinks the vector to it's current size
     *
     * This tells the underlying vector to un-allocate
     * it's memory to fit it's current contents.
     * This can save some memory, if we have more memory allocated then values
     * in our buffer.
     *
     * See: https://cplusplus.com/reference/vector/vector/shrink_to_fit/
     * For more info.
     *
     */
    void shrink() { this->buff.shrink_to_fit(); }

    /**
     * @brief Gets the value at the given channel and sample.
     * 
     * This method can be used to get values from the array based upon
     * the row and column.
     * The row can be thought of as the channel,
     * while the column can be thought of the sample in the channel.
     * 
     * @param channel Channel to get value from
     * @param sample Sample to get value from
     * @return Channel at the given channel and sample
     */
    T& at(int channel, int sample) { return this->buff[channel + this->channels() * sample]; }

    /**
     * @brief Gets a value at the given position
     * 
     * @param value Index to get value at
     * @return Value at given position
     */
    T& at(int value) { return this->buff[value]; }

    /**
     * @brief Gets the start sequential iterator for this buffer
     *
     * Returns the start iterator for proper sequential sample iteration.
     * We iterate over each sample in each channel sequentially,
     * meaning that we iterate over each channel in order until we move onto the
     * next. See the documentation for the SeqIterator class for more info.
     *
     * This iterator is useful for iterating over the raw signal,
     * without regard to the underlying channels.
     *
     * @return Buffer::SeqIterator<long double>
     */
    Buffer::SeqIterator<T> sbegin() {
        return Buffer::SeqIterator<T>(this);
    }

    /**
     * @brief Gets the end sequential iterator for this buffer
     *
     * Returns the end iterator for sequential iteration.
     * This is useful for determining when to stop iterating over samples.
     *
     * @return Buffer::SeqIterator<long double>
     */
    Buffer::SeqIterator<T> send() {
        return Buffer::SeqIterator<T>(
            this, this->buff.size());
    }

    /**
     * @brief Gets the start reverse iterator for this buffer
     *
     * Returns the reverse start iterator for proper sequential sample
     * iteration. We iterate over samples sequentially, but in reverse order,
     * meaning that this start iterator will initially point to the last value
     * in this buffer.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    std::reverse_iterator<Buffer::SeqIterator<T>> srbegin() {
        return std::reverse_iterator<Buffer::SeqIterator<T>>(
            Buffer::SeqIterator<T>(this,
                this->total_size()));
    }

    /**
     * @brief Gets the end reverse iterator for this buffer
     *
     * Returns the reverse sequential end iterator for this buffer.
     * Useful for determining when to stop iterating over samples in reverse.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    std::reverse_iterator<Buffer::SeqIterator<T>> srend() {
        return std::reverse_iterator<Buffer::SeqIterator<T>>(
            Buffer::SeqIterator<T>(this));
    }

    /**
     * @brief Gets the constant start sequential iterator for this buffer
     *
     * Returns the constant start iterator for proper sequential sample
     * iteration. We are identical to the normal sequential iterator, but we are
     * marked as constant, so the contents of the buffer can't be altered using
     * this iterator.
     *
     * @return Buffer::SeqIterator<const long double>
     */
    Buffer::SeqIterator<const T> scbegin() {
        return Buffer::SeqIterator<const T>(this);
    }

    /**
     * @brief Gets the end constant sequential iterator for this buffer
     *
     * Returns the constant end iterator for sequential iteration.
     * Useful for determining when to stop iterating over samples.
     *
     * Again, we are constant, so values can't be edited using this iterator.
     *
     * @return Buffer::SeqIterator<const T>
     */
    Buffer::SeqIterator<const T> scend() {
        return Buffer::SeqIterator<const T>(
            this, static_cast<int>(this->total_size()));
    }

    /**
     * @brief Gets the start interleaved iterator for this buffer
     *
     * Returns the start iterator for proper interleaved sample iteration.
     * We iterate over each sample in each channel in an interleaved manner,
     * meaning tht we iterate over each sample that occurs at the same time in
     * order of channels before moving onto the next sample.
     *
     * This iterator is useful for iterating over the raw signal,
     * without regard to the underlying channels.
     *
     * @return Buffer::InterIterator
     */
    Buffer::InterIterator<T> ibegin() {
        return Buffer::InterIterator<T>(this);
    }

    /**
     * @brief Gets the end interleaved iterator for this buffer
     *
     * Returns the end iterator for interleaved iteration.
     * This is useful for determining when to stop iterating over samples.
     *
     * @return Buffer::InterIterator
     */
    Buffer::InterIterator<T> iend() {
        return Buffer::InterIterator<T>(
            this, static_cast<int>(this->buff.size()));
    }

    /**
     * @brief Gets the start reverse sequential iterator for this buffer
     *
     * Returns the reverse start interleaved iterator for this buffer.
     * We iterate over samples in an interleaved manner, but in reverse order,
     * meaning that this iterator will initially point to the last value in the
     * buffer.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    std::reverse_iterator<Buffer::InterIterator<T>> irbegin() {
        return std::reverse_iterator(
            Buffer::InterIterator<T>(this,
                this->total_size()));
    }

    /**
     * @brief Gets the end interleaved reverse iterator for this buffer
     *
     * Returns the end reversed interleaved iterator for this buffer.
     * Useful for determining when to stop iterating.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    std::reverse_iterator<Buffer::InterIterator<T>> irend() { return std::reverse_iterator( Buffer<T>::InterIterator<T>(this)); }

    /**
     * @brief Gets the constant start interleaved iterator for this buffer
     *
     * Returns the start iterator for proper interleaved sample iteration.
     * We are identical to the normal interleaved iterator,
     * but we are marked as constant, so the contents of the buffer can't be
     * altered using this iterator.
     *
     * @return Buffer::InterIterator<const long double>
     */
    Buffer::InterIterator<const T> icbegin() {
        return Buffer::InterIterator<const T>(this); }

    /**
     * @brief Gets the end constant iterator for this buffer
     *
     * Returns the constant end iterator for interleaved iteration.
     * Useful for determining when to stop iterating over samples.
     *
     * Again, we are constant, so values can't be edited using this iterator.
     *
     * @return Buffer::InterIterator<const long double>
     */
    Buffer::InterIterator<const T> icend() {
        return Buffer::InterIterator<const T>(this, this->total_size()); }

    /**
     * @brief Default start iterator
     * 
     * The default start iterator simply returns an
     * InterIterator.
     * 
     * @return InterIterator
     */
    Buffer::InterIterator<T> begin() {
        return this->ibegin();
    }

    /**
     * @brief Default stop iterator
     * 
     * The default stop iterator simply returns
     * an InterIterator.
     * 
     * @return InterIterator 
     */
    Buffer::InterIterator<T> end() {
        return this->iend();
    }

   private:
    /// Sample rate in Hertz
    double sample_rate = SAMPLE_RATE;

    /// The underlying vector of audio data
    std::vector<T> buff;

    /// Size of each channel
    int csize = 0;

    /// Number of channels in this buffer
    int nchannels = 0;

    /// Various friend defintions
    friend class Buffer::SeqIterator<T>;
    friend class Buffer::SeqIterator<const T>;
    friend class Buffer::InterIterator<T>;
    friend class Buffer::InterIterator<const T>;
};

/**
 * @brief A ring buffer for storing arbitrary data.
 *
 * A ring buffer (AKA circular buffer) is a data structure
 * that allows for data to be stored as if it where stored end-to-end.
 * If the array pointer reaches the end of the buffer,
 * then it wraps back around to the start of the buffer.
 * For example, consider this sequence:
 *
 * 1, 2, 3, 4, 5, 6
 * ^
 *
 * And assume the pointer is at the start of the buffer.
 * If we advance the pointer, it will iterate through the buffer as excepted:
 * However, if the pointer is at the end of the buffer:
 *
 * 1, 2, 3, 4, 5, 6
 *                ^
 * Then the next advance will wrap around to the start of the buffer:
 *
 * 1, 2, 3, 4, 5, 6
 * ^
 *
 * See here for more info:
 * https://en.wikipedia.org/wiki/Circular_buffer
 *
 * This can be useful for many applications!
 * We use this class a bit throughout the library.
 * This class implements MAEC iterators,
 * along with some helper functions for seeking,
 * advancing/rewinding, setting/retrieving values, etc.
 */
template <typename T>
class RingBuffer {

   protected:
    /**
     * @brief Normalizes the given index to be within the bounds of the buffer
     *
     * We determine the given index falls within the bounds of the buffer.
     * We utilize the modulo operator to wrap the index around to the start of
     * the buffer.
     *
     * @param nindex Index to normalize
     * @return Normalized index
     */
    int normalize_index(int nindex) const { return nindex % this->size(); }

   public:
    /**
     * @brief An iterator that iterators over the ring buffer.
     *
     * As expected with ring buffers, when we reach the end of the buffer,
     * we wrap around to the start of the buffer.
     * This allows you to iterate over a buffer forever!
     * We will continue to loop if you keep advancing the iterator.
     *
     * @tparam T Type of data to iterate over
     */
    class RingIterator
        : public BaseMAECIterator<RingBuffer<T>::RingIterator, T> {

       public:
        /**
         * @brief Default constructor for this iterator
         *
         * ! WARNING !
         *
         * Using this method will create an iterator
         * with an invalid configuration!
         * THIS WILL LEAD TO BREAKAGE!
         *
         * It is only recommended to use this if you
         * want to create a dummy iterator TO BE OVERRIDEN LATER!
         *
         */
        RingIterator() = default;

        /**
         * @brief Construct a new RingIterator object
         *
         * We need to know the RingBuffer to iterate over,
         * and the starting position.
         *
         * @param buff RingBuffer we are iterating over
         * @param pos Starting position
         */
        explicit RingIterator(RingBuffer<T>* nbuff, int pos = 0) : buff(nbuff) {
            this->set_index(pos);
        }

        /**
         * @brief Determines the value we are pointing to
         *
         * We simply normalize the given index and determine the pointer to work
         * with. This method is called automatically where necessary.
         *
         */
        void resolve_pointer() {
            this->set_pointer(this->buff->buff.data() +
                              this->buff->normalize_index(this->get_index()));
        }

       private:
        /// The buffer we are iterating over
        RingBuffer<T>* buff = nullptr;
    };

    /**
     * @brief Default constructor
     *
     */
    RingBuffer() = default;

    /**
     * @brief Construct a new RingBuffer object
     *
     * This constructor allows you to specify the size of the buffer,
     * and we will automatically configure ourselves to use that size.
     *
     * @param nsize Size to use for this buffer
     */
    explicit RingBuffer(int nsize) { this->reserve(nsize); }

    /**
     * @brief Constructs a new RingBuffer object using existing data
     *
     * This constructor creates a new RingBuffer using data that already exists.
     * Simply pass a vector of data to this constructor,
     * and we will create a new RingBuffer with that data.
     *
     * @param other Data to use for this buffer
     */
    explicit RingBuffer(const std::vector<T>& other) : buff(other) {
        this->bsize = other.size();
    }

    /**
     * @brief Retrieves the size of this buffer
     *
     * @return int Size of this buffer
     */
    int size() const { return this->bsize; }

    /**
     * @brief Reserves the given size for this buffer
     *
     * We ensure the underlying vector has enough space for the given size.
     * We also keep the size of the buffer for determining any indexes.
     *
     * @param nsize New size of circular buffer
     */
    void reserve(int nsize) {
        this->buff.reserve(nsize);
        this->bsize = nsize;
    }

    /**
     * @brief Gets the start iterator for the ring buffer
     *
     * You can treat this iterator like any other iterator,
     * and can pass it to STD algorithms!
     *
     * @return Start iterator
     */
    RingBuffer::RingIterator begin() {
        return RingBuffer::RingIterator(this, 0);
    }

    /**
     * @brief Gets the end iterator for the ring buffer
     *
     * You can use this iterator to determine when you have reached the end of
     * the buffer.
     *
     * @return RingBuffer::RingIterator<T> Stop iterator
     */
    RingBuffer::RingIterator end() {
        return RingBuffer::RingIterator(this, this->size());
    }

    /**
     * @brief Retrieves the value at the current index
     *
     * @param i Index to retrieve
     * @return T& Value at index
     */
    T& operator[](int nindex) {
        return this->buff[this->normalize_index(nindex)];
    }

   private:
    /// Size of this ring buffer
    int bsize = 0;

    /// The underlying buffer
    std::vector<T> buff;

    /// Friend definition for iterators
    friend class RingIterator;
};
