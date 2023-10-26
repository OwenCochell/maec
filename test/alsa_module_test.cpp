/**
 * @file alsa_output_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for the ALSABase class
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ALSA_F

#include <catch2/catch_test_macros.hpp>

#include "io/alsa_module.hpp"
#include "base_oscillator.hpp"

TEST_CASE("ALSABase Test", "[alsa]") {

    // Construct the ALSABAse:

    ALSABase alsa;

    SECTION("Device Selection", "Ensures we can iterate over the devices and query them") {

        // Get the number of devices:

        int total = alsa.get_device_count();

        // Now, iterate over the devices:

        for (int i = 0; i < total; ++i) {

            // Get the device at this index:

            DeviceInfo dev = alsa.get_device_by_id(i);

            // Ensure that the device name maps back to ID:

            REQUIRE(dev.id == alsa.get_device_by_name(dev.name).id);
        }
    }

    SECTION("Device Changers", "Ensures we can set the device to utilize") {

        // Again, all assertions may not be reached here,
        // as the user may not have enough devices

        // If we don't have enough devices, then this test is worthless...

        if (alsa.get_device_count() < 2) {
            return;
        }

        // Ensure default device is correct:

        REQUIRE(alsa.get_device().name == "default");

        // Next, get a device

        DeviceInfo temp = alsa.get_device_by_id(0);

        alsa.set_device(temp);

        // Ensure that the set device is valid:

        REQUIRE(alsa.get_device().name == temp.name);
        REQUIRE(alsa.get_device().id == temp.id);
        REQUIRE(alsa.get_device().description == temp.description);
    }

    SECTION("Start", "Ensures the ALSABase can be started") {

        // Start the ALSABase:

        alsa.alsa_start(44100, 440);
    }
}

TEST_CASE("ALSASink Test", "[alsa][sink]") {

    // Create an ALSASink:

    ALSASink sink;

    SECTION("Start", "Ensures the ALSASink can be started") {

        sink.start();

        SECTION("Stop", "Ensures the ALSASink can be stopped") { sink.stop(); }
    }

    SECTION("Output", "Ensures we can output, just to null device") {

        // Set the device to the 'null' device:

        sink.set_device("null");

        // Create a ConstantOscillator:

        ConstantOscillator oconst(0.5);

        // Bind the modules together:

        sink.bind(&oconst);

        // Start the sink:

        sink.start();

        // Now, meta process the module:

        sink.meta_process();
    }
}

#endif
