/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains various meta audio modules
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 * This file contains various 'meta modules'
 * that can be useful, particularly for debugging,
 * but are not strictly necessary for generating audio.
 *
 */

#pragma once

#include "audio_buffer.hpp"
#include "chrono.hpp"
#include "source_module.hpp"

/**
 * @brief Counts the number of samples this module encounters, and how many
 * times it has been processed
 *
 * This module counts the size of the buffer from back modules,
 * and saves it for future use.
 * We also count how many times we have been processed.
 * From there, the audio buffer can be grabbed by any forward modules.
 *
 * This module is for testing only!
 * We don't recommend using it in production...
 *
 */
class Counter : public AudioModule<> {

private:
    /// Number of times we have been processed
    int m_processed = 0;

    /// Number of samples we have encountered
    int m_samples = 0;

public:
    /**
     * @brief Resets this module
     *
     * We simply set the number of processed and samples
     * encountered to zero.
     *
     */
    void reset() {
        this->m_processed = 0;
        this->m_samples = 0;
    }

    /**
     * @brief Gets the number of times we have been processed
     *
     * @return int Number of times we have been processed
     */
    int processed() const { return this->m_processed; }

    /**
     * @brief Gets the number of samples we have encountered
     *
     * @return int Number of samples encountered
     */
    int samples() const { return this->m_samples; }

    /**
     * @brief Counts how many samples we encounter, and how many times we have
     * been called
     *
     * We keep track of the info specified above.
     *
     */
    void process() override;
};

/**
 * @brief Measures various timing statistics for determining latency.
 *
 * Time information will be stored about each operation.
 * An operation is defined as the meta process of back modules.
 * So, if we time an operation, it will be the time it took
 * for back modules to process.
 *
 * We measure the following values:
 *
 * - total time elapsed since start
 * - expected time since start
 * - time of last operation
 * - latency of last operation
 * - total latency thus far
 * - average time of each operation
 * - average latency of each operation
 *
 * We utilize a chain timer that allows us to determine
 * the ideal time.
 * We also inherit from the Counter to aid with the process
 * of counting samples and times processed.
 * We do our timekeeping in nanoseconds.
 */
class LatencyModule : public Counter {

private:
    /// Time we started on
    int64_t start_time = 0;

    /// Time of the last operation
    int64_t operation_time = 0;

    /// Latency of the last operation
    int64_t operation_latency = 0;

    /// Sum of all operation times
    int64_t total_operation_time = 0;

    /// Sum of all latency times
    int64_t total_operation_latency = 0;

    /// Chain timer for ideal timekeeping
    ChainTimer timer;

public:
    LatencyModule() = default;

    /**
     * @brief Resets this module
     *
     * We simply clear all variables associated with this class,
     * as well as set the start time to now.
     *
     */
    void reset();

    /**
     * @brief Gets the start time for this module
     *
     * This value does not always correlate to anything!
     * We simply use it to determine time deltas down the line.
     *
     * @return int Start time in nanoseconds
     */
    int64_t get_start_time() const { return this->start_time; }

    /**
     * @brief Gets the total elapsed time in nanoseconds
     *
     * We utilize the start time to determine the total elapsed time.
     * This value does not take into account the processing time!
     * Any dead time spent doing nothing will be included here,
     * so it is recommended to use something more accurate,
     * such as the operation or latency time.
     *
     * @return int Total time elapsed since start in nanoseconds
     */
    int64_t elapsed() const { return get_time() - this->start_time; }

    /**
     * @brief Gets the expected time in nanoseconds
     *
     * We utilize the ChainTimer to get the time we expect to have elapsed
     * since the start of processing.
     *
     * @return int Expected elapsed time in nanoseconds
     */
    int64_t expected() const { return this->timer.get_time(); }

    /**
     * @brief Gets the time of the last operation in nanoseconds
     *
     * The operation time is the total time spent meta processing
     * all back modules.
     *
     * @return int Elapsed operation time in nanoseconds
     */
    int64_t time() const { return this->operation_time; }

    /**
     * @brief Gets the total operation time in nanoseconds
     *
     * We sum the time elapsed after each operation.
     *
     * @return int Total operation time in nanoseconds
     */
    int64_t total_time() const { return this->total_operation_time; }

    /**
     * @brief Gets the latency of the last operation in nanoseconds
     *
     * We determine the latency by subtracting the total time by the expected
     * time. Please note, a negative latency is possible! This can happen if we
     * are processing audio data way faster than the expected sample rate! A
     * negative latency value is good!
     *
     * @return int Latency of last operation in nanoseconds
     */
    int64_t latency() const { return this->operation_latency; }

    /**
     * @brief Gets the total latency in nanoseconds
     *
     * We take the total operation time and and current expected chain time
     * to determine the total latency.
     * This allows you to understand the latency up to this point.
     *
     * @return int Total latency in nanoseconds.
     */
    int64_t total_latency() const {
        return this->total_operation_time - this->timer.get_time();
    }

