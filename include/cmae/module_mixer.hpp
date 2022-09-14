/**
 * @file module_mixer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tools for working with many modules
 * @version 0.1
 * @date 2022-09-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <vector>
#include "audio_module.hpp"


/**
 * @brief An audio module that mixes down (n:1 relationship)
 * 
 * This class allows for multiple modules to be mixed down into one,
 * allowing for multiple modules to be combined into one buffer
 * for another module to process.
 * 
 * This relationship is n:1, meaning their are an arbitrary number of inputs,
 * but exactly one output.
 * This relationship can be represented as:
 * 
 * module --+
 *          |
 * module --+--> module
 *          |
 * module --+
 * 
 * In this case, the output from each module is combined,
 * and that result is sent to the one module we are attached to.
 * 
 * Their are some minor performance issues to keep in mind when using this module.
 * Each input module will be sampled and their outputs will be added together.
 * If the input modules are sufficiently complex, 
 * then their will be overhead while waiting for the output to be computed.
 * This will be an issue the more modules you add.
 * Adding together the outputs is minimal, but again,
 * the more modules that are added the more complex this gets.
 * 
 * Please note, the term 'mixing' in this case is not related to conventional 
 * audio mixing, which usually involves combining channels and tracks.
 * While each module could be interpreted as a track,
 * it would not be correct as their are many exceptions to this rule.
 * So, in this case, the only thing we are mixing is the audio buffers 
 * each input module provides.
 * 
 * This class is an AudioModule, so it can be manipulated and worked with
 * like any conventional audio module.
 * The only processing we do on the given buffer(s) is to add them together.
 */
class ModuleMixDown : public AudioModule {

    private:

        /// Vector of pointers to all input modules
        std::vector<AudioModule*> in;

        /// Vector of all input buffers
        std::vector<std::unique_ptr<long double>> buffs;

    public:

        /**
         * @brief Processes the incoming audio data
         * 
         * We simply sample each input module,
         * and add the given buffers together.
         * 
         */
        void process() override;

        /**
         * @brief Set the buffer object
         * 
         * We add the incoming buffer to a vector containing
         * all incoming buffers.
         * 
         * We will then use this data in process() which will 
         * then be mixed together and forwarded to our output module.
         * 
         * @param inbuff Buffer to add to this module
         */
        void set_buffer(std::unique_ptr<long double> inbuff) override;

        /**
         * @brief Binds a module to us
         * 
         * We do something a little differently when compared to normal.
         * Instead of setting one pointer for one input,
         * we contain a vector of pointers for each module bound to us.
         */
        void bind(AudioModule* mod) override;

};


/**
 * @brief An audio module that mixes up (1:n relationship)
 * 
 * This class allows for multiple modules to be mixed up into many,
 * allowing for one input to be sent to multiple output modules.
 * 
 * This relationship is 1:n, meaning their is one input, and an arbitrary number of outputs.
 * This relationship can be represented as:
 * 
 *          +--> module
 *          |
 * module --+--> module
 *          |
 *          +--> module
 * 
 * In this case, the output from the input module is copied and shared amongst
 * the output modules we are attached to.
 * 
 * This module has some performance/memory issues to keep in mind.
 * Conventionally, one audio buffer is shared between modules as it works its way up the chain.
 * This is very memory efficient, as only one buffer will live in memory for each chain.
 * This module changes that, as we make copies of the input buffer and pass it along to our output modules.
 * This means more buffers living in memory. 
 * Plus, if the buffersize is large enough, then the copy operation may take time.
 * 
 * Please note, the term 'mixing' in this case is not related to conventional 
 * audio mixing, which usually involves combining channels and tracks.
 * While each module could be interpreted as a track,
 * it would not be correct as their are many exceptions to this rule.
 * So, in this case, the only thing we are mixing is the audio buffers 
 * each input module provides.
 * 
 * This class is an AudioModule, so it can be manipulated and worked with
 * like any conventional audio module.
 * The only processing we do on the given buffer(s) is to add them together.
 */
class ModuleMixUp : public AudioModule {

    private:

        /// Vector of out modules
        std::vector<AudioModule*> out;

    public:

        /**
         * @brief Meta process method
         * 
         * We are identical to the parent meta process method,
         * but we instead duplicate and route
         * the audio buffer we get from input methods
         * and send it along to output modules.
         * 
         */
        void meta_process() override;

        /**
         * @brief Set the forward object
         * 
         * We are identical to the parent forward method,
         * except that we keep multiple pointers of the modules
         * in front of us.
         * 
         * @param mod 
         */
        void set_forward(AudioModule* mod) override;

};
