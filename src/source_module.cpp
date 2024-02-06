/**
 * @file source_module.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for source modules
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "source_module.hpp"

void SourceModule::meta_process() {

    // Call the processing module of our own:

    this->process();
}
