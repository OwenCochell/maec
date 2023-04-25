/**
 * @file source_module.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for generating audio
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains base components for generating audio,
 * which we will call source modules.
 * Audio sources can be many things, such as reading from a file,
 * network stream, generating audio data from scratch, ect.
 * 
 */

#pragma once

#include "audio_module.hpp"

/**
 * @brief Source module, base class for generating audio data
 * 
 * A source is a component that generates audio data.
 * How it goes about generating this data can vary greatly,
 * but can include reading from a file, network stream,
 * microphone, ect.
 * These components are usually the first components in the chain,
 * also known as left most of back most modules.
 * 
 * The biggest difference between source modules
 * and conventional modules is that we will NEVER process back modules.
 */
class SourceModule : public AudioModule {

    public:

        /**
         * @brief Process this module, does not back process
         * 
         * This method is identical to the conventional
         * audio module counterpart,
         * except that we don't process any back modules.
         * 
         */
        void meta_process() override;
};