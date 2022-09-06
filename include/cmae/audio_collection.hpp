/**
 * @file audio_collection.hpp
 * @author Owen Cochell (you@domain.com)
 * @brief Defines the AudioCollection, a tool for working with many modules
 * @version 0.1
 * @date 2022-09-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <vector>
#include "audio_module.hpp"

/**
 * @brief Component for working with many modules
 * 
 * This class allows for multiple audio modules to be sampled 
 * and mixed together.
 * So, instead of a one-to-one relationship:
 * 
 * module -> module
 * 
 * You can utilize a many-to-one relationship:
 * 
 * module --+
 *          |
 * module --+--> module
 *          |
 * module --+
 * 
 * This allows for multiple modules to be combined into one.
 * Technically, this class is an AudioModule,
 * so it can be manipulated and worked with as any AudioModule.
 * 
 * We simply add the values from two modules together.
 * TODO: Figure out a way to have custom mixing operations,
 * such as multiplying by a certain value
 * 
 */
class AudioCollection: public AudioModule {

    private:

        /// @brief Vector of audio modules
        std::vector<AudioModule> mods;

    public:

        /**
         * @brief Adds the given module to our collection
         * 
         * @param mod AudioModule to add to the connection
         */
        void add_module(AudioModule mod);

        /**
         * @brief Starts all modules attached to us
         * 
         * When this start method is called,
         * we start all modules attached to us recursively
         * 
         */

        /**
         * @brief Stops all modules in our collection
         * 
         */
        void stop_modules();

        /**
         * @brief Tells all modules to finish up
         * 
         * We call the 'finish()' method on each module,
         * and allows them to finish up as necessary.
         * 
         */
        void finish_modules();

        /**
         * @brief Resets this collection
         * 
         * We remove all modules attached to us and return to our default state.
         * Be warned, this will cause all objects in the vector to be destroyed!
         * 
         */
        void reset();

        /**
         * @brief Get the buffer object
         * 
         * We retrieve a buffer from each module attached to us,
         * and mix them together in some way (?)
         * TODO: Figure out the best way to do this!
         * By default we simply add all outputs together.
         * 
         * @return long* 
         */
        long double* get_buffer();

};