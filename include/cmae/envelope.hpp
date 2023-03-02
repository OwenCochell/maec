/**
 * @file envelope.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Various components for envelopes 
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 * In digital audio, an 'envelope' describes how a parameter of a sound changes over time.
 * For use in MAEC, you can think of an envelope as a value that changes over time.
 * Each envelope does not necessarily have to alter a component of a sound,
 * but most of the time they will be used to do so.
 * 
 * MAEC exposes these envelopes as AudioModules,
 * meaning they can be linked and worked with like any other module.
 * It is not recommended to use these to generate audio data to work with!
 * Instead, use the outputs from these envelopes to modulate the 
 * parameter of another module.
 */

#pragma once

#include "source_module.hpp"
#include "chrono.hpp"

/**
 * @brief Base class for all envelopes
 * 
 * This class provides some common functionality for all envelopes.
 * Mainly, we provide a timer that can be utilized by certain envelopes,
 * as well as certain values that can aid with timekeeping.
 * 
 * This envelope helps keep sub-envelopes aware of certain parameters
 * that are necessary for a value to change over time,
 * that being the:
 * 
 * - start_time - Time this envelope starts
 * - stop_time - Time this envelope stops
 * - value_start - The starting value
 * - value_stop - The stopping value
 * 
 * These values allow for any basic envelope to be implemented properly.
 * High level envelopes (such a ChainEnvelope or ADSREnvelope) will likely NOT
 * implement these values, and will instead work with low-level envelopes such as these.
 * TODO: Figure this out
 * 
 * It is not recommended to use ANY envelopes for generating audio data!
 * Instead, these modules should be used to modulate the parameter of other modules.
 * 
 */
class BaseEnvelope : public SourceModule {

    private:

        /// Time to utilize for timekeeping
        ChainTimer timer;

        /// Time our envelope is starting on
        int64_t start_time = 0;

        /// Time our envelope is stopping on
        int64_t stop_time = 0;

        /// Value we are starting on
        long double value_start = 0;

        /// Value we are stopping on
        long double value_stop = 0;

    public:

        BaseEnvelope() =default;

        /**
         * @brief Gets the chain timer associated with this envelope
         * 
         * @return ChainTimer* Pointer to chain timer
         */
        ChainTimer* get_timer() { return &(this->timer); }

        /**
         * @brief Gets the current time and increments the sample
         * 
         * We get the current time,
         * and then increment the sample number.
         * This is a convience method,
         * and it aims to reduce typing.
         * 
         * @return int64_t Current time
         */
        int64_t get_time_inc();

        /**
         * @brief Sets the start value of this envelope
         * 
         * This is the value the envelope will start on.
         * 
         * @param startv Value to start on
         */
        void set_start_value(long double startv) { this->value_start = startv; }

        /**
         * @brief Gets the start value of this envelope
         * 
         * @return long double Start value of this envelope
         */
        long double get_start_value() const { return this->value_start; }

        /**
         * @brief Sets the stop value of this envelope
         * 
         * This is the value the envelope will stop on.
         * 
         * @param stopv Value to stop on
         */
        void set_stop_value(long double stopv) { this->value_stop = stopv; }

        /**
         * @brief Gets the stop value of this envelope
         * 
         * @return long double Stop value of this envelope
         */
        long double get_stop_value() const { return this->value_stop; }

        /**
         * @brief Sets the start time of this envelope
         * 
         * @param startt Start time of this envelope
         */
        void set_start_time(int64_t startt) { this->start_time = startt; }

        /**
         * @brief Sets the stop time of this envelope
         * 
         * @param stopt Stop time of this envelope
         */
        void set_stop_time(int64_t stopt) { this->stop_time = stopt; }

        /**
         * @brief Gets the start time of this envelope
         * 
         * @return int64_t Start time of this envelope
         */
        int64_t get_start_time() const { return this->start_time; }

        /**
         * @brief Gets the stop time of this envelope
         * 
         * @return int64_t Stop time of this envelope
         */
        int64_t get_stop_time() const { return this->stop_time; }

        /**
         * @brief Determines the time difference
         * 
         * We compute the stop time minus the start time.
         * Could be useful in some situations,
         * and saves some time typing.
         * 
         * @return int64_t Stop time minus start time
         */
        int64_t time_diff() const { return this->stop_time - this->start_time; }

