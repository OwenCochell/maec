/**
 * @file audio_module.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief This file contains module components for working with audio data.
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <memory>

#include "audio_buffer.hpp"
#include "base_module.hpp"
#include "const.hpp"

/**
 * @brief Structure for holding information about an AudioChain
 *
 * This struct contains many attributes that module chains can utilize.
 * AudioModules and AudioChains are not required to consider this information!
 * However, good chains/modules WILL take this info into consideration.
 *
 * This values in this struct describes how audio data LEAVES the chain!
 * For example, the buffer_size will be the size of the buffer as it reaches the
 * sink. Modules (such as sinks) can use this info to self-configure themselves.
 * Other modules can alter this data to affect how sinks configure themselves.
 *
 * This data will be shared between ALL modules in the same chain.
 * The struct will be synced when modules are linked,
 * and will likely be used at start time by modules in the chain to configure
 * themselves.
 */
struct ChainInfo {

    /// Sample rate of audio data, if applicable
    long double sample_rate = SAMPLE_RATE;

    /// Size of buffer entering the sink
    int buffer_size = BUFF_SIZE;

    /// Number of audio channels
    int channels = 1;

    /// Number of modules in chain
    int module_num = 1;

    // Number of modules ready to stop:
    int module_finish = 0;
};

/**
 * @brief Structure for holding information about an AudioModule
 *
 * This struct contains many attributes that modules can utilize.
 * AudioModules are under no obligation to consider this information.
 * However, good modules WILL take this info into consideration,
 * and modules attached to this one will use this info.
 *
 * The info contained here represents the state of a single module.
 * Other modules can use this information to understand how to interact
 * with modules, and may allow other modules to alter the parameters described
 * here.
 */
struct ModuleInfo {

    /// The sample rate of the audio data, if applicable
    long double sample_rate = SAMPLE_RATE;

    /// Size of the incoming audio buffer
    int in_buffer = BUFF_SIZE;

    /// Size of the outgoing audio buffer
    int out_buffer = BUFF_SIZE;

    /// Channels of audio data
    int channels = 1;

    ModuleInfo() = default;

    ModuleInfo(ChainInfo cinfo) : sample_rate(cinfo.sample_rate), in_buffer(cinfo.buffer_size), out_buffer(cinfo.buffer_size), channels(cinfo.channels) {}

    /**
     * @brief Configures the ModuleInfo from ChainInfo
     * 
     * We simply grab the values from ChainInfo
     * and sets them in this instance.
     * 
     * @param cinfo ChainInfo to get data from
     */
    void from_chain(ChainInfo& cinfo) {

        sample_rate = cinfo.sample_rate;
        in_buffer = cinfo.buffer_size;
        out_buffer = cinfo.buffer_size;
        channels = cinfo.channels;
    }
};

/**
 * @brief Module for working with audio data
 *
 * These modules are designed to work with audio data.
 * Audio modules can be attached to each other to form a chain,
 * and audio data will flow them as they are linked.
 * A module chain is essentially a complicated doubly linked list!
 *
 * Audio modules are share a pointer to an 'audio_info' struct,
 * which will be used to share information between modules.
 *
 * All audio modules must inherit this class!
 * 
 * TODO: Really fix this documentation
 */
class AudioModule : public BaseModule {

private:
    /// Information for this specific module
    ModuleInfo info;

    /// Information for the chain this module is apart of
    ChainInfo* chain = nullptr;

    /// Pointer to the audio module we are attached to
    AudioModule* forward = nullptr;

    /// Pointer to the audio module that is attached to us
    AudioModule* backward = nullptr;

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
    AudioModule() = default;

    /**
     * @brief Destroy the Audio Module object
     *
     */
    ~AudioModule() override = default;

    /**
     * @brief Copy constructor
     *
     */
    AudioModule(const AudioModule&) = delete;

    /**
     * @brief Copy assignment operator
     *
     * @return AudioModule&
     */
    AudioModule& operator=(const AudioModule&) = delete;

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
     * @brief Meta start method
     *
     * This method contains all meta start operations.
     * 'Meta' operations are not necessarily related to audio processing,
     * and are instead related to chain and module operations that
     * are required for normal operation.
     * We essentially set the stage for audio processing.
     * This method will:
     *
     * - Start the backward module
     * - Extract backward info and set it to ours
     * - Start current module
     *
     * This method is intended to prepare this module,
     * and other modules in the chain, for processing.
     * This allows for modules to recursively start one another,
     * and for module info to propagate through the chain.
     *
     * Most of the time, it is unecessary to
     * overload this method yourself
     * unless you are working with an advanced module.
     *
     * TODO: We should really hash out how chains are started in sinks.
     * I am happy to say that users should call 'meta_start()' in the sink.
     */
    virtual void meta_start();

