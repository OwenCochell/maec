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

}


/**
 * @brief An audio module that mixes up (1:n relationship)
 * 
 */
class ModuleMixUp : public AudioModule {

}