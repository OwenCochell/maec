/**
 * @file base_output.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for outputting audio
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * This file contains base components for outputting audio.
 * Audio output can be many things, such as a file, sound card, network stream, etc.
 * 
 * We aim to simplify this experience by providing frameworks for different types of output systems.
 */

#pragma once

#include "audio_module.hpp"

/**
 * @brief Base output, the base class for all output modules
 * 
 * TODO: Give a better description
 */
class BaseOutput : public AudioModule {

};
