/**
 * @file audio_module.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains module components for working with audio data.
 * @version 0.1
 * @date 2022-09-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <array>
#include <memory>
#include <vector>
#include "base_module.hpp"


typedef std::unique_ptr<std::vector<long double>> AudioBuffer;


/**
 * @brief Structure for holding audio info
 * 
 * We contain various info to be shared among modules
 * that are linked together.
 */
struct AudioInfo {

    double freq;  /// The frequency of the audio data, if applicable
    double sample_rate;  /// The sample rate of the audio data, if applicable

    int done;  /// Number of modules that are finished
    int num;  /// Number of modules present in the chain
    int velocity;  /// Velocity of the audio data, if applicable
    int buff_size = 440;  /// The size of the audio buffer

    bool running;  // Value determining if we are running

    std::vector<std::unique_ptr<long double*>> points;

};


/**
 * @brief Module for working with audio data
 * 
 * These modules are designed to work with audio data.
 * Audio modules can be attached to each other to form a chain,
 * and audio data will flow them as they are linked.
 * 
 * Audio modules are share a pointer to an 'audio_info' struct,
 * which will be used to share information between modules.
 * 
 * All audio modules must inherit this class!
 */
class AudioModule : public BaseModule {

    private:

        /// Index of this module

        int index = 0;

        /// Pointer to the audio info

        AudioInfo* info;

        /// Pointer to the audio module we are attached to

        AudioModule* forward;

        /// Pointer to the audio module that is attached to us

        AudioModule* backward;

    protected:

        /// Pointer to the audio buffer we are working with
        // TODO: Figure out a better way to do this!
        // We are using raw C arrays, I would like to use C++ std::array (s) instead!

        AudioBuffer buff;

    public:

        /**
         * @brief Construct a new Audio Module object
         * 
         */
        AudioModule() {

            this->info = new AudioInfo();
        }

        /**
         * @brief Destroy the Audio Module object
         * 
         */
        virtual ~AudioModule() =default;

        /**
         * @brief Function called when processing is necessary.
         * 
         * This is where the magic happens!
         * Any time audio processing is required,
         * this method will be called.
         * 
         * This can do anything from generating audio data,
         * to taking audio data from previous modules and working with it.
         */
        virtual void process() {}

        /**
         * @brief Meta process method
         * 
         * This method contains all the meta code such as routing 
         * audio data to the next module in the chain,
         * and ensuring our state is valid (?).
         * 
         * Most users will not need to alter the code in this module,
         * but some advanced modules will need to, such as the audio mixers.
         */
        virtual void meta_process();

        /**
         * @brief Set the buffer object
         * 
         * This method is called when modules are attempting to set the buffer
         * for this audio module.
         *
         * This method can be overridden if necessary,
         * but most of the time it is not.
         * Advanced components such as the audio mixers
         * will use a custom version of this method.
         * 
         * @param inbuff Pointer to an audio buffer
         */
        virtual void set_buffer(AudioBuffer inbuff);

        /**
         * @brief Get the buffer object
         * 
         * This method is called by modules that are attempting to get the buffer
         * of the backward modules attached to them.
         * This function will move the ownership of the buffer to the caller,
         * so this should only be called by forward modules after processing is complete!
         * 
         * One other application for this is when the user defined process method
         * gets access to the buffer of the current module.
         * Keep in mind, the process method would then have to re-set the current
         * buffer using the set_buffer() method.
         * Therefore, it is recommended that you use the protected 'buff' pointer instead.
         * 
         * @return std::unique_ptr<long double> 
         */
        virtual AudioBuffer get_buffer();

        /**
         * @brief Binds another module to us
         * 
         * This method binds a module to us,
         * and makes that the source of audio buffers.
         * 
         * Most users can use the default method here with no problem,
         * but some advanced modules may put some custom code here.
         */
        virtual void bind(AudioModule* mod);

        /**
         * @brief Set the forward module
         * 
         * This sets the pointer to the forward audio module.
         * This method is usually called by the module we
         * are binding to.
         * 
         * @param mod 
         */
        virtual void set_forward(AudioModule* mod);

        /**
         * @brief Create a buffer object
         * 
         * Creates a buffer object and 
         * returns the result.
         * The resulting pointer should be unique,
         * and have ownership of the buffer.
         * TODO: TEST THIS FO SURE!
         * Not 100% sure that compilers will allow this...
         * 
         * @return long* 
         */
        AudioBuffer create_buffer();

        /**
         * @brief Get the forward object
         * 
         * We simply return the pointer to the forward module.
         * 
         * @return AudioModule* Pointer to the forward module
         */
        AudioModule* get_forward() { return this->forward; }

        /**
         * @brief Get the backward object
         * 
         * We simply return the pointer to the backward module.
         * 
         * @return AudioModule* Pointer to the backward module
         */
        AudioModule* get_backward() { return this->backward; }

        /**
         * @brief Get the info object
         * 
         * @return AudioInfo* 
         */
        AudioInfo* get_info() { return this->info; }

        /**
         * @brief Set the info object
         * 
         * @param info Audio info struct
         *
         */
        void set_info(AudioInfo* in) { this->info = in; }

};
