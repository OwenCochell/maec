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

#include "dsp/const.hpp"

#ifdef ALSA_F

#include "io/alsa_module.hpp"
#include "fund_oscillator.hpp"
#include "filter_module.hpp"
#include "meta_audio.hpp"

int main() {

    // Create the ALSASink

    std::cout << "Creating sink ..." << std::endl;

    ALSASink sink;

    // Create a test module:

    std::cout << "Creating saw oscillator ..." << std::endl;

    SineOscillator saw(440.0);

    // Bind the modules:

    std::cout << "Binding the modules ..." << std::endl;

    sink.bind(&saw);

    // Meta sync and start the chain:

    sink.meta_info_sync();
    sink.meta_start();

    // Temporary buffer size - Prevent underruns? TAKE INTO ACCOUNT THE SIZE OF THE FORMAT!

    // sink.get_info()->out_buffer = 524288 / 4;
    // saw.get_info()->out_buffer = 524288 / 4;
    //saw.get_info()->out_buffer = (sink.get_info()->out_buffer);
    //saw.get_info()->out_buffer = sink.get_info()->out_buffer;

    // Get expected period time:
 
    // Finally, meta process forever!

    // for(int i = 0; i < 5; i++) {
    while (true) {
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
