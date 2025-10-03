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
#include <type_traits>
#include <utility>

#include "audio_buffer.hpp"
#include "base_module.hpp"
#include "meta.hpp"

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
template<typename B = BaseModule*, typename F = BaseModule*>
class AudioModule : public BaseModule {
private:

    /// Gets qualifier free type of backwards module
    using BV = remove_qualifiers<B>;

    /// Gets qualifer free type of forwards module
    using FV = remove_qualifiers<F>;

    /// Module instance behind us
    B forwardv;

    /// Module instance infront of us
    F backwardv;

protected:
    /// Pointer to the audio buffer we are working with
    std::unique_ptr<AudioBuffer> buff = nullptr;

public:

    /// Backward type
    using BT = B;

    /// Forward type
    using FT = F;

    /// Default Constructor
    AudioModule() = default;

    /// Destructor
    ~AudioModule() override = default;

    /// Copy Constructor
    AudioModule(const AudioModule&) = delete;

    /// Move Constructor
    AudioModule(AudioModule&&) noexcept = default;

    /// Copy assignment operator
    AudioModule& operator=(const AudioModule&) = delete;

    /// Move assignment operator
    AudioModule& operator=(AudioModule&&) = default;

    FV& forward() requires (std::is_pointer_v<F>) {

        // In this case, simply dereference and return

        return (*forwardv);
    }

    FV& forward() requires (std::is_class_v<F> || std::is_reference_v<F>) {

        // In this case, we already have a reference
        // or a base type, just return it

        return forwardv;
    }

    void set_forward() {

        // TODO: Implement this!
        // Set the forward module:

        // this->forward = mod;
    }

    BV& backward() requires (std::is_pointer_v<B>) {

        // In this case, simply dereference and return

        return (*backwardv);
    }

    BV& backward() requires (std::is_class_v<B> || std::is_reference_v<F>) {

        // In this case, we already have a reference
        // or a base type, just return it

        return backwardv;
    }

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
    void meta_process() override { // NOLINT(misc-no-recursion): No recursion cycles present, valid chains will eventually end
        // Call the module behind us:

        this->backward().meta_process();

        // Grab the buffer from the module behind us:

        this->set_buffer(this->backward().get_buffer());

        // Call the processing module of our own:

        this->process();
    }

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
    void meta_start() override {

        // Ask the previous module to start:

        this->backward().meta_start();

        // Set our state:

        BaseModule::start();

        // Start this current module:

        this->start();
    }

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
    void meta_stop() override {

        // Yeah, just ask for previous module to stop:

        this->backward().meta_stop();

        // Set our state:

        BaseModule::stop();

        // Stop this current module:

        this->stop();
    }

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
    void meta_finish() override {

        // Ask backward module to stop:

        this->backward().meta_finish();

        // Set our state:

        BaseModule::finish();

        // Stop this current module:

        this->finish();
    }

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
    void info_sync() override {

        // By default, copy AudioInfo from front:
        // This is ugly? Maybe we should return a reference instead...

        *(this->get_info()) = *(this->forward().get_info());
    }

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
    void meta_info_sync() override {

        // Preform the current info sync:

        this->info_sync();

        // Preform backwards meta sync:

        this->backward().meta_info_sync();

    }

    /**
     * @brief Custom AudioModule done method
     *
     * This method simply reports our finish state to the chain.
     * It is REQUIRED for children to call this method,
     * as the chain will continue forever without proper module done reporting.
     *
     */
    void done() override {

        // Call the parent done method

        BaseModule::done();
    }

    /**
     * @brief Custom AudioModule finish method
     *
     * This method by default calls our done method immediately.
     * Any complicated modules may overload this method
     * and place custom finish logic here.
     *
     */
    void finish() override {

        // Call done immediately:

        this->done();
    }

    // TODO: Maybe move buffer functions to base class?

    /**
     * @brief Set the buffer object
     *
     * This method is called when modules are attempting to set the buffer
     * for this audio module.
     *
     * @param inbuff Pointer to an audio buffer
     */
    void set_buffer(std::unique_ptr<AudioBuffer> inbuff) { 

        // Set our buffer:

        this->buff = std::move(inbuff);
    }

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
    std::unique_ptr<AudioBuffer> get_buffer() override {

        // Return our buffer:

        return std::move(this->buff);
    }

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
    virtual AudioModule* bind(AudioModule* mod) {

        // Simply attach the pointer to ourselves:
        // TODO: This is no good now...

        this->backwardv = mod;

        // Set the forward module:

        mod->set_forward(this);

        // Set the chain info in the back to ours:
        // TODO: Really need to find a more robust way of doing this!
        // If a chain is added to another, previous ChainInfo pointers will not be updated!

        mod->set_chain_info(this->get_chain_info());

        return mod;
    }

    virtual AudioModule* link(B mod) {

        // Set our backwards module to this value

        this->backwardv = mod;

        // Get the NEW standardized backwards module

        auto& nmod = this->backward();

        // Determine if they expect a pointer or an instance

        if constexpr (std::is_pointer_v<typename B::FT>) {

            // They want a pointer, give them our address

            nmod.set_forward(this);
        }

        else {
            
            // Give them a class instance

            nmod.set_forward(*this);
        }
    }

    /**
     * @brief Set the forward module
     *
     * This sets the pointer to the forward audio module.
     * This method is usually called by the module we
     * are binding to.
     *
     * @param mod The module to set as forward
     */
    virtual void set_forward(F mod) {

        forwardv = mod;
    }

    virtual void set_forward(F&& mod) {

        forwardv = std::move(mod);
    }
};
