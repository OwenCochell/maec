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

int AlsaOutput::get_device_index(std::string name) {

    // Get the index via the name:

    return snd_card_get_index((name.c_str()));
}

std::string AlsaOutput::get_device_name(int index) {

    // Create a dummy char value:

    char* val;

    // Call the appropriate method:

    snd_card_get_name(index, &val);

    // Convert char to string:

    std::string name(val);

    // Free the val:

    free(val);

    // Finally, return the name:

    return name;
}

void AlsaOutput::start() {

    // TODO: Implement error checking and correction!

    // Create the PCM device:

    int err = snd_pcm_open(&this->pcm, this->device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    // Set the options:
    // TODO: Figure out how we sample the AudioInfo struct!

    snd_pcm_hw_params_alloca(&this->params);

    // Now configure some options:
    // TODO: Figure out all of this:

    snd_pcm_hw_params_any(pcm, this->params);

	snd_pcm_hw_params_set_access(pcm, this->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm, this->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm, this->params, 1);
	snd_pcm_hw_params_set_rate(pcm, this->params, this->get_info()->sample_rate, 0);
	snd_pcm_hw_params_set_periods(pcm, this->params, 10, 0);
    snd_pcm_hw_params_set_period_size(pcm, this->params, this->get_info()->buff_size, 0);
	snd_pcm_hw_params_set_period_time(pcm, this->params, 100000, 0); // 0.1 seconds period time

}

void AlsaOutput::stop() {

    // Close the PCM device:

    snd_pcm_close(this->pcm);
}

void AlsaOutput::process() {

    // Create a dummy buffer:

    std::vector<long double> temp(this->get_buffer()->size() * this->periods);

    // Generate a number of periods:

    for(int i = 0; i < this->periods; ++i) {

        // Copy this value to the buffer:

        std::copy(this->get_buffer()->ibegin(), this->get_buffer()->iend(), temp.begin() + (i * this->get_buffer()->size()));
    }

    // Finally, output the audio data:

    snd_pcm_writei(this->pcm, std::data(temp), this->get_info()->buff_size);

}

#endif
