/**
 * @file base_module.hpp
 * @author Owen Cochell (you@domain.com)
 * @brief This file contains components used for working with modules.
 * @version 0.1
 * @date 2022-09-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once


/**
 * @brief Represents a module
 * 
 * This class offers a framework for modules,
 * allowing them to be worked with and managed by higher level components.
 * 
 * A "module" is a class that has the following properties:
 *  1. Has a state (unloaded, loaded, started, stopped)
 *  2. Can be utilized by module collections
 *  3. Inherits this class
 * 
 * Modules should be designed to do one thing,
 * and do it well!
 * This is optional, as a single module could do many things,
 * but it is recommended to follow this suggestion.
 * 
 * Each module has a state it can be in:
 * 
 * Created -> Started -> Running -> Finishing -> Finished -> Stopped
 * 
 *  - Created - Module has been created 
 *  - Started - Module has been started, and will start working with data
 *  - Running - Module is currently running and working with audio data
 *  - Finishing - Module has been requested to stop, and should do any necessary operations
 *  - Stopped - Module has been stopped and will no longer work with data
 * 
 * The state can be identified by using the 'GetState()' method.
 * The state will automatically be set by each module as it is used.
 * 
 * We also contain state methods that are called when necessary,
 * checkout 'start()', 'stop()', 'finish()', ect. for more info.
 * 
 */
class BaseModule {

    private:

        /// The types of states we can be:

        enum class State {Created, Started, Running, Finishing, Finished, Stopped};

        /// The state type of this module:

        State state_type =State::Created;

    public:

        BaseModule() =default;

        virtual ~BaseModule() =default;

        /**
         * @brief Method called then this module is stopped
         * 
         * When this module is stopped,
         * this method will be called.
         * 
         * By default, we alter the state to the 'Stopped' value.
         * You can put any module specific stop code in here,
         * but be sure to call the parent version of this method!
         */
        virtual void stop();

        /**
         * @brief Method called when this module is started
         * 
         * When this module is started,
         * this method will be called.
         * 
         * Be default, we alter the state to the 'Started' value.
         * You can put any module specific stop code in here,
         * but be sure to call the parent version of this method!
         */
        virtual void start();

        /**
         * @brief Method called when this module is requested to stop
         * 
         * When this module is requested to stop,
         * this method will be called.
         * 
         * By default, we alter the state to the 'Finishing' value.
         * You can put any module specific finish code in here,
         * but be sure to call the parent version of this method!
         * 
         * In most cases, when modules are requested to stop,
         * it is appropriate to mark them as done immediately.
         * This is the default operation.
         * However, some modules may do things after the chain is asked to be stopped,
         * such as fade out in the case of an ADSR envelope.
         * 
         * Do note, that this method may not be called!
         * If this module is stopped without being asked nicely,
         * (stop() method is called without a call to finish()),
         * then it should do so and not cause trouble.
         */
        virtual void finish();

        /**
         * @brief Method called when this module is done
         * 
         * When this module is done and ready to be stopped,
         * this method will be called.
         * 
         * By default, we alter the state to the 'Stopped' value.
         * You can put any module specific done code in here,
         * but be sure to call the parent version of this method!
         * 
         * It is very important to call this method once we are done!
         * This tells the collection using this module that we are ready to be stopped.
         * If all modules are not marked as done, then the module chain will not stop!
         */
        virtual void done();

        /**
         * @brief Get the state of this module
         * 
         * We return the state of this module.
         * 
         * @return State An integer representing the state
         */
        State get_state() {return this->state_type;}

};
