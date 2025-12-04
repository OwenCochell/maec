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

#include <type_traits>

#include "base_module.hpp"
#include "meta.hpp"

/**
 * @brief Concept that enforces maec module types
 *
 * This concept ensures a type is a valid maec module.
 * Mainly, this ensures that a type is a base class of
 * BaseModule, which is a HARD maec module requirement.
 *
 * We also allow pointer and reference types,
 * which will be reduced to a qualifier free type
 * for checking.
 *
 * Any component that utilizes template metaprograming
 * and is taking in compile time maec module types
 * (such as static chains),
 * should use this concept to enforce compile time checks
 * and ensure the incoming types are valid and safe.
 *
 * @tparam T Type to check
 */
template <typename T>
concept maecm =
    std::is_base_of_v<BaseModule, remove_qualifiers<T>> || std::is_void_v<T>;

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
template <typename C = void, maecm B = BaseModule*>
class AudioModule : public BaseModule {
public:
    /// Backward type
    using BT = B;

    /// Gets qualifier free type of backwards module
    using BV = remove_qualifiers<BT>;

    /// Type of child module
    using CT = C;

    /// Default Constructor
    AudioModule() = default;

    /// Destructor
    ~AudioModule() override = default;

    /// Copy Constructor
    AudioModule(const AudioModule&) = default;

    /// Move Constructor
    AudioModule(AudioModule&&) noexcept = default;

    /// Copy assignment operator
    AudioModule& operator=(const AudioModule&) = default;

    /// Move assignment operator
    AudioModule& operator=(AudioModule&&) = default;

    /**
     * @brief Gets a reference to the backward module
     *
     * This function is only enabled in dynamic chains
     * where the backwards module is a pointer.
     * We simply de-reference and return a reference.
     */
    BV& backward()
        requires(std::is_pointer_v<BT>)
    {

        // In this case, simply dereference and return

        return *backwardv;
    }

    /**
     * @brief Gets a reference to the backward module
     *
     * This function is only enabled in static chains
     * where the backwards module is known and is an instance.
     * We simply create and return a reference to the backwards module.
     */
    BV& backward()
        requires(std::is_class_v<BT> || std::is_reference_v<BT>)
    {

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
     * We also include some logic for preforming
     * explicit de-virtualization, as well as explicitly
     * calling target functions in static chains.
     *
     * Most users will not need to alter the code in this module,
     * but some advanced modules will need to, such as the audio mixers.
     */
    void meta_process()
        override {  // NOLINT(misc-no-recursion): No recursion cycles present,
                    // valid chains will eventually end

        // We have a separate path here depending on the backwards module type,
        // if the backwards type is known then we can directly call it's meta
        // processing method

        if constexpr (!std::is_pointer_v<BT>) {

            // Backward instance is NOT pointer,
            // operate directly on instance!

            backwardv.meta_process();

            // Grab the buffer

            this->set_buffer(backwardv.get_buffer());
        }

        else {

            // Call the module behind us using normal method

            this->backward().meta_process();

            this->set_buffer(this->backward().get_buffer());
        }

        // Now, we need to define a seperate path here if we know the child
        // type, if so we can preform some manual de-virtualization by
        // explicitly calling the process method

        if constexpr (std::is_void_v<CT>) {

            // The child module is NOT specified,
            // so just use the generic case.
            // This will at worst preform a virtual call,
            // but if defined correctly it could be optimized out.

            this->process();
        } else {
            // Attempt explicit de-virtualization. If CT is still incomplete
            // this qualified call is ill-formed. In this case,
            // users should rely on virtual dispatch or ensure CT definition
            // precedes use.
            static_cast<CT*>(this)->CT::process();
        }
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
     * An info sync should ONLY be preformed until the chain is completely
     * linked! Otherwise, any additions or removals could lead to runtime
     * issues. Best practice is asking the sink to preform a meta info sync
     * after you have constructed the chain.
     *
     * By default, we directly mirror the AudioInfo from the forward module.
     * Again, modules can overload this method and preform any action here.
     * As a note for users, if one wants to alter the AudioInfo,
     * they should do so AFTER the module is linked to the chain,
     * otherwise their changes may be destroyed by the info sync.
     * In addition, we also preform the chain info sync here,
     * which allows for large chain operations to still be valid after an info
     * sync. Finally, we also increment the number of modules attached in the
     * chain info. This allows for the module count to be accurate even after
     * large chain changes.
     *
     * It is HIGHLY recommended to call this parent function
     * if you intend to overload and implement your own functionality!
     * We preform some very important housekeeping here,
     * and failing to preform these actions could lead to trouble!
     */
    void info_sync() override {

        // By default, copy AudioInfo from front:
        // This is ugly? Maybe we should return a reference instead...

        *(this->get_info()) = *(this->forward()->get_info());

        // Also, copy the chain info from the front as well

        this->set_chain_info(this->forward()->get_chain_info());

        // Increment the number of modules in the chain

        this->get_chain_info()->module_num++;
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

        // Determine if we are a static chain,
        // if so then we may need to define the forward module for the backward
        // module

        if constexpr (!std::is_pointer_v<BT>) {

            // Backwards module is static, set the forward module

            this->backwardv.forward(this);
        }

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

    /**
     * @brief Links another module to us
     *
     * The process of 'linking' creates a relationship between
     * ourselves and the module behind us.
     * This relationship describes the processing order of the modules,
     * and modifies the sequence of operations preformed when using the chain.
     *
     * This method links a module to us,
     * and makes that the source of audio buffers.
     * We also set ourselves as the forward module.
     * Most users can use the default method here with no problem,
     * but some advanced modules may put some custom code here.
     *
     * We also return the AudioModule we added, allowing for operations like
     * this:
     *
     * mod.bind(mod1.bind(mod2.bind(mod3)));
     *
     * or:
     *
     * mod.link(mod1)->link(mod2)->link(mod3);
     *
     * The result would be this:
     *
     * mod3 -> mod2 -> mod1 -> mod
     *
     * @param mod The module to bind to us
     * @return BaseModule* The AudioModule we just bound
     */
    BaseModule* link(BaseModule* mod) override {

        // Disable this body if we are NOT a BaseModule

        if constexpr (!std::is_same_v<BT, BaseModule*>) {

            // TODO: Fire an assertion warning here

            // Just quit

            return nullptr;
        } else {

            // Set our backwards module to this value

            this->backwardv = mod;

            // Get the NEW standardized backwards module

            auto& nmod = this->backward();

            // Give them a pointer to ourselves

            nmod.forward(this);

            // Return pointer to backwards value

            return (&this->backward());
        }
    }

    BV* link_static(BT&& mod)
        requires(!std::is_pointer_v<BT>)  // We DON'T want to work with pointers
    {

        // TODO: Forward pointer values will differ if not linked correctly!

        // Set our backwards module to this value

        this->backwardv = std::move(mod);

        // Get the NEW standardized backwards module

        auto& nmod = this->backward();

        // Give them a pointer to ourselves

        nmod.forward(this);

        // Return pointer to backwards value

        return &this->backward();
    }

private:
    /// Module instance behind us
    BT backwardv{};
};
