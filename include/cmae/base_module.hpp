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
 * Created -> Started -> Running -> Finishing -> Stopped
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

        enum class Type {Created, Started, Running, Finishing, Stopped};

        /// The state type of this module:

        Type state_type =Type::Created;

    public:

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
        void stop();

        void start();  /// Method called when this module is started
        void finish();  // Method called when this module is requested to stop
        void done();  /// Method called when this module is

};