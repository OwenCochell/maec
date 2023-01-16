/**
 * @file alsa_module.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for working with ALSA
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * Here we define various classes and modules that work 
 * with ALSA
 */

#pragma once

#ifdef ALSA_F

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <string>
#include "sink_module.hpp"

/**
 * @brief A struct containing info on an ALSA device
 * 
 * This struct contains various info on specific ALSA devices.
 * 
 * We contain:
 * 
 * - Device index
 * - Device name
 * - Device description
 * - Device IO type, be it Input or Output
 * - Maximum number of channels this device supports
 * - Minimum channels this device supports
 * - Current number of channels on this device
 * - Maximum period size
 * - Minimum period size
 * - Current period size
 * - Maximum number of periods
 * - Minimum number of periods
 * - Number of periods
 *  
 * We are used by the ALSA modules to represent these devices.
 * You can use these structs to work with and understand these devices,
 * as well as tell the ALSA module which device to utilize.
 * 
 * When returned by the ALSA class,
 * this struct will contain all default values for this specific device.
 * If you wish to change settings, simply change the values specified here
 * and pass it along to the ALSA module.
 * It will use the values specified here.
 * 
 * If any of these values are -1, then the ALSA module
 * will use the default recommended value from ALSA.
 * 
 */
struct DeviceInfo {

    /// The id of this device
    int id;

    /// The name of this device
    std::string name;

    /// The description of this device
    std::string description;

    /// The IO type determining if we are an input device
    bool input = false;

    /// The IO type determining if we are an output device
    bool output = false;

    /// Maximum number of periods this device supports
    unsigned int period_max = 0;

    /// Minimum number of periods this device supports
    unsigned int period_min = 0;

    /// Number of periods for this device
    unsigned int period = -1;

    /// Maximum period size for this device
    unsigned long period_size_max = 0;

    /// Minimum period size for this device
    unsigned long period_size_min = 0;

    /// Period size for this device
    unsigned long period_size = -1;

    /// Number of channels for this device
    unsigned int channels = 1;

    /// The maximum number of channels this device supports
    unsigned int channels_max = 0;

    /// The minimum number of channels this device supports
    unsigned int channels_min = 0;

    /// Max period time
    unsigned int period_time_max = 0;

    /// Min period time
    unsigned int period_time_min = 0;

    /// Period time
    unsigned int period_time = -1;

    DeviceInfo() =default;

    /**
     * @brief Construct a new Device Info object
     * 
     * We take the given values and use them
     * to initialize this struct
     * 
     * @param hint Hint to use for getting device info 
     * @param id ID of this device
     */
    DeviceInfo(void** hint, int id) { this->create_device(hint, id); }

    /**
     * @brief Initializes the values in this struct
     * 
     * We take the given hint,
     * which is a value returned by the ALSA library,
     * and extract necessary device info from it.
     * We also expect an integer that represents our id
     * 
     * WE WILL NOT FREE THIS HINT!
     * 
     * It is up to the caller to free the hint after we have extracted 
     * our information from it.
     * 
     * @param hint The current device to extract hints from
     */
    void create_device(void** hint, int id);

};

/**
 * @brief A base class that works with ALSA
 * 
 * We define a class that works with ALSA.
 * We define the framework for querying and selecting ALSA devices.
 * All ALSA modules must inherit this class!
 * 
 * By default, we setup the configured device (the default device if not specified)
 * at start time.
 * At stop time, we destroy the device and any other components that are not needed.
 * During processing, we do nothing.
 * 
 * Please note, this class does NOT implement MAEC modules,
 * meaning that this class does not support binding, processing,
 * start/stop, ect.
 * Therefore, this class is really only useful for developing 
 * modules that utilize ALSA.
 * You probably want to use the ALSASink and ALSASource modules
 * to properly utilize audio data. 
 */
class ALSABase {

    private:

        /// The current device in use by this module
        DeviceInfo device;

    protected:

        /// The ALSA PCM interface
        snd_pcm_t *pcm;