    /**
     * @brief Gets the sum of all operation latency
     *
     * @return int64_t Sum of all operation latency
     */
    int64_t sum_latency() const { return this->total_operation_latency; }

    /**
     * @brief Gets the average operation time in nanoseconds
     *
     * We compute the average operation time
     * by using the total operation time and the number of times processed.
     *
     * @return int Average operation time in nanoseconds
     */
    int64_t average_time() const {
        return this->total_operation_time / this->processed();
    }

    /**
     * @brief Gets the average operation latency in nanoseconds
     *
     * We compute the average operation latency
     * by using the total operation latency and the number of times processed
     *
     * @return int Average operation latency in nanoseconds
     */
    int64_t average_latency() const {
        return this->total_operation_latency / this->processed();
    }

    /**
     * @brief Starts this latency module
     *
     * We simply set the start time for later use.
     *
     */
    void start() override { this->start_time = get_time(); }

    /**
     * @brief Meta process this module
     *
     * We do some timekeeping operations
     * and save them for future use.
     *
     */
    void meta_process() override;

    /**
     * @brief Dummy process method
     *
     * We simply do nothing.
     * This allows any subclassed modules
     * to put custom stuff here,
     * and prevents the count module from being
     * processed twice.
     *
     */
    void process() override {}
};

/**
 * @brief Repeats a buffer for output
 *
 * This module will save a buffer, and repeat it forever.
 * We do this by copying an internal buffer to a new one when processed.
 * Please keep this performance issue in mind when using this module.
 *
 * We also set the size of the chain to match the size of this buffer.
 * TODO: Is this a good idea?
 */
class BufferModule : public SourceModule {

private:
    /// The 'good' copy to repeat
    AudioBuffer* gbuff = nullptr;

public:
    BufferModule() = default;

    explicit BufferModule(AudioBuffer* ibuff) { this->set_rbuffer(ibuff); }

    /**
     * @brief Sets the buffer to repeat
     *
     * @param ibuff Buffer to repeat
     */
    void set_rbuffer(AudioBuffer* ibuff) {
        this->gbuff = ibuff;
        this->get_info()->out_buffer = static_cast<int>(ibuff->size());
    }

    /**
     * @brief Gets the buffer being repeated
     *
     * @return AudioBuffer* Buffer being repeated
     */
    AudioBuffer* get_rbuffer() const { return this->gbuff; }

    /**
     * @brief Copys the initial buffer into a new one
     *
     * We create a new buffer to be returned,
     * and copy the old contents into the new one.
     *
     */
    void process() override;
};

/**
 * @brief Ensures outgoing AudioBuffers are a uniform size
 *
 * This module takes buffers of arbitrary size on the input,
 * and outputs a buffer of standard size.
 * For example, if we are inputting buffers with size 20,
 * and we want an output size of 10, then we will split up the buffer into two
 * parts. The opposite is also true, if we are inputting buffers with size 10
 * and want an output size of 20, then we will sample the back modules twice
 * and merge the contents into one buffer.
 *
 * This is useful if you want to work with buffers of a specific size,
 * and you don't know exactly what the incoming buffer size will be,
 * or if you don't want to do the work yourself.
 *
 * Be warned, we may process the back modules multiple times to fill the buffer!
 * Ensure that any syncronization issues will not arise if this module is used.
 *
 */
class UniformBuffer : public AudioModule<> {

private:
    /// Index to empty position in out buffer
    int index = 0;

    /// Index to next copy position in in buffer
    int iindex = 0;

    /// Current incoming buffer
    AudioBuffer ibuff;

public:
    UniformBuffer() = default;

    /**
     * @brief Process method
     *
     * Within, we do the buffer uniform operation.
     *
     */
    void process() override;

    /**
     * @brief Overwrite the meta process method
     *
     * We simply do nothing but call the process method!
     * It will handle back processing when necessary.
     *
     */
    void meta_process() override { this->process(); }
};

/**
 * @brief Generates buffers filled with a given value
 *
 * We will create and fill a vector with the given value.
 * You can set this number with the set_value() method,
 * or by passing a double to the class constructor.
 * Otherwise, the default value is 0.0.
 *
 */
class ConstModule : public SourceModule {

private:
    /// The value to return
    long double value = 0.0;

public:
    ConstModule() = default;

    /**
     * @brief Construct a new Constant Oscillator object
     *
     * @param invalue Value to use in buffer creation
     */
    ConstModule(long double invalue) : value(invalue) {}

    /**
     * @brief Set the value to use in buffer creation
     *
     * @param invalue Value to use in buffer creation
     */
    void set_value(long double invalue) { this->value = invalue; }

    /**
     * @brief Get the value object
     *
     * We return the value that will be used in buffer creation.
     *
     * @return long double
     */
    long double get_value() const { return this->value; }

    /**
     * @brief Override process method
     *
     * We will fill a buffer filled with the given value.
     *
     */
    void process() override;
};
