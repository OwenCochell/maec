/**
 * @file alsa_output.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for ALSA Output
 * @version 0.1
 * @date 2022-10-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ALSA_F

#include "alsa_output.hpp"

void DeviceInfo::create_device(void** hint, int id) {

    // Extract the necessary info:

    char* n = snd_device_name_get_hint(*hint, "NAME");
	char* descr = snd_device_name_get_hint(*hint, "DESC");
	char* io = snd_device_name_get_hint(*hint, "IOID");

    // Set name and description:

    this->name = std::string(n);
    this->description = std::string(descr);

    // Determine the IO type:

    if (io == NULL) {

        // We are both input and output:

        this->input = true;
        this->output = true;
    }

    else {

        // Not null, convert to string and compare:

        std::string temp(io);

        if (temp == "Output") {

            this->output = true;
        }

        else if (temp == "Input")
        {
            this->input = true;
        }
    }

    // Do some freeing:

    free(n);
    free(descr);
    free(io);

    // Finally, set our id:

    this->id = id;

}

int ALSABase::get_device_count() {

    // Define some values:

    void** hints;
    void** n;

    // Get array of all devices:

    int err = snd_device_name_hint(-1, "pcm", &hints);

    n = hints;

    // Iterate until we reach the end:

    int num = 0;

    for (; *n != nullptr; ++n) {

        ++num;

    }

    // Free the hints, as they are not needed:

    snd_device_name_free_hint(hints);

    // Finally, return the number of devices:

    return num;

}

DeviceInfo ALSABase::get_device_by_id(int index) {

    // Define some values:

    void** hints;
    void** n;

    // Get specific device:

    int err = snd_device_name_hint(-1, "pcm", &hints);

    // Ensure the given index is within our count:
    // Note: Using get_device_count() ensures we aren't repeating ourselves.
    // but is it the most efficient? Probably not...
 
    if (index >= this->get_device_count()) {
        // TODO: DO SOMETHING!
        // NOT DOING SOMETHING WILL RESULT IN A SEG FAULT!!!!
    }

    // Otherwise, seek n to the proper value:

    n = hints + index;

    // Create struct and initialize it:

    DeviceInfo info(n, index);

    // Free the hint:

    snd_device_name_free_hint(hints);

    // Finally, return the device info:

    return info;

}

DeviceInfo ALSABase::get_device_by_name(std::string name) {

    // Define some values:

    void** hints;
    void** n;

    // Get all devices:

    int err = snd_device_name_hint(-1, "pcm", &hints);

    n = hints;

    // Iterate over each device:

    int id = 0;
    DeviceInfo info;

    for (; *n != NULL; ++n) {

        // Create a DeviceInfo class:

        info = DeviceInfo(n, id);

        // Determine if the name is matching:

        if (name == info.name) {

            // Found it, break
            // TODO: Implement some kind of error correction
            // For now we assume all provided names are valid which is DANGEROUS!

            break;
        }

        // Increment our index

        ++id;
    }

    // Free the hints:

    snd_device_name_free_hint(hints);

    // Finally, return the device:

    return info;
}

void ALSABase::start(int sample_rate, int buffer_size) {

    // TODO: Implement error checking and correction!

    // Create the PCM device:

    int err = snd_pcm_open(&this->pcm, this->device.name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    // Set the options:
    // TODO: Figure out how we sample the AudioInfo struct!

    snd_pcm_hw_params_alloca(&this->params);

    // Now configure some options:
    // TODO: Figure out all of this:

    snd_pcm_hw_params_any(pcm, this->params);

    // TODO: Figure out if wrong access will mess things up...

    snd_pcm_hw_params_set_access(pcm, this->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm, this->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm, this->params, 1);
	snd_pcm_hw_params_set_rate(pcm, this->params, sample_rate, 0);
	snd_pcm_hw_params_set_periods(pcm, this->params, 1, 0);
    snd_pcm_hw_params_set_period_size(pcm, this->params, buffer_size, 0);
	snd_pcm_hw_params_set_period_time(pcm, this->params, 100000, 0); // 0.1 seconds period time

}

void ALSABase::stop() {

    // Close the PCM device:

    snd_pcm_close(this->pcm);

    // Free the params:

    snd_pcm_hw_params_free(this->params);

}

void ALSASink::process() {

    // Output the audio data:

    snd_pcm_writei(this->pcm, this->buff.get(), AudioModule::get_info()->buff_size);

}

void ALSASink::start() {

    // First, upcall:

    ALSABase::start(this->get_info()->sample_rate, this->get_info()->buff_size);

    // Next, set the new period number:

    snd_pcm_hw_params_set_periods(pcm, this->params, this->get_period(), 0);

}

#endif
