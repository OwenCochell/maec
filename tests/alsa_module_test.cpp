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

#include <gtest/gtest.h>
#include "alsa_module.hpp"
#include "base_oscillator.hpp"

TEST(ALSABaseTest, Construct) {

    ALSABase alsa;
}

TEST(ALSABaseTest, DeviceSelection) {

    // Unfortunately, all assertions may not be reached
    // on all systems if there are no ALSA devices.

    // Create an TODO: PUT NAME!!!!

    ALSABase alsa;

    // Get the number of devices:

    int total = alsa.get_device_count();

    // Now, iterate over the devices:

    for (int i = 0; i < total; ++i) {

        // Get the device at this index:

        DeviceInfo dev = alsa.get_device_by_id(i);

        // Ensure that the device name maps back to ID:

        ASSERT_EQ(i, alsa.get_device_by_name(dev.name).id);
    }

}

TEST(ALSABaseTest, DeviceChangers) {

    // Again, all assertions may not be reached here,
    // as the user may not have enough devices

    // Create an TODO: Put name

    ALSABase alsa;

    // If we don't have enough devices, then this test is worthless...

    if (alsa.get_device_count() < 2) { return; }

    // Ensure default device is correct:

    ASSERT_EQ(alsa.get_device().name, "default");

    // Next, get a device 

    DeviceInfo temp = alsa.get_device_by_id(0);

    alsa.set_device(temp);

    // Ensure that the set device is valid:

    ASSERT_EQ(alsa.get_device().name, temp.name);
    ASSERT_EQ(alsa.get_device().id, temp.id);
    ASSERT_EQ(alsa.get_device().description, temp.description);

}

TEST(ALSABaseTest, Start) {

    // Create an ALSABase class:

    ALSABase alsa;

    // See if the start method works:

    alsa.alsa_start(44100, 440);

}

TEST(ALSASinkTest, Construct) {

    // Create an ALSASink class:

    ALSASink sink;
}

TEST(ALSASinkTest, Start) {

    // Create an ALSASink class:

    ALSASink sink;

    // Call the start method:

    sink.start();
}

TEST(ALSASinkTest, Output) {

    // Create an ALSASink class:

    ALSASink sink;

    // Set the device to the 'null' device:

    sink.set_device("null");

    // Create a ConstantOscillator:

    ConstantOscillator oconst(0.5);

    // Bind the modules together:

    sink.bind(&oconst);

    // Now, meta process the module:

    sink.meta_process();

}

#endif
