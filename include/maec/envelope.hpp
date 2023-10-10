/**
 * @file envelope.hpp
 * @author Owen Cochell (owencochell@gmail.com)
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

#include <memory>

#include "source_module.hpp"
#include "chrono.hpp"
#include "utils.hpp"

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
         * @brief Gets the current time in nanoseconds
         * 
         * We get the time for our ChainTimer.
         * 
         * @return int64_t Current time in nanoseconds
         */
        int64_t get_time() const { return this->timer.get_time(); }

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

        /**
         * @brief Determines the number of remaining samples
         * 
         * THis method determines the number of samples this envelope
         * has left ot return until it becomes invalid, i.e current time >= strop time
         * 
         * @return int 
         */
        int64_t remaining_samples() { return (this->get_stop_time() - this->get_timer()->get_time()) / this->get_timer()->get_npf(); }
};

/**
 * @brief Provides functionality for defining envelope durations
 * 
 * Envelopes, by design, have a understanding of a start time and stop time.
 * This is necessary for the envelope to understand where it is in time,
 * and when it needs to stop.
 * This system allows for a high level of customization,
 * but requires that the envelope has an 'absolute' understanding of time.
 * 
 * Sometimes, it is not possible for an envelope to know the exact time
 * it starts and stops. Sometimes it is only possible for an envelope
 * to understand it's own duration, and the absolute time values should be determined later.
 * 
 * This class does just that, we take an envelope, and a duration.
 * When this envelope is started, we save the start time from our current timer.
 * We then use this value to determine the start and stop value,
 * allowing you to define an envelope with a duration,
 * without having to worry about the absolute time information.
 * Most calls are just proxied to the envelope we are currently managing.
 * As of now, we are not configured for managing the envelope correctly!
 * The idea is that the envelope to be managed should be configured with necessary settings
 * BEFORE it is managed within this envelope.
 * 
 * TODO: Elaborate on difference between absolute and relative time
 * 
 */
class DurationEnvelope : public BaseEnvelope {

    private:

        /// Internal envelope to manage
        BaseEnvelope* env = nullptr;

        /// Duration of this envelope
        int64_t duration = 0;

    public:

        DurationEnvelope() =default;

        /**
         * @brief Sets the duration to the given value, in nanoseconds
         * 
         * @param dur Duration in nanoseconds
         */
        void set_duration(int64_t dur) { this->duration = dur; }

        /**
         * @brief Gets the current duration, in nanoseconds
         * 
         * @return int64_t Duration in nanoseconds
         */
        int64_t get_duration() const { return this->duration; }

        /**
         * @brief Sets the envelope to the current value
         * 
         * @param sev Envelope to work with
         */
        void set_envelope(BaseEnvelope* sev) { this->env = sev; }

        /**
         * @brief Gets the current envelope
         * 
         * @return BaseEnvelope* Current envelope we are working with
         */
        BaseEnvelope* get_envelope() const { return this->env; }

        /**
         * @brief Computes and sets the start and stop time
         * 
         */
        void start() override;

        /**
         * @brief Meta process this envelope
         * 
         * We simply call the meta process method on the child envelope,
         * and set it's buffer to be ours.
         * 
         */
        void meta_process() override { this->env->meta_process(); this->set_buffer(this->env->get_buffer()); }
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
 * v(t) = V0 * ((V1 / V0) * ((t - T0) - (T1 - T0)))
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
 * v(t) = V0 + (V1 - V0) * ((t - T0) / (T1 - T0))
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
 * 
 * We do utilize the start value and stop time.
 * If there are no current envelopes at the start of this chain,
 * then we return the start value until we reach a valid envelope.
 * Same goes for stop time, if we reach the end of all valid envelopes,
 * then we keep returning values until stop time.
 * 
 * If any envelope has a stop time in the negatives,
 * then we will NOT move onto the next envelope automatically,
 * and will sample it forever.
 * The user can force a move to the next envelope if need be.
 * 
 * This class has functionality to 'optimize' it's structure,
 * allowing for envelopes to be inserted in dead zones.
 * For example, if there is a period of 1 second in between two envelopes,
 * we will insert envelopes that return the value of the last envelope.
 * 
 * TODO: Really not super happy with this system...
 * I would like to separate this functionality.
 * Maybe move 'safe' functionality to another class, SafeChainEnvelope?
 * Could be good to ensure speed for components that can guarantee their structure
 */
class ChainEnvelope : public BaseEnvelope {

    private:

