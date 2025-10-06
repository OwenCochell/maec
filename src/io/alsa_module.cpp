/**
 * @file alsa_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for ALSA Output
 * @version 0.1
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifdef ALSA_F

#include "io/alsa_module.hpp"

#include <alsa/control.h>
#include <alsa/pcm.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "audio_buffer.hpp"

void DeviceInfo::create_device(void** hint, int id) {

    // First, set our id:

    this->id = id;

    // Extract the necessary info:

    char* n = snd_device_name_get_hint(*hint, "NAME");
    char* descr = snd_device_name_get_hint(*hint, "DESC");
    char* io = snd_device_name_get_hint(*hint, "IOID");

    // Set name and description:

    this->name = std::string(n);
    this->description = std::string(descr);

    // Determine the IO type:

    if (io == nullptr) {

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

        else if (temp == "Input") {

            this->input = true;
        }
    }

    // Do some freeing:

    free(n);
    free(descr);
    free(io);

    // Now, get our device:

    snd_pcm_t* pcm = nullptr;

    int err =
        snd_pcm_open(&pcm, this->name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    if (err != 0) {

        // We have encountered an error, DO NOT CONTINUE!

        this->load_fail = true;

        return;
    }

    this->update();
}

void DeviceInfo::update() {

    // Allocate the hardware parameters:

    snd_pcm_hw_params_t* params = nullptr;

    snd_pcm_hw_params_alloca(&params);

    // Now, get our device:

    snd_pcm_t* pcm = nullptr;

    int err =
        snd_pcm_open(&pcm, this->name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    if (err != 0) {

        // We have encountered an error, DO NOT CONTINUE!

        this->load_fail = true;

        return;
    }

    // Fill the hardware parameters for our device:

    snd_pcm_hw_params_any(pcm, params);

    // Set some default values:

    int a = snd_pcm_hw_params_set_access(pcm, params,
                                         SND_PCM_ACCESS_RW_INTERLEAVED);

    // Determine if we should set values:

    if (this->channels != 0) {

        snd_pcm_hw_params_set_channels(pcm, params, this->channels);
    }

    if (this->sample_rate != 0) {

        snd_pcm_hw_params_set_rate(pcm, params, this->sample_rate, 0);
    }

    if (this->period != 0) {

        snd_pcm_hw_params_set_periods(pcm, params, this->period, 0);
    }

    if (this->period_size != 0) {

        snd_pcm_hw_params_set_period_size(pcm, params, this->period_size, 0);
    }

    // Commit the parameters:

    int commit = snd_pcm_hw_params(pcm, params);

    // Now, do some querying:

    snd_pcm_hw_params_get_periods_max(params, &this->period_max, 0);
    snd_pcm_hw_params_get_periods_min(params, &this->period_min, 0);
    snd_pcm_hw_params_get_period_size_max(params, &this->period_size_max, 0);
    snd_pcm_hw_params_get_period_size_min(params, &this->period_size_min, 0);
    snd_pcm_hw_params_get_channels(params, &this->channels);
    snd_pcm_hw_params_get_channels_max(params, &this->channels_max);
    snd_pcm_hw_params_get_channels_min(params, &this->channels_min);
    snd_pcm_hw_params_get_period_time_min(params, &this->period_time_min, 0);
    snd_pcm_hw_params_get_period_time_max(params, &this->period_time_max, 0);

    snd_pcm_hw_params_get_periods(params, &this->period, 0);
    snd_pcm_hw_params_get_period_size(params, &this->period_size, 0);
    snd_pcm_hw_params_get_period_time(params, &this->period_time, 0);

    snd_pcm_hw_params_get_rate(params, &this->sample_rate, 0);
    snd_pcm_hw_params_get_rate_max(params, &this->sample_rate_max, 0);
    snd_pcm_hw_params_get_rate_min(params, &this->sample_rate_min, 0);

    snd_pcm_hw_params_get_buffer_size(params, &this->buffer_size);
    snd_pcm_hw_params_get_buffer_size_max(params, &this->buffer_size_max);
    snd_pcm_hw_params_get_buffer_size_min(params, &this->buffer_size_min);

    // Do some freeing:

    snd_pcm_close(pcm);
    snd_pcm_hw_free(pcm);
}

int ALSABase::get_device_count() {

    // Define some values:

    void** hints = nullptr;
    void** n = nullptr;

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

    void** hints = nullptr;
    void** n = nullptr;

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

DeviceInfo ALSABase::get_device_by_name(const std::string& name) {

    // Define some values:

    void** hints = nullptr;
    void** n = nullptr;

    // Get all devices:

    int err = snd_device_name_hint(-1, "pcm", &hints);

    n = hints;

    // Iterate over each device:

    int id = 0;
    DeviceInfo info;

    for (; *n != nullptr; ++n) {

        // Get the name from the hint:

        std::string const hname(snd_device_name_get_hint(*n, "NAME"));

        // Determine if the name is our target:

        if (hname == name) {

            // Found it, break
            // TODO: Implement some kind of error correction
            // For now we assume all provided names are valid which is
            // DANGEROUS!

            info = DeviceInfo(n, id);

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

void ALSABase::alsa_start() {

    // TODO: Implement error checking and correction!

    // Create the PCM device:

    int err = snd_pcm_open(&(this->pcm), this->device.name.c_str(),
                           SND_PCM_STREAM_PLAYBACK, 0);

    // Allocate the hardware parameters:

    snd_pcm_hw_params_t* params = nullptr;

    snd_pcm_hw_params_alloca(&params);

    // Now configure some options:
    // TODO: Figure out all of this:

    // Fill the parameters with default values:

    int tah = snd_pcm_hw_params_any(pcm, params);

    // TODO: Figure out if wrong access will mess things up...

    // Enable resampling:

    int h = snd_pcm_hw_params_set_rate_resample(pcm, params, 1);

    // Set some non-negotiable values:

    int a = snd_pcm_hw_params_set_access(pcm, params,
                                         SND_PCM_ACCESS_RW_INTERLEAVED);
    int b = snd_pcm_hw_params_set_format(
        pcm, params, SND_PCM_FORMAT_S16);  // THIS FAILS, ONLY WITH FLOAT_64
    int c = snd_pcm_hw_params_set_channels(pcm, params, this->device.channels);
    int d =
        snd_pcm_hw_params_set_rate(pcm, params, this->device.sample_rate, 0);
    int eee = snd_pcm_hw_params_set_period_size(pcm, params,
                                                this->device.period_size, 0);
    snd_pcm_hw_params_set_periods(pcm, params, this->device.period, 0);

    // Commit the parameters:

    int commit = snd_pcm_hw_params(pcm, params);

    snd_pcm_hw_params_get_periods(params, &(this->device.period), 0);

    int junk = 0;

    int rret = snd_pcm_hw_params_get_period_size(
        params, &(this->device.period_size), &junk);

    long unsigned int buffer_sizeb;

    snd_pcm_hw_params_get_buffer_size(params, &(this->device.buffer_size));

    snd_pcm_hw_params_get_period_time(params, &(this->device.period_time),
                                      &junk);

    unsigned int rate = 0;

    snd_pcm_hw_params_get_rate(params, &rate, &junk);

    // DIVIDE BY 8 TO GET ACTUAL WIDTH!

    int sbits = snd_pcm_hw_params_get_sbits(params);
}

void ALSABase::alsa_stop() {

    // Drain the device:

    snd_pcm_drain(this->pcm);

    // Close the PCM device:

    snd_pcm_close(this->pcm);

    // Free the hardware:

    snd_pcm_hw_free(this->pcm);
}

void ALSASink::process() {

    // First, define our temporary vector:

    std::vector<int16_t> temp(buff.size() * buff.channels());

    // Next, squish it:

    squish_inter(&this->buff, temp.begin(), &mf_int16);

    // Determine if we need to prepare the device again:

    if (this->return_code == -EPIPE) {

        // Underrun occurred, prepare the device again:
        std::cout << "ALSA Underrun (Before)" << std::endl;

        snd_pcm_prepare(this->pcm);
    }

    // Finally, send the data along:

    this->return_code =
        snd_pcm_writei(this->pcm, reinterpret_cast<int16_t*>(temp.data()),
                       static_cast<snd_pcm_uframes_t>(temp.size()));

    if (this->return_code == -EPIPE) {
        // Underrun occurred

        std::cout << "ALSA Underrun (After)" << std::endl;
        snd_pcm_prepare(this->pcm);
    }
}

void ALSASink::start() {

    // First, determine if the device supports output:

    if (!this->get_device().output) {

        // Device does not support output, do something!
        // TODO: Figure this out!
    }

    // Next, upcall:

    ALSABase::alsa_start();
}

void ALSASink::info_sync() {

    // Set the period number:

    this->set_period(this->get_device().period);

    // Set the desired buffer size:

    auto bsize = this->get_device().period_size / 2;

    this->get_info()->out_buffer = bsize;
    this->get_info()->in_buffer = bsize;
}

#endif
