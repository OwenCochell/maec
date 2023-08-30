/**
 * @file filter_module.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various modules for filtering data
 * @version 0.1
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains several modules that can
 * be used to filter audio data in the chain.
 * We offer a few methods for filtering,
 * ranging from FIR filtering via convolution,
 * to recursive filtering.
 * Each implementation has it's own benefits
 * and nuances, so be sure to read up on the docs for each module!
 */

#pragma once

#include "audio_module.hpp"

#include "dsp/const.hpp"

/**
 * @brief A base class for filters
 * 
 * This class defines some attributes that can be
 * configured by the user.
 * Some common filtering values are provided,
 * such as the filter type and frequency.
 * 
 * If the filter type in not band related,
 * then filters will ignore the stop_freq parameter.
 * For example, if you have a low pass filter,
 * then the start_freq parameter will be used for the frequency value.
 * All frequency values should be in hertz!
 * 
 * By default, we set the filter type to be of LowPass,
 * and the start/stop frequency values to be 0.
 * 
 */
class BaseFilter: public AudioModule {

    private:

        /// Current filter type
        FilterType filter_type = FilterType::LowPass;

        /// Start frequency of filter
        double start_freq = 0;

        /// Stop frequency of the filter
        double stop_freq = 0;

    public:

    BaseFilter() =default;

    /**
     * @brief Creates a new BaseFilter
     * 
     * This is a convience method that allows developers
     * to set the basic filter parameters when constructing the object.
     * 
     * @param type Filter type
     * @param startf Start frequency of filter
     * @param stopf Stop frequency of filter
     */
     BaseFilter(FilterType type, double startf, double stopf)
         : filter_type(type), start_freq(startf), stop_freq(stopf) {};

     /**
      * @brief Gets the type of this filter.
      *
      * The filter type describes the type of the filter,
      * be it low pass, high pass, band pass, band reject.
      * You can use this method to retrieve this type.
      *
      * @return FilterType Type of this filter
      */
     FilterType get_type() const;

     /**
      * @brief Sets the type of this filter.
      *
      * As stated above, the filter type describes the type of the filter.
      * You can use this method to retrieve this type.
      *
      * @param type Type to set
      */
     void set_type(FilterType type);

     /**
      * @brief Gets the start frequency
      *
      * @return long double Start frequency
      */
     double get_start_freq() const;

     /**
      * @brief Sets the start frequency
      *
      * @param freq The frequency value to set
      */
     void set_start_freq(double freq);

     /**
      * @brief Gets the stop frequency
      *
      * @return long double Stop frequency
      */
     double get_stop_freq() const;

     /**
      * @brief Sets the stop frequency
      *
      * @param freq The frequency value to set
      */
     void set_stop_freq(double freq);
};

/**
 * @brief A base class for filters FIR filters
 * 
 * This class defines some operations that may be useful
 * for filters that utilize convolution.
 * For one, we define methods for generating filter kernels,
 * and we set the default process method to be a convolution.
 * 
 */
class BaseConvFilter : public BaseFilter {

    private:

        /// Filter kernel to utilize
        BufferPointer kernel = nullptr;

        /// Size of the filter kernel
        int size = 0;

    public:

        /**
         * @brief Gets the size of the filter kernel
         * 
         * @return int Size of filter kernel
         */
        int get_size() const;

        /**
         * @brief Sets the size of the filter kernel
         * 
         * @param nsize New size of filter kernel.
         */
        void set_size(int nsize);

        /**
         * @brief Gets a pointer to the kernel
         * 
         * @return AudioBuffer* AudioBuffer to contain a kernel
         */
        BufferPointer get_kernel();

        /**
         * @brief Sets the kernel using the BufferPointer
         * 
         * @param nkern New kernel to set
         */
        void set_kernel(BufferPointer nkern);

        /**
         * @brief Creates a filter kernel
         * 
         * Each filter modules should use this method
         * to create the necessary filter kernel.
         * This method will be called automatically where necessary.
         * 
         */
        virtual void generate_kernel() {}

        /**
         * @brief Starts this module
         * 
         * We call the 'generate_kernel()' method at startup,
         * allowing for proper filtering to take place
         * once audio data is provided.
         * 
         */
        void start() override;

        /**
         * @brief Processes incoming audio data
         * 
         * We Just convolve the incoming audio data
         * with the generated filter kernel. 
         * 
         */
        void process() override;
};

class SincFilter : public BaseConvFilter {

    public:

        SincFilter() =default;

        /**
         * @brief Generates a sinc filter kernel
         * 
         * We generate a sinc filter kernel based upon
         * the filter parameters in this class.
         * 
         */
        void generate_kernel() override;
};