        /**
         * @brief An internal module to be used by ChainEnvelope
         * 
         * This module is used in between other envelopes.
         * For example, if an envelope ends at one second and the 
         * next envelope starts at second 2, then we will use this envelope
         * until we reach the next one.
         * 
         * When We reach a time in between envelopes,
         * we simply return the stop value of the last envelope that we encountered.
         * 
         * This envelope is self-destructive!
         * We will free ourselves once complete.
         * 
         */
        class InternalEnvelope : public ConstantEnvelope {};

        /// Collection of envelopes
        std::vector<BaseEnvelope*> envs {};

        /// Vector of internal envelopes
        std::vector<std::unique_ptr<InternalEnvelope>> inter {};

        /// Current envelope index we are on:
        int env_index = -1;

        /// Index of the last module optimized
        int optimized = 0;

        /// Current envelope we are working with:
        BaseEnvelope* current = nullptr;

        /// Value determining if we optimize
        bool can_optimize = true;

    public:

        ChainEnvelope() =default;

        /**
         * @brief Creates an internal envelope
         * 
         * TODO: ELABORATE
         */
        void create_internal(int index);

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
         * @brief Determines the next envelope
         * 
         * We find the next envelope in the chain,
         * TODO: Elaborate
         * 
         */
        void next_envelope();

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
        void start() override { this->next_envelope(); }

        /**
         * @brief Optimizes the internal structure of the ChainEnvelope
         * 
         * Our main purpose is to "fill in the blanks",
         * that being place certain envelopes in between two envelopes if there is dead time.
         * We use the "InternalEnvelope" for this, which simply returns the stop value
         * from the previous envelope.
         * 
         * By default, we keep track of the envelopes that are previously optimized,
         * meaning that when called again, only new envelopes will be optimized,
         * thus saving some time.
         * If need be, (an envelope is inserted into previously optimized envelopes)
         * then we can ignore this value and re-optimize the entire chain.
         * 
         * This function will be called automatically when necessary.
         */
        void optimize();
};

/**
 * @brief Defines an Attack, Decay, Sustain Release (ADSR) envelope
 * 
 * This class defines an ADSR Envelope:
 * 
 * - Attack is the time it takes to reach maximum value when key is initially pressed
 * - Decay is time it takes to release to sustain value
 * - Sustain is the value to hold at while key is held
 * - Release is the time it takes to ramp to zero after key is released
 * 
 */
class ADSREnvelope : public ChainEnvelope {

    private:

        /// Time it takes to ramp to max value
        int64_t attack = 0;

        /// Time it takes to ramp to sustain value
        int64_t decay = 0;

        /// Value to sustain at
        long double sustain = 0;

        /// Time it takes to ramp to zero after key is released
        int64_t release = 0;

        /// Collection for keeping envelopes:
        Collection<BaseEnvelope> envs;

    public:

        ADSREnvelope() =default;

        ADSREnvelope(int64_t att, int64_t dec, long double sus, int64_t rel) : attack(att), decay(dec), sustain(sus), release(rel) {}

        /**
         * @brief Sets the attack value
         * 
         * Sets the attack of this envelope.
         * 
         * @param att Attack to set
         */
        void set_attack(int64_t att) { this->attack = att; }

        /**
         * @brief Gets the attack value
         * 
         * @return int64_t Current attack
         */
        int64_t get_attack() const { return this->attack; }

        /**
         * @brief Sets the decay value
         * 
         * Sets the decay of this envelope
         * 
         * @param dec Decay to set
         */
        void set_decay(int64_t dec) { this->decay = dec; }

        /**
         * @brief Gets the decay value
         * 
         * @return int64_t Current decay
         */
        int64_t get_decay() const { return this->decay; }

        /**
         * @brief Sets the sustain value
         * 
         * Sets the sustain value of this envelope
         * 
         * @param sus Sustain to set
         */
        void set_sustain(long double sus) { this->sustain = sus; }

        /**
         * @brief Gets the sustain value
         * 
         * @return long double Current sustain value
         */
        long double get_sustain() const { return this->sustain; }

        /**
         * @brief Sets the release value
         * 
         * Sets the release value of this envelope
         * 
         * @param rel Release to set
         */
        void set_release(int64_t rel) { this->release = rel; }

        /**
         * @brief Gets the release value
         * 
         * @return int64_t Release to set
         */
        int64_t get_release() const { return this->release; }

        /**
         * @brief Starts this envelope
         * 
         * Here, we configure the internal envelope to match
         * the functionality of an ADSR envelope.
         * 
         */
        void start() override;

        /**
         * @brief Finishes this envelope
         * 
         * Here, we configure the envelope to ramp
         * down to zero when called.
         * We determine the current value we are operating at
         * and ramp downwards.
         * 
         */
        void finish() override;
};
