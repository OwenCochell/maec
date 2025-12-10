/**
 * @file base_module.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief This file contains components used for working with modules.
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "audio_buffer.hpp"
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
    double sample_rate = SAMPLE_RATE;

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
    double sample_rate = SAMPLE_RATE;

    /// Size of the incoming audio buffer
    int in_buffer = BUFF_SIZE;

    /// Size of the outgoing audio buffer
    int out_buffer = BUFF_SIZE;

    /// Channels of audio data
    int channels = 1;

    ModuleInfo() = default;

    ModuleInfo(ChainInfo cinfo)
        : sample_rate(cinfo.sample_rate),
          in_buffer(cinfo.buffer_size),
          out_buffer(cinfo.buffer_size),
          channels(cinfo.channels) {}

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
 * @brief Represents a module
 *
 * This class offers a framework for modules,
 * allowing them to be worked with and managed by higher level components.
 * This component will be used to represent all modules in maec,
 * and contains some functionality that ALL modules should utilize and
 * implement!
 *
 * A "module" is a class that has the following properties:
 *  1. Has a state (unloaded, loaded, started, stopped)
 *  2. Has processing methods (process, meta_process)
 *  3. Has state methods (start, stop, finish, done)
 *  4. Has info sync methods
 *  5. Has chain traversal methods (forward / backward)
 *  6. Has buffer exchange methods
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
 *  - Finishing - Module has been requested to stop, and should do any necessary
 * operations
 *  - Stopped - Module has been stopped and will no longer work with data
 *
 * The state can be identified by using the 'get_state()' method.
 * The state will automatically be set by each module as it is used.
 *
 * We also contain state methods that are called when necessary,
 * checkout 'start()', 'stop()', 'finish()', ect. for more info.
 *
 */
class BaseModule {
private:
    /// Information for this specific module
    ModuleInfo info;

    /// Information for the chain this module is apart of
    ChainInfo* chain = nullptr;

    /// Pointer to forward module
    BaseModule* forwardv = nullptr;

protected:
    /// Pointer to the audio buffer we are working with
    AudioBuffer buff;

public:
    /// Forward type, by default BaseModule pointer
    using FT = BaseModule*;

    /// The types of states we can be:
    enum class State : uint8_t {
        Created,
        Started,
        Finishing,
        Finished,
        Stopped
    };

    BaseModule() = default;

    virtual ~BaseModule() = default;

    /// We DON'T allow copies thanks to unique pointer
    BaseModule(const BaseModule&) = default;

    /// Move constructor for moving support
    BaseModule(BaseModule&&) = default;

    /// We DON'T allow copies thanks to unique pointer
    BaseModule& operator=(const BaseModule&) = default;

    /// Move assignment for moving support
    BaseModule& operator=(BaseModule&&) = default;

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
    virtual void meta_process() = 0;

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
    virtual void meta_start() = 0;

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
    virtual void meta_stop() = 0;

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
    virtual void meta_finish() = 0;

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
    virtual void info_sync() = 0;

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
    virtual void meta_info_sync() = 0;

    /**
     * @brief Set the buffer object
     *
     * This method is called when modules are attempting to set the buffer
     * for this audio module.
     *
     * @param inbuff Pointer to an audio buffer
     */
    void set_buffer(AudioBuffer&& inbuff);

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
    virtual AudioBuffer&& get_buffer();

    /**
     * @brief Creates an AudioBuffer
     *
     * Creates a buffer object and
     * returns the result.
     * The resulting pointer should be unique,
     * and have ownership of the buffer.
     *
     * We will automatically create a buffer with using
     * the size from the AudioInfo struct,
     * along with the sample rate and channel count.
     *
     * @return The newly created buffer
     */
    // std::unique_ptr<AudioBuffer> create_buffer(int channels = 1);

    void reserve();

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
    // static std::unique_ptr<AudioBuffer> create_buffer(int size, int
    // channels);

    void reserve(int size, int channels);

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
    // virtual BaseModule* bind(BaseModule* mod);

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
     * However, some modules may do things after the chain is asked to be
     * stopped, such as fade out in the case of an ADSR envelope.
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
     * This tells the collection using this module that we are ready to be
     * stopped. If all modules are not marked as done, then the module chain
     * will not stop!
     */
    virtual void done();

    /**
     * @brief Get the state of this module
     *
     * We return the state of this module.
     *
     * @return State An integer representing the state
     */
    State get_state() { return this->state_type; }

    /**
     * @brief Set the forward module
     *
     * This sets the pointer to the forward audio module.
     * This method is usually called by the module we
     * are binding to.
     *
     * @param mod The module to set as forward
     */
    virtual void forward(BaseModule* mod) { forwardv = mod; }

    virtual BaseModule* forward() { return forwardv; }

    virtual BaseModule* link(BaseModule* mod) { return mod; };

private:
    /// The state type of this module:

    State state_type = State::Created;
};
