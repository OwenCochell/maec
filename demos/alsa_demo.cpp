/**
 * @file alsa_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests ALSA modules
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include "alsa_module.hpp"
#include "fund_oscillator.hpp"

int main(int argc, char** argv) {

    // Create the ALSASink

    std::cout << "Creating sink ..." << std::endl;

    ALSASink sink;

    // Create a test module:

    std::cout << "Creating saw oscillator ..." << std::endl;

    SineOscillator saw(440.0);

    // Bind the modules:

    std::cout << "Binding the modules ..." << std::endl;

    sink.bind(&saw);

    // Start all modules:

    sink.start();
    saw.start();

    // Temporary buffer size - Prevent underruns? TAKE INTO ACCOUNT THE SIZE OF THE FORMAT!

    sink.get_info()->buff_size = 524288 / 4;
    saw.get_info()->buff_size = 524288 / 4;

    int dang = sizeof(long double);
    int dfloat = sizeof(float);
    int dumb = sizeof(char);
    // Finally, meta process forever!

    for(int i = 0; i < 5; i++) {

        std::cout << "Processing ..." << std::endl;

        sink.meta_process();

    }

    sink.stop();
    saw.stop();

}
