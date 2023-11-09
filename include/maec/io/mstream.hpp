/**
 * @file mstream.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief MAEC stream components
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file defines the interface/components for MAEC streams.
 * MAEC streams are custom objects that support reading and writing
 * from various sources.
 * These mstreams are designed to abstract away the details of streaming data.
 * 
 * Please note, mstreams are not designed to intepret/format/decode the data they are retrieving!
 * They are designed to simply retrieve the data from a source.
 * You should use higher level components for using this data in a meaningful way.
 */

#pragma once

#include <fstream>

/**
 * @brief Base class for mstreams
 * 
 * This class defines the basic components that should
 * be present in ALL mstreams.
 * We contain methods for state management, error checking,
 * and configuration.
 * 
 * mstreams have the following states:
 * 
 * - init - Inital state, no operations have been done
 * - started - mstream has been started, and can be utilized
 * - stopped - mstream is in stop state, no longer can be utilized
 * - err - mstream is in a bad state, no longer can be utilized
 */
template<bool Input = false, bool Output = false>
class BaseMStream {
public:
    /// State definition
    enum mstate { init, started, stopped, err };

    /**
     * @brief Start method for mstreams
     *
     * This method is called when the user is
     * ready to utilize the mstream.
     * This method only sets the run state.
     * Child mstreams should put any start code into this method.
     *
     * Before a mstream is used, it SHOULD be started.
     */
    virtual void start() {

        // Set state:

        this->state = started;
    }

    /**
     * @brief Stop method for mstreams
     * 
     * This method is called when the user is
     * ready to stop the mstream.
     * We will only configure the state.
     * 
     * After a stream has been stopped, it SHOULD NOT
     * be used again.
     * If an mstream is in the stop state,
     * then the state should be considered useless.
     */
    virtual void stop() {

        // Set stop state:

        this->state = stopped;
    }

    /**
     * @brief Gets the current state of the mstream
     * 
     * Te state of the mstream can be utilize
     * to understand the current status of the mstream.
     * 
     * @return Current state of mstream
     */
    mstate get_state() const { return this->state; }

    /**
     * @brief Determines if we are an output mstream
     * 
     * @return true We are output stream
     * @return false We are not an output stream
     */
    static bool is_output() { return Output; }

    /**
     * @brief Determines if we are an input stream
     * 
     * @return true We are an input stream
     * @return false We are not an input stream
     */
    static bool is_input() { return Input; }

private:
    /// Current state of mstream
    mstate state = init;
};

/**
 * @brief Base class for input mstreams
 * 
 * This class provides methods and functionality
 * for input mstreams.
 * We provide methods for reading data.
 * 
 * ALL mistreams MUST implement the methods and functionality defined here.
 */
template<bool Input = true, bool Output = false>
class BaseMIStream : public BaseMStream<Input, Output> {
public:

    /**
     * @brief Reads a number of bytes from the stream
     * 
     * This function handles the process of reading bytes from the stream.
     * How this function operates is specific to the stream class,
     * but the general idea is you provide a character array and a number 
     * of bytes to read, and the component will read and place those
     * bytes into the character array.
     * 
     * @param byts Charater array to place data into
     * @param num Number of bytes to read
     */
    virtual void read(char* byts, int num) =0;
};

/**
 * @brief Base class for output mstreams
 * 
 * This class provides methods and functionality for output streams.
 * We provide methods for writing data.
 * 
 * ALL mostreams MUST implement the methods and functionally defined here.
 */
template<bool Input = false, bool Output = true>
class BaseMOStream : public BaseMStream<Input, Output> {
public:

    /**
     * @brief Writes a number of bytes to the stream
     * 
     * This function handles the process of writing events to a stream.
     * How this function operations is specific to the stream class,
     * but the general idea is you provide a character array and a number
     * of bytes to read, and the component will read and place those
     * bytes into the character array.
     * 
     * @param byts Character array to write to stream
     * @param num Number of bytes to write
     */
    virtual void write(char* byts, int num) =0;
};

/**
 * @brief Base class for IO streams
 * 
 * This class provides methods and functionality for input/output streams.
 * We provide methods for reading and writing data.
 * 
 * ALL miostreams MUST inherit this class!
 * 
 */
class BaseMIOStream : public BaseMIStream<true, true>, public BaseMOStream<true, true> {

};

class FIStream : public BaseMIStream<> {
private:
public:
    virtual void read(char* byts, int num);
};