        /**
         * @brief Determines the value of value_stop / value_start
         * 
         * We take into account the possibility that value_start may be zero.
         * If this is the case, we use a very low value.
         * 
         * @return long double Result of the divide operation
         */
        long double val_divide() const { return this->value_stop / (this->value_start != 0 ? this->value_start : SMALL); }

        /**
         * @brief Determines the value difference
         * 
         * We compute the stop value minus the start value.
         * This is a convience function,
         * and aims to save some typing.
         * 
         * @return long double Stop value minus start value
         */
        long double val_diff() const { return this->value_stop - this->value_start; }

};

/**
 * @brief Envelope that always returns the start value
 * 
 * This envelope will ALWAYS return the start value.
 * You can set the other values in this envelope, 
 * but they will be ignored.
 * 
 * Higher level components (such as the ChainEnvelope)
 * will employ these values, but by itself
 * this component will ignore them.
 * 
 */
class ConstantEnvelope : public BaseEnvelope {

    public:

        ConstantEnvelope() =default;

        /**
         * @brief Return the start value
         * 
         */
        void process() override;

};

/**
 * @brief Envelope that jumps to a value at the given time
 * 
 * We return the start value until we reach the end time.
 * Once we reach the end time, we return the stop value.
 * We do not do any ramp operations!
 * 
 */
class SetValue : public BaseEnvelope {

    public:

        SetValue() =default;

        /**
         * @brief Determine the value to return
         * 
         */
        void process() override;

};

/**
 * @brief Envelope that exponentially ramps to the target
 * 
 * We exponentially ramp to a value over a given time period.
 * Here is the formula for this operation:
 * 
 * v(t) = V0 * ((V1 / V0) ^ ((t - T0) - (T1 - T0)))
 * 
 * t = current time
 * V0 = start value
 * V1 = stop value
 * T0 = start time
 * T1 = end time
 * 
 */
class ExponentialRamp : public BaseEnvelope {

    public:

        ExponentialRamp() =default;

        /**
         * @brief Determine the current value at this time
         * 
         */
        void process() override;
};

/**
 * @brief Envelope that linearly ramps to the target
 * 
 * We linearly ramp to a value over a given time period.
 * Here is the formula for this operation:
 * 
 * v(t) = V0 + (V1 - V0) ^ ((t - T0) / (T1 - T0))
 * 
 * t = current time
 * V0 = start value
 * V1 = stop value
 * T0 = start time
 * T1 = end time
 * 
 */
class LinearRamp : public BaseEnvelope {

    public:

        LinearRamp() = default;

        /**
         * @brief Determine the current value at this time
         * 
         */
        void process() override;
};

/**
 * @brief Manages a collection of envelopes
 * 
 * This class handles a collection of envelopes,
 * switching to each one when necessary.
 * 
 * We synchronize all the envelopes using one timer,
 * allowing for time data to be used amongst the envelopes.
 * Envelopes should be added in the order they will executed in!
 * We will only move to the next envelope if the current one is complete.
 * So, if the start time of the next envelope falls within the interval
 * of the current envelope, we will not move to the next envelope
 * until the current envelope finishes.
 */
class ChainEnvelope : public BaseEnvelope {

    private:

        /// Collection of envelopes
        std::vector<BaseEnvelope*> envs;

        /// Current envelope we are working with:
        BaseEnvelope* current = nullptr;

    public:

        ChainEnvelope() =default;

        /**
         * @brief Adds the given envelope to this collection
         * 
         * We add the given envelope to the collection.
         * Again, envelopes are executed in the order 
         * they are added! Please be sure that you 
         * keep the order in a way that you want.
         * 
         * @param env Envelope to add
         */
        void add_envelope(BaseEnvelope* env);

        /**
         * @brief Gets the current envelope
         * 
         * @return BaseEnvelope* Current envelope
         */
        BaseEnvelope* get_current() const { return this->current; }

        /**
         * @brief Process this envelope
         * 
         * We determine if we need to move onto the next envelope,
         * and process the current envelope.
         * 
         */
        void process() override;

        /**
         * @brief Start method
         * 
         */
        void start() override;
};