        /// The ALSA PCM hardware parameters
        snd_pcm_hw_params_t *params;

    public:

        /**
         * @brief Construct a new ALSAModule object
         * 
         * We set the default device to "default"
         */
        ALSABase() { this->set_device("default"); }

        /**
         * @brief Set the ALSA device.
         * 
         * This method will set the ALSA device to 
         * be used upon starting.
         * 
         * We require a DeviceInfo struct for determining the
         * device to use.
         * You should check out the other methods in this class
         * for getting this device,
         * such as get_device_info().
         * 
         * @param device Device to use for audio operations
         */
        void set_device(DeviceInfo device) { this->device = device; }

        /**
         * @brief Set the ALSA device using it's name
         * 
         * This method will set the ALSA device
         * to be used upon starting.
         * 
         * We search all devices until we find one with a matching name.
         * Otherwise, we do something
         * TODO: Implement some error checking
         * 
         * @param device Name of the ALSA device to use
         */
        void set_device(std::string device) { this->set_device(this->get_device_by_name(device)); }

        /**
         * @brief Set the ALSA device using it's id
         * 
         * This method will set the ALSA device
         * to be used upon starting.
         * 
         * We retrieve the device info using the given index.
         * If this index does not exist, then we do, something...
         * TODO: Implement some error checking, again
         * 
         * @param device Index of the ALSA device to use
         */
        void set_device(int device) { this->set_device(this->get_device_by_id(device)); }

        /**
         * @brief Gets the current ALSA device
         * 
         * This method will get the current device to
         * be loaded upon starting.
         * 
         * @return DeviceInfo The current device
         */
        DeviceInfo get_device() const { return this->device; }

        /**
         * @brief Gets the number of devices we have to work with
         * 
         * We return the number of devices available for use 
         * 
         * @return int Number of devices available for use
         */
        int get_device_count();

        /**
         * @brief Gets a device by it's id
         * 
         * We grab the device at the given id
         * and return it's info.
         * If this index does not exist, then we do something!!!
         * TODO: IMPLEMENT THIS!
         * 
         * @param id ID of the device
         * @return DeviceInfo Information about retrieved device
         */
        DeviceInfo get_device_by_id(int id);

        /**
         * @brief Gets a device by it's name
         * 
         * We search all devices until we find one 
         * that has the given name.
         * If we are unable to find a device with the given name,
         * then we DO STUFF.
         * TODO: Yeah, you know
         * 
         * @param name name of the device to get
         * @return DeviceInfo Information about retrieved device 
         */
        DeviceInfo get_device_by_name(std::string name);

        /**
         * @brief Starts this module.
         * 
         * We configure the necessary ALSA parameters,
         * and ensure everything is initialized.
         * 
         * @param sample_rate The sample of the device to open
         * @param buffer_size The buffer size of the device to open
         */
        void alsa_start(int sample_rate, int buffer_size);

        /**
         * @brief Stops the underlying ALSA components
         * 
         * We stop the ALSA components we are using,
         * and wait for the buffer to empty.
         * 
         */
        void alsa_stop();

};

/**
 * @brief Outputs audio data to an ALSA device
 * 
 * We consume audio data from the back modules
 * and send them out to the selected ALSA device.
 * We inherit most functionality from the ALSAModule class.
 *  
 */
class ALSASink : public ALSABase, public SinkModule {

    public:

        /**
         * @brief Construct a new ALSASink object
         * 
         * Default constructor
         * 
         */
        // ALSASink() : SinkModule(&squish_inter) {}

        /**
         * @brief Sends audio data to the selected ALSA device
         * 
         * After grabbing the audio data from the back modules,
         * we send that data to the output ALSA device.
         * 
         */
        void process() override;

        /**
         * @brief Starts this module
         * 
         * We set the period numbers to
         * the value set by the SinkModule we inherit.
         * 
         * We also upcall the original start method in ALSAModule
         * 
         */
        void start() override;

        void stop() override { this->alsa_stop(); }

        /// TODO: DELETE THIS!

        static float cast_float(long double val) { return float(val); }
};

#endif
