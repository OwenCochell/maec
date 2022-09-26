/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations of meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "meta_audio.hpp"

void BackStop::meta_process() {

    // Call the processing module of our own:

    this->process();

}