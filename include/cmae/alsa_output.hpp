/**
 * @file alsa_output.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for outputting audio to an ALSA device
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * Here, we define the AlsaOutput class,
 * which allows incoming audio data to be outputted to an ALSA device.
 */

#pragma once

#ifdef ALSA_F

#include <alsa/asoundlib.h>
#include <string>
#include "base_output.hpp"

/**
 * @brief Outputs given data to an ALSA device
 * 
 * We offer an easy to use module for outputting audio to an ALSA device.
 * This works as any other module, meaning that modules can be bound to this one,
 * Back modules are sampled and outputted to the ALSA device.
 * 
 * We also offer some methods for selecting the ALSA device,
 * and other configuration options.
 * 
 * ALSA is a Linux only library, so this module will not work on Windows or Mac.
 * 
 * TODO: Determine if we should remove all of these getter and setters!
 */
class AlsaOutput : public BaseOutput {

    private:

        /// The ALSA PCM interface
        snd_pcm_t *pcm;

        /// The ALSA PCM hardware parameters
        snd_pcm_hw_params_t *params;

        /// The name of the device to use: 
        std::string device_name = "default";

        /// Number of periods per cycle
        int periods = 1;

    public:

        /**
         * @brief Construct a new Alsa Output object
         * 
         * We utilize create and utilize a default AudioInfo struct.
         */
        AlsaOutput() =default;

        /**
         * @brief Construct a new AlsaOutput object
         * 
         * This constructor will create a new AlsaOutput object.
         * It will initialize the ALSA device, and set the default parameters.
         * 
         * @param device The ALSA device to use
         * @param sample_rate The sample rate to use
         * @param channels The number of channels to use
         * @param buffer_size The buffer size to use
         */
        AlsaOutput(std::string device, int sample_rate, int channels, int buffer_size);

        /**
         * @brief Destroy the AlsaOutput object
         * 
         * This destructor will destroy the AlsaOutput object.
         * It will close the ALSA device.
         */
        ~AlsaOutput();

        /**
         * @brief Process the module
         * 
         * This method will process the module.
         * It will sample the back module, and output the data to the ALSA device.
         */
        void process() override;

        /**
         * @brief Set the sample rate
         * 
         * This method will set the sample rate.
         * 
         * @param sample_rate The sample rate to use
         */
        void set_sample_rate(int sample_rate);

        /**
         * @brief Set the number of channels
         * 
         * This method will set the number of channels.
         * 
         * @param channels The number of channels to use
         */
        void set_channels(int channels);

        /**
         * @brief Set the buffer size
         * 
         * This method will set the buffer size.
         * 
         * @param buffer_size The buffer size to use
         */
        void set_buffer_size(int buffer_size);

        /**
         * @brief Set the ALSA device
         * 
         * This method will set the ALSA device.
         * 
         * @param device The ALSA device to use
         */
        void set_device(std::string device);

        /**
         * @brief Get the sample rate
         * 
         * This method will get the sample rate.
         * 
         * @return int The sample rate
         */
        int get_sample_rate();

        /**
         * @brief Gets a device index via it's name
         * 
         * We return the index of a device given it's name.
         * 
         * @param name Name of the device 
         * @return int Index of the device
         */
        int get_device_index(std::string name);

        /**
         * @brief Gets a device name via it's index
         * 
         * We return the name of a device given it's index
         * 
         * @param index Index of the device
         * @return std::string Name of the device
         */
        std::string get_device_name(int index);

        /**
         * @brief Gets the number of devices we have to work with
         * 
         * We return the number of devices available for use 
         * 
         * @return int Number of devices available for use
         */
        int get_device_count();

        /**
         * @brief Starts this module.
         * 
         * We configure the necessary ALSA parameters,
         * and ensure everything is initialized.
         * 
         */
        void start() override;

        /**
         * @brief Stops this module.
         * 
         * We stop the ALSA components we are using,
         * and wait for the buffer to empty.
         * 
         */
        void stop() override;

};

#endif