/**
 * @file parallel.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Parallel module
 * @version 0.1
 * @date 2025-11-13
 *
 * @copyright Copyright (c) 2025
 *
 * This file contains components for running maec components in parallel.
 * The concepts within allows you to construct chains that
 * have many components running at once, which can significantly improve
 * performance.
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>

#include "audio_buffer.hpp"
#include "audio_module.hpp"

/**
 * @brief Allows modules to operate in parallel
 *
 * This module will run all backwards modules in parallel.
 * We spin up a thread that will continuously sample
 * the backwards modules, which allows for multiple cores to be utilized.
 *
 * We keep a cache of result buffers, which puts a limit on how many buffers we
 * store. For example, if the limit is 5, then we will stop processing once
 * there are 5 buffers present in the cache.
 * When we hit the cache limit, we stop processing the backwards module.
 * This means values must be removed from the cache before more buffers are
 * generated.
 *
 * ONLY the processing methods are ran in parallel. Any state operations
 * are ran synchronously.
 *
 */
class ParallelModule : public AudioModule<> {
private:
    /// Thread managed by this object
    std::thread thread;

    /// Queue of buffer data
    std::deque<AudioBuffer> queue;

    /// Size of the queue, how many values to cache
    std::size_t qsize = 1;

    /// Condition variable to determine if we can add to cache
    std::condition_variable cv;

    /// Mutex for mutual exclusion to queue
    std::mutex qmut;

    /// Boolean determining if threads should be done
    std::atomic<bool> done = false;

    /**
     * @brief Function ran by the thread
     *
     * We preform a few operations here:
     * 1. We sample the previous module in our running thread
     * 2. We add sampled data to the buffer queue
     * 3. We pause execution if we have too many values in the queue
     *
     */
    void run_thread() {

        // First, we iterate until we are stopped

        while (true) {

            // Define a lock to utilize for accessing the queue

            std::unique_lock<std::mutex> lock(qmut);

            // Wait on the condition variable until we have enough space,
            // or until we have been stopped

            cv.wait(lock, [&] { return queue.size() < qsize || done; });

            // Determine if we are done here

            if (done) {

                // We should stop processing now

                return;
            }

            // We want to process the backward module,
            // this can take some time so we unlock to allow readers to grab data

            lock.unlock();

            // We now have the ability to sample the backwards module

            this->backward().meta_process();

            // We need to grab the lock once more to ensure we have exclusive access

            lock.lock();

            // Move the backwards buffer to the back of the dequeue

            queue.emplace_back(std::move(this->backward().get_buffer()));

            // Remove the lock now that the value is added,
            // we want the consumer to immediatly be able to pull buffers
            // once notified

            lock.unlock();

            // Alter the write we are done

            cv.notify_all();
        }
    }

public:
    ParallelModule() = default;

    /**
     * @brief Gets the maximum size of the queue
     *
     * The max size is the number of buffers to cache.
     * The larger this value, the more buffers we will store
     * under the hood which can then be returned to the caller.
     *
     * @return std::size_t Queue size
     */
    std::size_t max_size() const { return qsize; }

    /**
     * @brief Sets the maximum size of the queue
     *
     * The max size is the number of buffers to cache.
     * The larger this value, the more buffers we will store
     * under the hood which can then be returned to the caller.
     *
     * @param size New queue size
     */
    void max_size(std::size_t size) { qsize = size; }

    /**
     * @brief Gets the current size of the queue
     *
     * The size is the number of buffers currently in the queue
     *
     * @return std::size_t Samples present in queue
     */
    std::size_t size() const { return queue.size(); }

    /**
     * @brief Starts the parallel module
     *
     * We set our state as usual,
     * and spin up the thread in the background.
     *
     */
    void start() override {

        // Call the parent start method

        AudioModule<>::start();

        {
            // We grab the lock to ensure our next operation
            // on the queue is valid.
            std::lock_guard<std::mutex> lock(qmut);

            // Clear the queue

            this->queue.clear();

            // Set the done value to be false
            
            this->done = false;
        }

        // Create the thread that runs the target function

        thread = std::thread{&ParallelModule::run_thread, this};
    }

    /**
     * @brief Stops the parallel module
     *
     * We set our state as usual,
     * and notify the thread that is is time to stop.
     *
     * We also join the thread to enable a graceful shutdown,
     * so this method may block for some time
     *
     */
    void stop() override {

        // Call the parent start method

        AudioModule<>::stop();

        // Set the done value to be true

        done = true;

        // Notify all condition variables

        this->cv.notify_all();

        // Join the thread and wait for completion

        this->thread.join();
    }

    /**
     * @brief Custom meta process operation
     *
     * Since the backwards module needs to be ran in a thread,
     * we do NOT want the generic functionality here.
     * Instead, we just want to grab and set the buffer
     * from the cache and set it as our own.
     *
     */
    void meta_process() override {

        // Create a unique lock from the mutex,
        // which drives the condition variable

        std::unique_lock<std::mutex> lock(qmut);

        // We need to wait until we have been notified,
        // and when the queue is not empty

        cv.wait(lock, [&] { return !this->queue.empty() || this->done; });

        // Determine if we are done and should not continue

        if (this->done) {
            return;
        }

        // Set the buffer to be front of the dequeue

        this->set_buffer(std::move(this->queue.front()));

        // Remove the first value in the dequeue

        this->queue.pop_front();

        // Explicitly unlock so the worker can process

        lock.unlock();

        // Notify any threads that it is ok to write

        cv.notify_all();
    }
};
