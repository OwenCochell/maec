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

#include <memory>
#include <utility>

#include "const.hpp"
#include "base_module.hpp"
#include "audio_buffer.hpp"

/**
 * @brief Structure for holding audio info
 * 
 * We contain various info to be shared among modules
 * that are linked together.
 * 
 * THIS INFO IS HOW THE AUDIO DATA WILL BE ONCE IT LEAVES THE CHAIN!
 * For example, the sample rate can be changed in the middle of the chain if need be.
 * But, it MUST match the chain sample rate when it leaves the chain,
 * otherwise we will encounter issues with the audio output modules.
 * 
 * TODO: Finalize and really structure this data.
 * 
 * Do we really need all values here?
 * We need to figure out how this data can be altered and changed in the chain...
 */
struct AudioInfo {

    long double freq = 0;  /// The frequency of the audio data, if applicable
    long double sample_rate = SAMPLE_RATE;  /// The sample rate of the audio data, if applicable

    int done = 0 ;  /// Number of modules that are finished
    int num = 0;  /// Number of modules present in the chain
    int velocity = 0;  /// Velocity of the audio data, if applicable
    int buff_size = BUFF_SIZE;  /// The size of the audio buffer

    bool running = false;  // Value determining if we are running

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

        /// Pointer to the audio info

        std::shared_ptr<AudioInfo> info=nullptr;

        /// Pointer to the audio module we are attached to

        AudioModule* forward=nullptr;

        /// Pointer to the audio module that is attached to us

        AudioModule* backward=nullptr;

    protected:

        /// Pointer to the audio buffer we are working with

        std::unique_ptr<AudioBuffer> buff = nullptr;

    public:

        /**
         * @brief Construct a new Audio Module object
         * 
         * We also create a AudioInfo struct for use
         * 
         */
        AudioModule() { this->info = std::make_shared<AudioInfo>(); }

        /**
         * @brief Destroy the Audio Module object
         * 
         */
        virtual ~AudioModule() =default;

        /**
         * @brief Copy constructor
         * 
         */
        AudioModule(const AudioModule&) =delete;

        /**
         * @brief Copy assignment operator
         * 
         * @return AudioModule& 
         */
        AudioModule& operator=(const AudioModule&) =delete;

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
         * This method contains all the meta code such as
         * retrieving the buffer from the previous module,
         * and calling the necessary processing methods. 
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
         * @param inbuff Pointer to an audio buffer
         */
        void set_buffer(std::unique_ptr<AudioBuffer> inbuff);

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
         * @return std::unique_ptr<AudioBuffer>
         */
        virtual std::unique_ptr<AudioBuffer> get_buffer();

        /**
         * @brief Binds another module to us
         * 
         * This method binds a module to us,
         * and makes that the source of audio buffers.
         * 
         * Most users can use the default method here with no problem,
         * but some advanced modules may put some custom code here.
         * 
         * We also specify if the AudioInfo should be synced between the two modules.
         * 
         * We also return the AudioModule we added, allowing for operations like this:
         * 
         * mod.bind(mod1.bind(mod2.bind(mod3)));
         * TODO: ^ This is less ideal, as AudioInfo structs are not synced, elaborate on this
         * 
         * or:
         * 
         * mod.bind(mod1).bind(mod2).bind(mod3);
         * 
         * The result would be this:
         * 
         * mod3 -> mod2 -> mod1 -> mod
         * 
         * @param mod The module to bind to us
         * @return AudioModule* The AudioModule we just bound
         */
        virtual AudioModule* bind(AudioModule* mod);

        /**
         * @brief Set the forward module
         * 
         * This sets the pointer to the forward audio module.
         * This method is usually called by the module we
         * are binding to.
         * 
         * @param mod The module to set as forward
         */
        virtual void set_forward(AudioModule* mod);

        /**
         * @brief Create a buffer object
         * 
         * Creates a buffer object and 
         * returns the result.
         * The resulting pointer should be unique,
         * and have ownership of the buffer.
         * 
         * We will automatically create a buffer with using
         * the size from the AudioInfo struct.
         * You can also specify the default number of channels,
         * but by default this will be 1.
         * 
         * TODO: TEST THIS FO SURE!
         * Not 100% sure that compilers will allow this...
         * 
         * @return The newly created buffer
         */
        std::unique_ptr<AudioBuffer> create_buffer(int channels=1);

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
         * @return std::shared_ptr<AudioInfo> The audio info in use by this module
         */
        std::shared_ptr<AudioInfo> get_info() { return this->info; }

        /**
         * @brief Set the info object
         * 
         * @param in Audio info struct
         *
         */
        void set_info(std::shared_ptr<AudioInfo> inf) { this->info = std::move(inf); }
};
