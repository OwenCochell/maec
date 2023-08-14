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
#include "meta_audio.hpp"

int main() {

    // Create the ALSASink

    std::cout << "Creating sink ..." << std::endl;

    ALSASink sink;

    // Create a test module:

    std::cout << "Creating saw oscillator ..." << std::endl;

    SineOscillator saw(440.0);

    std::cout << "Creating LatencyModule ..." << std::endl;

    LatencyModule latency;

    std::cout << "Creating filter module ..." << std::endl;

    SincFilter sinc;

    // Set type to lowpass:

    sinc.set_type(FilterType::LowPass);

    // Set low frequency to 440:

    sinc.set_start_freq(440.0);

    // Set the size:

    int kern_size = 50;

    sinc.set_size(kern_size);

    // Bind the modules:

    std::cout << "Binding the modules ..." << std::endl;

    sinc.bind(&saw);
    latency.bind(&sinc);
    sink.bind(&latency);
    // latency.bind(&saw);
    // sink.bind(&latency);

    // Start all modules:

    sink.start();
    saw.start();
    latency.start();
    sinc.start();

    // Temporary buffer size - Prevent underruns? TAKE INTO ACCOUNT THE SIZE OF THE FORMAT!

    // sink.get_info()->out_buffer = 524288 / 4;
    // saw.get_info()->out_buffer = 524288 / 4;
    saw.get_info()->out_buffer = (sink.get_info()->out_buffer - kern_size + 1);
    //saw.get_info()->out_buffer = sink.get_info()->out_buffer;

    // Get expected period time:

    int period_time = sink.get_device().period_time * 1000;

    // Finally, meta process forever!

    // for(int i = 0; i < 5; i++) {
    while (true) {
        std::cout << "Processing ..." << std::endl;

        sink.meta_process();

        std::cout << latency.latency() << std::endl;
        std::cout << latency.average_latency() << std::endl;
        std::cout << latency.total_latency() << std::endl;
        std::cout << latency.time() << std::endl;
        std::cout << latency.time() - period_time << std::endl;
        std::cout << period_time << std::endl;
    }

    sink.stop();
    saw.stop();
}

#else

int main(int argc, char** argv) {

    std::cout << "ALSA Is not installed!" << std::endl;
}

#endif
