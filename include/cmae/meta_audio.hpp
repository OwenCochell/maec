/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains various meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "audio_module.hpp"


/**
 * @brief This module does not sample any backwards modules!
 * 
 * This module will NEVER sample any backwards modules.
 * A good use for this is a module that is at the start of a chain
 * and does not sample any other modules.
 *
 * For example, all oscillators inherit this class,
 * as they produce audio data, not alter it.
 * 
 */
class BackStop : public AudioModule {

    public:

        /**
         * @brief BackStop meta process method
         * 
         * We do the same thing as the AudioModule class,
         * except that we don't call the meta_process() 
         * method on backwards modules!
         * 
         */
        void meta_process() override;
};
