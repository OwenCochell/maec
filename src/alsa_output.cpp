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

#ifdef ALSA_F

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

    // Send our audio to the buffer

    // This is really dirty, cleanup later!
    // TODO: Maybe add this to an AudioBuffer class?

    std::vector<long double>* thing = this->get_buffer().get();

    auto* blah = std::data(*thing);

    snd_pcm_writei(this->pcm, blah, this->get_info()->buff_size);
}

#endif