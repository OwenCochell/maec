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

#include <array>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <vector>

#include "const.hpp"
#include "util.hpp"

/**
 * @brief Base class for maec Iterators
 *
 * This class implements some useful operations and features
 * that are required for maec iterators.
 * maec iterators are special components that all maec
 * classes MUST implement and utilize!
 * We implement random access iterators,
 * so all of the operators are implemented and working.
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
 * @tparam I The class that derives from this class.
 * @tparam T The typename this iterator will iterate over
 */
template <class I, typename T, bool IsConst = false>
class BaseMAECIterator {
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

    using iterator_type = typename ChooseType<IsConst, const I, I>::type;

    using value_type = typename ChooseType<IsConst, const T, T>::type;
    using pointer = value_type*;
    using reference = value_type&;

    BaseMAECIterator() = default;

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

        this->point =
            static_cast<iterator_type*>(this)->resolve_pointer(this->index);
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
     * @return iterator_type&
     */
    iterator_type& operator++() {
        this->set_index(this->get_index() + 1);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Post-increments the iterator
     *
     * @return iterator_type
     */
    iterator_type operator++(int) {
        iterator_type tmp = static_cast<iterator_type&>(*this);
        ++(*this);
        return tmp;
    }

    /**
     * @brief Pre-decrements the iterator
     *
     * @return iterator_type&
     */
    iterator_type& operator--() {
        this->set_index(this->get_index() - 1);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Post-decrements the iterator
     *
     * @return iterator_type
     */
    iterator_type operator--(int) {
        iterator_type tmp = static_cast<iterator_type&>(*this);
        --(*this);
        return tmp;
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * @param num Number to add to current index
     * @return iterator_type& This iterator
     */
    iterator_type& operator+=(const int& num) {
        this->set_index(this->get_index() + num);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * Set plus operation for unsigned int
     *
     * @param num Number to add to current index
     * @return iterator_type& This iterator
     */
    iterator_type& operator+=(const unsigned int& num) {
        this->set_index(this->get_index() + num);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * Set plus operation for unsigned int
     *
     * @param num Number to add to current index
     * @return iterator_type& This iterator
     */
    iterator_type& operator+=(
        const long unsigned int& num) {  // NOLINT(google-runtime-int)
        this->set_index(this->get_index() + num);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Adds the given number to this iterator
     *
     * Set plus operation for fixed size int64_t
     *
     * @param num Number to add to current index
     * @return iterator_type& This iterator
     */
    iterator_type& operator+=(const int64_t& num) {
        this->set_index(this->get_index() + static_cast<int>(num));
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Subtracts the given number from this iterator
     *
     * @param num Number to add to the current index
     * @return C& This iterator
     */
    iterator_type& operator-=(const int& num) {
        this->set_index(this->get_index() - num);
        return static_cast<iterator_type&>(*this);
    }

    /**
     * @brief Creates a new iterator by adding the given number to our index
     *
     * @param num Number to add to the current index
     * @return C A new iterator with the new index
     */
    iterator_type operator+(const int& num) const {

        // Make a non-const copy:
        I tmp = static_cast<const I&>(*this);

        // Increment value m.knj:
        tmp += num;

        // Return value:
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
    iterator_type operator+(const unsigned int& num) const {
        iterator_type tmp = static_cast<const iterator_type&>(*this);
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
    iterator_type operator+(const int64_t& num) const {
        iterator_type tmp = static_cast<const iterator_type&>(*this);
        tmp += num;
        return tmp;
    }

    /**
     * @brief Creates a new iterator by adding the given number to our index
     *
     * Plus operator for long unsigned int
     *
     * @param num Number to add to the current index
     * @return A new iterator with the new index
     */
    iterator_type operator+(
        const long unsigned int& num) const {  // NOLINT(google-runtime-int)
        iterator_type tmp = static_cast<const iterator_type&>(*this);
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
    iterator_type operator+(const I& iter) const {
        iterator_type tmp = static_cast<const iterator_type&>(*this);
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
    iterator_type operator-(const int& num) const {

        // Make non-const copy:
        I tmp = static_cast<const I&>(*this);

        // Make changes:
        tmp -= num;

        // Return copy:
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
    iterator_type operator-(const iterator_type& iter) const {
        iterator_type tmp = static_cast<const iterator_type&>(*this);
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
    operator int() const {  // NOLINT - Casting to other types for iteration
                            // impossible if explicit
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
    bool operator==(const iterator_type& cmp) const {
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
    bool operator!=(const iterator_type& cmp) const { return !(*this == cmp); }

    /**
     * @brief Determines if the we are less than the given iterator
     *
     * @param second Iterator to check
     * @return true If we are less than the given iterator
     * @return false If we are equal or greater than the given iterator
     */
    bool operator<(const iterator_type& second) const {
        return this->get_index() < second.get_index();
    }

    /**
     * @brief Determines if we are greater than the given iterator
     *
     * @param second Iterator to check
     * @return true If we are greater than the given iterator
     * @return false If we are less than or equal to the given iterator
     */
    bool operator>(const iterator_type& second) const {
        return this->get_index() > second.get_index();
    }

    /**
     * @brief Determines if we are less than or equal to the given iterator
     *
     * @param second Iterator to check
     * @return true If we are less than or equal to the given iterator
     * @return false If we are greater than the given iterator
     */
    bool operator<=(const iterator_type& second) const {
        return this->get_index() <= second.get_index();
    }

    /**
     * @brief Determines if we are greater than or equal to the given iterator
     *
     * @param second Iterator to check
     * @return true If we are greater than or equal to the given iterator
     * @return false If we are less than or equal to the given iterator
     */
    bool operator>=(const iterator_type& second) const {
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
    reference operator[](int val) const {

        return *(static_cast<iterator_type*>(this)->resolve_pointer(val));
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

        return *(static_cast<iterator_type*>(this)->resolve_pointer(val));
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
    pointer base() const { return &(this->point); }

private:
    /// Current pointer we are on
    pointer point = nullptr;

    /// The current sample we are on
    int index = 0;
};

/**
 * @brief A concepts that defines valid container to be used
 *
 * We require the following features for any containers used
 * in maec buffers.
 *
 * @tparam T Type to check
 */
template <typename T>
concept BufferContainer = requires(T typ) {
    typ[0];       // Values can be accessed through subscripting
    typ.size();   // Has a size function
    typ.begin();  // Can start iteration
    typ.end();    // Can stop iteration
    typ.data();   // Access underlying data
};

/**
 *
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
 * but we also provide methods and iterators to offer easy access into this
 * data. You could also implement this buffer as a type of matrix pretty easily.
 *
 * This buffer offers methods to determine the capacity of the vector
 * if every channel and sample was filled in.
 * It is recommended to NEVER iterate over a buffer that is only
 * partially full, or if one channel has more samples than another.
 * This will lead to a lot of problems! Please avoid this scenario.
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
 * interlaced_iterator - Iterates over each channel in an interleaved manner
 * (again, see below) group_iterator - Iterates over each channel and provdes a
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
template <BufferContainer B, typename T = B::value_type>
class BaseBuffer {
public:
    ///
    // Some traits for this container
    ///

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using container = B;  /// Container this class utilizes

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
     *
     * You can think of this iterator as iterating over the rows of the audio
     * data matrix.
     */
    template <bool IsConst = false>
    class SeqIterator
        : public BaseMAECIterator<BaseBuffer::SeqIterator<IsConst>, T,
                                  IsConst> {

    public:
        using BufferType = typename ChooseType<IsConst, const BaseBuffer<B, T>,
                                               BaseBuffer<B, T>>::type;
        using reference =
            typename BaseMAECIterator<BaseBuffer::SeqIterator<IsConst>, T,
                                      IsConst>::reference;
        using pointer =
            typename BaseMAECIterator<BaseBuffer::SeqIterator<IsConst>, T,
                                      IsConst>::pointer;

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
        SeqIterator() = default;

        /**
         * Constructor
         *
         * We need to know the Buffer we are iterating over,
         * and the position to start at.
         *
         * @param buff The Buffer we are iterating over
         * @param pos The index to start at
         */
        explicit SeqIterator(BufferType* buff, int pos = 0) : buff(buff) {
            this->set_index(pos);
        }

        /**
         * @brief Get the Channel we are on
         *
         * This returns the channel we are on.
         *
         * @return int Channel we are on
         */
        int get_channel() const { return this->get_channel(this->get_index()); }

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
            this->set_index(channel * this->buff->channel_capacity());
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
        pointer resolve_pointer(int index) const {
            return this->buff->buff.data() +
                   (this->get_channel(index) +
                    this->buff->channels() * this->get_sample(index));
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
            this->set_index((channel * this->buff->channel_capacity()) +
                            sample);
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
        int get_sample() const { return this->get_sample(this->get_index()); }

    private:
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
        int get_sample(int index) const {
            return static_cast<int>(index % this->buff->channel_capacity());
        }

        int get_channel(int index) const {
#ifdef _DEBUG
            // This section is specific to MSVC debug mode,
            // as without the modulo we get safe iterator errors,
            // causing broken functionality.
            // MSVC release mode and all other compilers don't have an issue
            // with this line
            return static_cast<int>(index / this->buff.channel_capacity()) %
                   this->buff.channels();
#else
            return static_cast<int>(index / this->buff->channel_capacity());
#endif
        }

        /// Audio Buffer we are iterating over
        BufferType* buff;
    };

    /**
     * @brief An iterator that iterates over signal data in an interleaved
     * manner
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
     * You can also think of this iterator as iterating over the *columns*
     * of the audio data matrix.
     *
     * TODO: See if we should add extra methods, like in SeqIterator...
     */
    template <bool IsConst = false>
    class InterIterator
        : public BaseMAECIterator<InterIterator<IsConst>, T, IsConst> {
    public:
        using BufferType = typename ChooseType<IsConst, const BaseBuffer<B, T>,
                                               BaseBuffer<B, T>>::type;
        using reference = typename BaseMAECIterator<InterIterator<IsConst>, T,
                                                    IsConst>::reference;
        using pointer = typename BaseMAECIterator<InterIterator<IsConst>, T,
                                                  IsConst>::pointer;

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
        InterIterator() = default;

        /**
         * @brief Construct a new Inter Iterator object
         *
         * We need to know the Buffer to iterate over,
         * and the starting position
         *
         * @param buff Buffer we are iterating over
         * @param pos Starting position
         */
        explicit InterIterator(BufferType* buff, int pos = 0) : buff(buff) {
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
        pointer resolve_pointer(int index) const {
            return this->buff->buff.data() + index;
        }

        /**
         * @brief Gets the current channel we are on
         *
         * Gets the current channel number we are on.
         *
         * @return int The current channel
         */
        int get_channel() const { return this->get_channel(this->get_index()); }

        /**
         * @brief Get the current sample we are on
         *
         * Gets the current sample we are on.
         *
         * @return int Current sample
         */
        int get_sample() const { return this->get_sample(this->get_index()); }

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
            this->set_index((this->buff->channels() * sample) + channel);
        }

    private:
        /**
         * @brief Gets the current channel we are on
         *
         * Gets the current channel number we are on.
         *
         * @return int The current channel
         */
        int get_channel(int index) const {
            return index % this->buff->channels();
        }

        /**
         * @brief Get the current sample we are on
         *
         * Gets the current sample we are on.
         *
         * @return int Current sample
         */
        int get_sample(int index) const {
            return static_cast<int>(index / this->buff->channels());
        }

        /// Buffer we are iterating over
        BufferType* buff;
    };

    /**
     * @brief Default constructor
     *
     * We use the braced initializer to ensure our container
     * gets initialzed correctly.
     *
     */
    constexpr BaseBuffer() : buff{} {};

    /**
     * @brief Constructs with size, channels, and sample rate
     *
     * This constructor will populate channel count, sample rate,
     * and channel capacity with the given values.
     * It will also pass the size to the constructor of the underlying
     * container.
     *
     * If the underlying container does not support the passing of an initial
     * size, then child Buffers should NOT call this function!
     *
     * TODO: Should size be total size of the buffer, or size of each channel?
     * It seems like with other constructors, we intepret size to be the size of
     * the buffer, but those also imply copying from a destination, so the sizes
     * need to match. Something needs to be done here
     *
     * @param size Total size of each channel
     * @param channels Number of channels to utilize
     * @param sra Sample rate to utilize
     */
    constexpr BaseBuffer(int size, int channels = 1, double sra = SAMPLE_RATE)
        : buff(size * channels), nchannels(channels), sample_rate(sra) {}

    /**
     * @brief Constructs with size, channels, and sample rate
     *
     * Identical to above constructor, but uses size_t for channel size
     * (great for standard library compatibility!)
     *
     * @param size Total size of each channel
     * @param channels Number of channels to utilize
     * @param sra Sample rate to utilize
     */
    constexpr BaseBuffer(std::size_t size, int channels = 1,
                         double sra = SAMPLE_RATE)
        : buff(size * channels), nchannels(channels), sample_rate(sra) {}

    /**
     * @brief Constructs using the copy constructor of the underlying container
     *
     * This constructor utilizes the copy constructor of the underlying
     * container, allowing for any behaviors realted to that to be evaluated.
     *
     * The contents of this vector MUST be in interleaved format,
     * as it will be directly copied into the underlying container.
     * Users can provide the number of channels present so we can correctly
     * intepret the contents of this buffer.
     *
     * @param vect Container to copy into underlying container
     * @param channels Number of channels present
     */
    constexpr explicit BaseBuffer(const B& vect, int channels = 1,
                                  double sra = SAMPLE_RATE)
        : buff(vect), nchannels(channels), sample_rate(sra) {}

    /**
     * @brief Constructs using the iterator constructor of the undelying
     * container
     *
     * This constructor utilizes the iterator constructor of the underlying
     * container, allowing for data from an arbitrary container to be copied
     * into this buffer.
     *
     * Users must provide a start and end iterator, which will be iterated upon
     * and copied into the underlying buffer.
     *
     * @tparam ITER1 Type of start iterator
     * @tparam ITER2 Type of stop iterator
     * @param begin Start iterator
     * @param end Stop iterator
     * @param channels Number of channels present
     */
    template <std::input_iterator ITER1, std::input_iterator ITER2>
    constexpr explicit BaseBuffer(const ITER1& begin, const ITER2& end,
                                  int channels = 1, double sra = SAMPLE_RATE)
        : buff(begin, end), nchannels(channels), sample_rate(sra) {}

    /**
     * @brief Constructs using an initializer list
     *
     * This constructor utilizes the initializer list constructor of the
     * underlying container, allowing for the container to be initialized to
     * some value.
     *
     * @param list Initializer list to utilize
     * @param channels Number of channels to utilize
     * @param sra Sample rate to utilize
     */
    constexpr BaseBuffer(const std::initializer_list<T>& list, int channels = 1,
                         double sra = SAMPLE_RATE)
        : buff(list), nchannels(channels), sample_rate(sra) {}

    /**
     * @brief Constructs using variadic templates
     *
     * This constructor utilizes the variadic template constructor of the
     * underlying container, which usually means the container can have values
     * placed into it at compile time.
     *
     * @tparam A Type to be placed into container, MUST be convertible to T
     */
    template <typename... A>
        requires(std::convertible_to<A, T> && ...)
    constexpr BaseBuffer(A... vals) : buff{vals...} {}

    /// Destructor
    constexpr ~BaseBuffer() = default;

    /// Copy constructor
    constexpr BaseBuffer(const BaseBuffer&) = default;

    /// Move Constructor
    constexpr BaseBuffer(BaseBuffer&&) noexcept = default;

    /// Copy assignment
    constexpr BaseBuffer& operator=(const BaseBuffer&) noexcept = default;

    /// Move assignment
    constexpr BaseBuffer& operator=(BaseBuffer&&) noexcept = default;

    /// Container copy assignment TODO: TEST
    constexpr BaseBuffer& operator=(const B& other) {

        this->assign(other);

        return *this;
    }

    /// Container move assignment TODO: TEST
    constexpr BaseBuffer& operator=(B&& other) {

        this->assign(std::move(other));

        return *this;
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
    constexpr void set_samplerate(double rate) { this->sample_rate = rate; }

    /**
     * @brief Get the samplerate of this buffer
     *
     * Returns the reported sample rate of this buffer.
     * It can be assumed that the reported sample rate
     * is accurate to the contained signal.
     *
     * @return double The current samplerate
     */
    constexpr double get_samplerate() const { return this->sample_rate; }

    /**
     * @brief Gets the size of this buffer.
     *
     * We report the absolute size of this buffer,
     * that being the number of values currently present.
     *
     * @return Size of this buffer (including all channels)
     */
    constexpr std::size_t size() const { return this->buff.size(); }

    /**
     * @brief Gets the capacity of each channel
     *
     * We report the capacity of each individual channel,
     * that being the number of samples present in each channel.
     *
     * We calculate this value dynamically by dividing the container size
     * byt eh channel count (rounding up).
     *
     * @return Capacity of each channel
     */
    constexpr std::size_t channel_capacity() const {
        return ceil(this->size() / this->channels());
    }

    /**
     * @brief Gets the number of channels in this buffer
     *
     * We report the number of channels,
     * not the channel size or total size.
     * This does not report the actual number of channels present,
     * but instead reports the expected number of channels.
     *
     * @return Number of channels
     */
    constexpr std::size_t channels() const { return this->nchannels; }

    /**
     * @brief Sets the number of channels
     *
     * Again, this only sets our understanding of the number of channels,
     * and does not set any content in the buffer.
     *
     * @param nchannels Number of channels to set
     */
    constexpr void set_channels(std::size_t nchannels) {
        this->nchannels = nchannels;
    }

    /**
     * @brief Copy assignment operation
     *
     * This function preforms a copy assignment on the underlying container.
     *
     * TODO: Must test
     *
     * @param other Container to copy from
     */
    constexpr void assign(const B& other) { this->buff = other; }

    /**
     * @brief Move assignment operation
     *
     * This function preforms a move assignment on the underlying container.
     *
     * TODO: Must test
     *
     * @param other Container to move from
     */
    constexpr void assign(B&& other) { this->buff = std::move(other); }

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
     * @return Value at the given channel and sample
     */
    constexpr reference at(int channel, int sample) {
        return this->buff[channel + (this->channels() * sample)];
    }

    /**
     * @brief Gets the const value at the given channel and sample.
     *
     * This method is identical to the normal at method,
     * with the exception that we are marked const and will not allow
     * alterations.
     *
     * @param channel Channel to get value from
     * @param sample Sample to get value from
     * @return Value at the given channel and sample
     */
    constexpr const_reference at(int channel, int sample) const {
        return this->buff[channel + (this->channels() * sample)];
    }

    /**
     * @brief Gets a value at the given position
     *
     * @param value Index to get value at
     * @return Value at given position
     */
    constexpr reference at(int value) { return this->buff[value]; }

    /**
     * @brief Gets a const value at the given position
     *
     * @param value Index to get value at
     * @return const_reference Value at given position
     */
    constexpr const_reference at(int value) const { return this->buff[value]; }

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
    constexpr BaseBuffer::SeqIterator<> sbegin() {
        return BaseBuffer::SeqIterator<>(this);
    }

    /**
     * @brief Gets the end sequential iterator for this buffer
     *
     * Returns the end iterator for sequential iteration.
     * This is useful for determining when to stop iterating over samples.
     *
     * @return Buffer::SeqIterator<long double>
     */
    constexpr BaseBuffer::SeqIterator<> send() {
        return BaseBuffer::SeqIterator<>(this, this->buff.size());
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
    constexpr std::reverse_iterator<BaseBuffer::SeqIterator<>> srbegin() {
        return std::reverse_iterator<BaseBuffer::SeqIterator<>>(
            BaseBuffer::SeqIterator<>(this, this->size()));
    }

    /**
     * @brief Gets the end reverse iterator for this buffer
     *
     * Returns the reverse sequential end iterator for this buffer.
     * Useful for determining when to stop iterating over samples in reverse.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    constexpr std::reverse_iterator<BaseBuffer::SeqIterator<>> srend() {
        return std::reverse_iterator<BaseBuffer::SeqIterator<>>(
            BaseBuffer::SeqIterator<>(this));
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
    constexpr BaseBuffer::SeqIterator<true> scbegin() const {
        return BaseBuffer::SeqIterator<true>(this);
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
    constexpr BaseBuffer::SeqIterator<true> scend() const {
        return BaseBuffer::SeqIterator<true>(this,
                                             static_cast<int>(this->size()));
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
    constexpr BaseBuffer::InterIterator<> ibegin() {
        return BaseBuffer::InterIterator<>(this);
    }

    /**
     * @brief Gets the end interleaved iterator for this buffer
     *
     * Returns the end iterator for interleaved iteration.
     * This is useful for determining when to stop iterating over samples.
     *
     * @return Buffer::InterIterator
     */
    constexpr BaseBuffer::InterIterator<> iend() {
        return BaseBuffer::InterIterator<>(this,
                                           static_cast<int>(this->buff.size()));
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
    constexpr std::reverse_iterator<BaseBuffer::InterIterator<>> irbegin() {
        return std::reverse_iterator(
            BaseBuffer::InterIterator<>(this, this->size()));
    }

    /**
     * @brief Gets the end interleaved reverse iterator for this buffer
     *
     * Returns the end reversed interleaved iterator for this buffer.
     * Useful for determining when to stop iterating.
     *
     * @return std::reverse_iterator<Buffer::SeqIterator<long double>>
     */
    constexpr std::reverse_iterator<BaseBuffer::InterIterator<>> irend() {
        return std::reverse_iterator(BaseBuffer::InterIterator<>(this));
    }

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
    constexpr BaseBuffer::InterIterator<true> icbegin() const {
        return BaseBuffer::InterIterator<true>(this);
    }

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
    constexpr BaseBuffer::InterIterator<true> icend() const {
        return InterIterator<true>(this, this->size());
    }

    /**
     * @brief Default start iterator
     *
     * The default start iterator simply returns an
     * InterIterator.
     *
     * @return InterIterator
     */
    constexpr BaseBuffer::InterIterator<> begin() { return this->ibegin(); }

    /**
     * @brief Default stop iterator
     *
     * The default stop iterator simply returns
     * an InterIterator.
     *
     * @return InterIterator
     */
    constexpr BaseBuffer::InterIterator<> end() { return this->iend(); }

protected:
    /**
     * @brief Gets the underlying buffer container
     *
     * This method is private as end users should NOT
     * be interacting with the underlying buffer component.
     *
     * @return B& Buffer container in use
     */
    constexpr B& get_buff() { return this->buff; }

private:
    /// Underlying container holding data
    B buff;

    /// Number of channels in this buffer
    std::size_t nchannels = 1;

    /// Sample rate in Hertz
    double sample_rate = SAMPLE_RATE;

    /// Various friend defintions
    friend class BaseBuffer::SeqIterator<>;
    friend class BaseBuffer::SeqIterator<true>;
    friend class BaseBuffer::InterIterator<>;
    friend class BaseBuffer::InterIterator<true>;
};

template <typename T>
class Buffer : public BaseBuffer<std::vector<T>> {
public:
    Buffer() = default;

    /**
     * @brief Construct a new Audio Buffer object
     *
     * Please be aware, this vector will be empty!
     * You will need to fill it with you own data for things to work correctly.
     *
     * @param size The size of each channel, AKA the number of samples per
     * channel
     * @param channels The number of channels in this buffer, by default 1
     * @param sra The sample rate to utilize in this buffer
     */
    constexpr explicit Buffer(int size, int channels = 1,
                              double sra = SAMPLE_RATE)
        : BaseBuffer<std::vector<T>>(size, channels, sra) {}

    /**
     * @brief Construct a new Audio Buffer object
     *
     * Identical to the above constructor, but uses size_t for channel size
     * (Great for standard library compatibility!)
     *
     * @param size The size of each channel, AKA the number of samples per
     * channel
     * @param channels The number of channels in this buffer, by default 1
     * @param sra The sample rate to utilize in this buffer
     */
    constexpr explicit Buffer(std::size_t size, int channels = 1,
                              double sra = SAMPLE_RATE)
        : BaseBuffer<std::vector<T>>(size, channels, sra) {}

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
    constexpr explicit Buffer(const std::vector<T>& vect, int channels = 1,
                              double sra = SAMPLE_RATE)
        : BaseBuffer<std::vector<T>>(vect, channels, sra) {}

    /**
     * @brief Constructs using the iterator constructor of the undelying
     * container
     *
     * This constructor utilizes the iterator constructor of the underlying
     * container, allowing for data from an arbitrary container to be copied
     * into this buffer.
     *
     * Users must provide a start and end iterator, which will be iterated upon
     * and copied into the underlying buffer.
     *
     * @tparam ITER1 Type of start iterator
     * @tparam ITER2 Type of stop iterator
     * @param begin Start iterator
     * @param end Stop iterator
     * @param channels Number of channels present
     */
    template <class ITER1, class ITER2>
    constexpr explicit Buffer(const ITER1& begin, const ITER2& end,
                              int channels = 1, double sra = SAMPLE_RATE)
        : BaseBuffer<std::vector<T>>(begin, end, channels, sra) {}

    /**
     * @brief Constructs using an initializer list
     *
     * This constructor utilizes the initializer list constructor of the
     * underlying container, allowing for the container to be initialized to
     * some value.
     *
     * @param list Initializer list to utilize
     * @param channels Number of channels to utilize
     * @param sra Sample rate to utilize
     */
    constexpr Buffer(const std::initializer_list<T>& list, int channels = 1,
                     double sra = SAMPLE_RATE)
        : BaseBuffer<std::vector<T>>(list, channels, sra) {}

    /**
     * @brief Constructs using variadic templates
     *
     * We send the values directly to the underlying vector via the variadic
     * template.
     *
     * @tparam A Type to be placed into container, MUST be convertible to T
     */
    template <typename... A>
        requires(std::convertible_to<A, T> && ...)
    constexpr Buffer(A... vals) : BaseBuffer<std::vector<T>>(vals...) {}

    /// Destructor
    constexpr ~Buffer() = default;

    /// Copy constructor
    constexpr Buffer(const Buffer&) = default;

    /// Move Constructor
    constexpr Buffer(Buffer&&) noexcept = default;

    /// Copy assignment
    constexpr Buffer& operator=(const Buffer&) noexcept = default;

    /// Move assignment
    constexpr Buffer& operator=(Buffer&&) noexcept = default;

    /// Container copy assignment TODO: TEST
    constexpr Buffer& operator=(
        const BaseBuffer<std::vector<T>>::container& other) {

        this->assign(other);

        return *this;
    }

    /// Container move assignment TODO: TEST
    constexpr Buffer& operator=(BaseBuffer<std::vector<T>>::container&& other) {

        this->assign(std::move(other));

        return *this;
    }

    /**
     * @brief Pre-allocates the buffer to a certain size.
     *
     * This tells the underlying vector to allocate memory
     * beforehand, which leads to faster performance
     * when writing (and reading, but to a lesser degree...),
     * so it is recommended to allocate the vector before any operations.
     *
     * See: https://cplusplus.com/reference/vector/vector/reserve/
     * For more info.
     *
     * We reserve the size using the value provided to us.
     *
     * @param size Size to reserve buffer
     */
    constexpr void reserve(std::size_t size) { this->get_buff().reserve(size); }

    /**
     * @brief Resizes the vector size
     *
     * This method resizes the vector to the provided elements.
     * https://en.cppreference.com/w/cpp/container/vector/resize
     *
     * @param size Size to resize vector to
     */
    constexpr void resize(int size) { this->get_buff().resize(size); }

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
    constexpr void shrink() { this->buff.shrink_to_fit(); }

    /**
     * @brief Removes all values from vector
     *
     * This removes all values from the buffer,
     * which will leave the container with a size of 0.
     * A reallocation may occur, so the capacity may remain unchanged.
     *
     * This function is great for resetting the state of the vector
     * to something sane.
     * In particular, after a buffer has been destroyed during a move
     * (source of move),
     * then this function can be used to reset the vector to a workable state.
     * From there, you can manipulate and reset the vector as usual.
     *
     * See: https://cplusplus.com/reference/vector/vector/clear/
     * For more info
     */
    constexpr void clear() { this->get_buff().clear(); }

    /**
     * @brief Adds a value to the buffer
     *
     * We push a value to the back of the buffer.
     * Please be aware, that we have no understanding of channels
     * or channel sizes!
     * You will need to provide your information in interleaved format
     * if you are working with multiple channels.
     *
     * @param val Value to add to buffer
     */
    constexpr void push_back(const T& val) { this->get_buff().push_back(val); }
};

template <typename T, std::size_t SIZE>
class StaticBuffer : public BaseBuffer<std::array<T, SIZE>> {
public:
    StaticBuffer() = default;

    /**
     * @brief Construct a new Audio Buffer object
     *
     * This will set the number of channels to utilize
     *
     * @param channels The number of channels in this buffer, by default 1
     * @param sra Sample rate of this buffer, by default the global MAEC default
     * sample rate
     */
    explicit StaticBuffer(int channels, double sra = SAMPLE_RATE) {

        // Set the channels and sample rate:

        this->set_channels(channels);
        this->set_samplerate(sra);
    }

    /**
     * @brief Constructs using variadic templates
     *
     * This constructor passes the provided values directly to the underlying
     * vector.
     *
     * @tparam A Type to be placed into container, MUST be convertible to T
     */
    template <typename... A>
        requires(std::convertible_to<A, T> && ...)
    StaticBuffer(A... vals) : BaseBuffer<std::array<T, SIZE>>(vals...) {}

    /// Destructor
    ~StaticBuffer() = default;

    /// Copy constructor
    StaticBuffer(const StaticBuffer&) = default;

    /// Move Constructor
    StaticBuffer(StaticBuffer&&) noexcept = default;

    /// Copy assignment
    StaticBuffer& operator=(const StaticBuffer&) noexcept = default;

    /// Move assignment
    StaticBuffer& operator=(StaticBuffer&&) noexcept = default;

    /// Container copy assignment TODO: TEST
    constexpr StaticBuffer& operator=(
        const BaseBuffer<std::array<T, SIZE>>::container& other) {

        this->assign(other);

        return *this;
    }

    /// Container move assignment TODO: TEST
    constexpr StaticBuffer& operator=(
        BaseBuffer<std::array<T, SIZE>>::container&& other) {

        this->assign(std::move(other));

        return *this;
    }
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
    template <typename V, bool IsConst = false>
    class RingIterator
        : public BaseMAECIterator<RingBuffer::RingIterator<V, IsConst>, V,
                                  IsConst> {

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
        T* resolve_pointer(int index) {
            return this->buff->buff.data() + this->buff->normalize_index(index);
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
    explicit RingBuffer(const std::vector<T>& other)
        : buff(other), bsize(other.size()) {}

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
    RingBuffer::RingIterator<T> begin() {
        return RingBuffer::RingIterator<T>(this, 0);
    }

    /**
     * @brief Gets the end iterator for the ring buffer
     *
     * You can use this iterator to determine when you have reached the end of
     * the buffer.
     *
     * @return RingBuffer::RingIterator<T> Stop iterator
     */
    RingBuffer::RingIterator<T> end() {
        return RingBuffer::RingIterator<T>(this, this->size());
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
    friend class RingIterator<T>;
};
