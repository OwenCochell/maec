/**
 * @file sink_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for sink modules
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sink_module.hpp"

void SinkModule::meta_process() {

    // Iterate a number of times based upon our period

    for(int i = 0; i < this->periods; ++i) {

        // Call the module behind us:

        this->get_backward()->meta_process();

        // Claim it's buffer

        buff = this->get_backward()->get_buffer();

        // Process ? :

        this->process();
    }
}

void SinkModule::info_sync() {

    // Configure the AudioInfo:

    this->get_info()->from_chain(this->chain_instance);
}
