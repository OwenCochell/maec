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

#ifdef ALSA_F

#include "alsa_module.hpp"
#include "fund_oscillator.hpp"
#include "filter_module.hpp"

int main(int argc, char** argv) {

    // Create the ALSASink

    std::cout << "Creating sink ..." << std::endl;

    ALSASink sink;

    // Create a test module:

    std::cout << "Creating saw oscillator ..." << std::endl;

    SawtoothOscillator saw(440.0);

    std::cout << "Creating filter module ..." << std::endl;

    SincFilter sinc;

    // Set type to lowpass:

    sinc.set_type(FilterType::LowPass);

    // Set low frequency to 440:

    sinc.set_start_freq(440.0);

    // Set the size:

    sinc.set_size(100);

    // Bind the modules:

    std::cout << "Binding the modules ..." << std::endl;

    sinc.bind(&saw);
    sink.bind(&sinc);

    // Start all modules:

    sink.start();
    saw.start();
    sinc.start();

    // Temporary buffer size - Prevent underruns? TAKE INTO ACCOUNT THE SIZE OF THE FORMAT!

    sink.get_info()->out_buffer = 524288 / 4;
    saw.get_info()->out_buffer = 524288 / 4;

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

#else

int main(int argc, char** argv) {

    std::cout << "ALSA Is not installed!" << std::endl;
}

#endif
