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
#include "alsa_output.hpp"

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

#endif
