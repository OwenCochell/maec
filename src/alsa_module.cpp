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

#include "alsa_module.hpp"

#include <iostream>

#include "chrono.hpp"

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

    int err = snd_pcm_open(&pcm, this->name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    if (err != 0) {

        // We have encountered an error, DO NOT CONTINUE!

        this->load_fail = true;

        return;
    }

    // Allocate the hardware parameters:

    snd_pcm_hw_params_t* params = nullptr;

    snd_pcm_hw_params_alloca(&params);

    // Fill the hardware parameters for our device:

    snd_pcm_hw_params_any(pcm, params);

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

DeviceInfo ALSABase::get_device_by_name(std::string name) {

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

        std::string hname (snd_device_name_get_hint(*n, "NAME"));

        // Determine if the name is our target:

        if (hname == name) {

            // Found it, break
            // TODO: Implement some kind of error correction
            // For now we assume all provided names are valid which is DANGEROUS!

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

void ALSABase::alsa_start(int sample_rate, int buffer_size) {

    // TODO: Implement error checking and correction!

    // Create the PCM device:

    int err = snd_pcm_open(&(this->pcm), this->device.name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    // Set the options:
    // TODO: Figure out how we sample the AudioInfo struct!

    snd_pcm_hw_params_alloca(&(this->params));

    // Now configure some options:
    // TODO: Figure out all of this:

    // Fill the parameters with default values:

    int tah = snd_pcm_hw_params_any(pcm, this->params);

    // TODO: Figure out if wrong access will mess things up...

    // Enable resampling:

    int h = snd_pcm_hw_params_set_rate_resample(pcm, this->params, 1);

    // Set some non-negotiable values:

    int a = snd_pcm_hw_params_set_access(pcm, this->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	int b = snd_pcm_hw_params_set_format(pcm, this->params, SND_PCM_FORMAT_FLOAT); // THIS FAILS, ONLY WITH FLOAT_64
	int c = snd_pcm_hw_params_set_channels(pcm, this->params,  this->device.channels);
	int d = snd_pcm_hw_params_set_rate(pcm, this->params, sample_rate, 0);

    // Determine if we should attempt to set the number of periods:

    if (this->device.period > 0) {

        // We have a request, get it close:

        int pcode = snd_pcm_hw_params_set_periods_near(pcm, params, &(this->device.period), 0);
    }

    // Determine if we should attempt to set the period size:

    if (this->device.period_size > 0) {

        // We have a request, get it close:

        int pscode = snd_pcm_hw_params_set_period_size_near(pcm, params, &(this->device.period_size), 0);
    }

    // Commit the parameters:

    int commit = snd_pcm_hw_params(pcm, params);

    snd_pcm_hw_params_get_periods(params, &(this->device.period), 0);

    int junk = 0;

    int rret = snd_pcm_hw_params_get_period_size(params, &(this->device.period_size), &junk);

    long unsigned int buffer_sizeb;

    snd_pcm_hw_params_get_buffer_size(params, &(this->device.buffer_size));

    snd_pcm_hw_params_get_period_time(params, &(this->device.period_time), &junk);

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

    int64_t start = get_time();

    // First, define our temporary vector:

    std::vector<float> temp(buff->size() * buff->get_channel_count());

    auto tthing = buff->size();

    // Next, squish it:

    squish_inter(this->buff.get(), temp.begin(), &mf_float);

    auto thing = temp.size();

    int64_t stop = get_time();

    std::cout << "Squish Latency:" << std::endl;
    std::cout << stop - start << std::endl;

    // int state = snd_pcm_state(this->pcm);

    // Determine if we need to prepare the device again:

    if (this->return_code == -EPIPE) {

        // Underrun occurred, prepare the device again:
        std::cout << "ALSA Underrun (Before)" << std::endl;

        snd_pcm_prepare(this->pcm); 
    }

    // Finally, send the data along:

    this->return_code =
        snd_pcm_writei(this->pcm, reinterpret_cast<float*>(temp.data()),
                       static_cast<snd_pcm_uframes_t>(temp.size()));

    std::cout << this->return_code << std::endl;

    if (this->return_code == -EPIPE) {
        // Underrun occurred

        std::cout << "ALSA Underrun (After)" << std::endl;
        snd_pcm_prepare(this->pcm);
    }
}

void ALSASink::start() {

    // First, determine if the device supports output:

    if(!this->get_device().output) {

        // Device does not support output, do something!
        // TODO: Figure this out!
    }

    // Next, upcall:

    ALSABase::alsa_start(this->get_info()->sample_rate, this->get_info()->out_buffer);

    // Next, set the new period number:

    this->set_period(this->get_device().period);

    // Set the new buffer size:

    this->get_info()->out_buffer = this->get_device().period_size / 4;
}

#endif
