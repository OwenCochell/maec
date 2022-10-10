/**
 * @file alsa_output.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for ALSA Output tools
 * @version 0.1
 * @date 2022-10-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "alsa_output.hpp"

void AlsaOutput::start() {

    // TODO: Implement error checking and correction!

    // Create the PCM device:

    int err = snd_pcm_open(&this->pcm, this->device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    // Set the options:
    // TODO: Figure out how we sample the AudioInfo struct!

    snd_pcm_hw_params_alloca(&this->params);

    // Now configure some options:
    // TODO: Figure out all of this:

	snd_pcm_hw_params_set_access(pcm, this->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm, this->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm, this->params, 1);
	snd_pcm_hw_params_set_rate(pcm, this->params, SAMPLE_RATE, 0);
	snd_pcm_hw_params_set_periods(pcm, this->params, 10, 0);
	snd_pcm_hw_params_set_period_time(pcm, this->params, 100000, 0); // 0.1 seconds period time

    snd_pcm_hw_params_any(pcm, this->params);

}