    /**
     * @brief Meta stop method
     *
     * This method contains all meta stop operations.
     * This method will:
     *
     * - Stop the backward module
     * - Stop ourselves
     *
     * Yeah, that is about it.
     * This method is intended to prepare this module,
     * and other modules in the chain, for stopping.
     * This allows modules to recursively stop one another.
     *
     * Again, most of the time, it is unecessary to
     * overload this method yourself
     * unless you are working with an advanced module.
     *
     */
    virtual void meta_stop();

    /**
     * @brief Meta finish method
     *
     * This method contains all meta finish operations.
     * This method will:
     *
     * - Finish backward module
     * - Finish ourselves
     *
     * Again, not very crazy.
     * This method is intended to prepare this module,
     * and other modules in the chain, for finishing.
     *
     * Most of the time, it is unecessary to overload this method yourself
     * unless you are working with an advanced module.
     *
     */
    virtual void meta_finish();

    /**
     * @brief Determines the current AudioModule data
     * 
     * Preforms an info sync for this module.
     * Many modules configure themselves based upon
     * the info from forward modules.
     * For example, a source may determine it's output buffer size
     * based upon the in buffer size of forward module it is connected to.
     * 
     * This method should contain functionality that considers forward modules
     * and self-configures the current module.
     * This can be anything ranging from a static configuration,
     * to a dynamic configuration that does a lot of thinking.
     * 
     * One rule of best practice is that modules should really only consider
     * any forward modules directly in front of them.
     * Modules can traverse the chain and consider ChainInfo,
     * but this can lead to bugprone behavior.
     * 
     * This function is called automatically when modules are linked,
     * but it can be called anytime to preform a new info sync,
     * although it is recommended to do so BEFORE modules are started
     * to ensure the info is properly utilized.
     * 
     * By default, we directly mirror the AudioInfo from the forward module.
     * Again, modules can overload this method and preform any action here.
     * As a note for users, if one wants to alter the AudioInfo,
     * they should do so AFTER the module is linked to the chain,
     * otherwise their changes may be destroyed by the info sync.
     */
    virtual void info_sync();

    /**
     * @brief Meta info sync
     * 
     * This method preforms a chain-wide info sync
     * on all backwards modules.
     * This method is usually called for you automatically,
     * but one can invoke this method manually.
     * A good application for this method is
     * if you manually change the current module's info,
     * then you can call this method to sync all backwards modules.
     * 
     */
    virtual void meta_info_sync();

    /**
     * @brief Custom AudioModule done method
     *
     * This method simply reports our finish state to the chain.
     * It is REQUIRED for children to call this method,
     * as the chain will continue forever without proper module done reporting.
     *
     */
    void done() override;

    /**
     * @brief Custom AudioModule finish method
     *
     * This method by default calls our done method immediately.
     * Any complicated modules may overload this method
     * and place custom finish logic here.
     *
     */
    void finish() override;

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
     * so this should only be called by forward modules after processing is
     * complete!
     *
     * One other application for this is when the user defined process method
     * gets access to the buffer of the current module.
     * Keep in mind, the process method would then have to re-set the current
     * buffer using the set_buffer() method.
     * Therefore, it is recommended that you use the protected 'buff' pointer
     * instead.
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
     * We also specify if the AudioInfo should be synced between the two
     * modules.
     *
     * We also return the AudioModule we added, allowing for operations like
     * this:
     *
     * mod.bind(mod1.bind(mod2.bind(mod3)));
     * TODO: ^ This is less ideal, as AudioInfo structs are not synced,
     * elaborate on this
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
     * @brief Get the forward object
     *
     * We simply return the pointer to the forward module.
     *
     * @return AudioModule* Pointer to the forward module
     */
    AudioModule* get_forward() { return this->forward; }

    /**
     * @brief Creates an AudioBuffer
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
     * @return The newly created buffer
     */
    std::unique_ptr<AudioBuffer> create_buffer(int channels = 1);

    /**
     * @brief Creates an AudioBuffer
     *
     * Creates a buffer object and
     * returns the result.
     * The resulting pointer should be unique,
     * and have ownership of the buffer.
     *
     * @param size Size of buffer to create
     * @param channels Channels in AudioBuffer
     * @return std::unique_ptr<AudioBuffer> Newly created buffer
     */
    static std::unique_ptr<AudioBuffer> create_buffer(int size, int channels);

    /**
     * @brief Get the backward object
     *
     * We simply return the pointer to the backward module.
     *
     * @return AudioModule* Pointer to the backward module
     */
    AudioModule* get_backward() { return this->backward; }

    /**
     * @brief Get the module info object
     *
     * @return ModuleInfo* ModuleInfo struct in use by this class
     */
    ModuleInfo* get_info() { return &this->info; }

    /**
     * @brief Set the module info object
     *
     * @param in Audio info struct
     */
    void set_info(ModuleInfo& inf) { this->info = inf; }

    /**
     * @brief Get the chain info object
     *
     * @return ChainInfo struct in use by this class
     */
    ChainInfo* get_chain_info() const { return this->chain; }

    /**
     * @brief Set the chain info object
     *
     * @param inf Chain info struct
     */
    void set_chain_info(ChainInfo* inf) { this->chain = inf; }
};
