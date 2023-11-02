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
template <typename C>
class BaseMStream {
protected:

    /**
     * @brief Sets our input value
     * 
     * @param val Value to set
     */
    void set_input(bool val) { this->input = val; }

    /**
     * @brief Sets our output value
     * 
     * @param val Value to set
     */
    void set_output(bool val) { this->output = val; }

public:
    /// State definition
    enum mstate { init, started, stopped, err };

    /**
     * @brief Start method for mstreams
     *
     * This method is called when the user is
     * ready to utilize the mstream.
     * We will not only configure the state of the mstream,
     * but we will also call the child start method.
     * Child mstreams should put any start code into this method.
     *
     * Before a mstream is used, it SHOULD be started.
     */
    void start() {

        // call child method:

        static_cast<C*>(this)->start();

        // Set state:

        this->state = started;
    }

    /**
     * @brief Stop method for mstreams
     * 
     * This method is called when the user is
     * ready to stop the mstream.
     * We will configure the state and call the child stop method.
     * Child streams should put any stop code into this method.
     * 
     * After a stream has been stopped, it SHOULD NOT
     * be used again.
     * If an mstream is in the stop state,
     * then the state should be considered useless.
     */
    void stop() {

        // Call child method:

        static_cast<C*>(this)->stop();

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
    bool is_output() { return this->output; }

    /**
     * @brief Determines if we are an input stream
     * 
     * @return true We are an input stream
     * @return false We are not an input stream
     */
    bool is_input() { return this->input; }

private:
    /// Boolean determining if this stream is input
    bool input = false;

    /// Boolean determining if this stream is output
    bool output = false;

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
template <typename C>
class BaseMIStream : public BaseMIStream<BaseMIStream<C>> {
public:

    /**
     * @brief Starts this mistream
     * 
     * We simply set our input status and call
     * the child start method.
     */
    void start() {

        // Change our input/output state:

        this->set_input(true);

        // Call start method of child:

        static_cast<C*>(this)->start();
    }

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
    void read(char* byts, int num) {

        // Call child method:

        return static_cast<C*>(this)->read(byts, num);
    }
};

/**
 * @brief Base class for output mstreams
 * 
 * This class provides methods and functionality for output streams.
 * We provide methods for writing data.
 * 
 * ALL mostreams MUST implement the methods and functionally defined here.
 */
template <typename C>
class BaseMOStream : public BaseMStream<C> {
public:

    /**
     * @brief Starts this mostream
     * 
     * We simply set the output status,
     * and call the child start method
     */
    void start() {

        // Set our write status:

        this->set_output(true);

        // Call start method of child:

        static_cast<C*>(this)->start();
    }

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
    void write(char* byts, int num) {

        // Call child method:

        return static_cast<C*>(this)->write(byts, num);
    }
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
template <typename C>
class BaseMIOStream : public BaseMIStream<BaseMIOStream<C>>, public BaseMOStream<BaseMIOStream<C>> {

};
