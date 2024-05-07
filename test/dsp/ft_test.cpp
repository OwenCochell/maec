/**
 * @file ft_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for FT operations
 * @version 0.1
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dsp/ft.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <complex>
#include <ctime>
#include <random>
#include <vector>

#include "dsp/util.hpp"

/// Known data to compute
std::vector<long double> ft_data = {-81.0611399999996160709, -32.4312400000002611124, -38.8382199999997827569, -111.7422700000002612, 52.9393500000000637312, 61.0861000000000130931, -16.2175099999997593923, -10.9511700000003418193, -61.7699399999997511741, -69.2286100000000882773, 0.421569999999996698238, 37.6517800000001357057, 80.4401700000000488971, -2.04557000000011644271, -85.9569899999997715442, 2.72508999999991183543, -66.3554300000000642709, -2.34488999999995559407, -40.3914900000000819981, -3.64987999999990044789, -17.7549800000004358823, 71.0510400000002124293, -48.0264400000000275734, 110.045180000000132392, -105.146259999999952539, -56.3275699999998687544, 95.8438100000005266874, 64.2721399999995513172, -43.4969999999996350254, -16.9237900000002831694, -109.863809999999695023, 84.7972800000000368273, 77.4762800000004462145, 57.2437800000003134647, -110.995029999999789382, -55.1876299999999012057, -95.6075800000002865298, 75.4721999999997809339, -12.7008200000005678967, -36.9976999999999474796, -49.1758900000000834345, -71.2256300000002166503, 108.823060000000235506, -110.420349999999915749, 90.9002899999999186831, -105.065209999999517897, 45.9153499999996984778, -1.05934999999953613926, -92.979900000000370891, -44.7161299999997329906};

/// Real and non-real parts of above data
std::vector<long double> reals = {-689.550949999998468776, -360.761730393757710567, -9.09627805513791995104, -327.622828943792766043, -237.608057949997369562, -56.4823335254953177095, -420.058431365067667523, -405.18109821272589835, 410.473673843006695933, 384.975560036773781697, -493.178524073103451975, 396.298291421185540917, 58.616429016356431804, 317.639586912586099038, -125.923587794969226675, -26.3771664745074108874, -186.769206518632613412, 192.096986502756412657, -396.525639886057485434, 145.43231520399339618, -187.554675926895537533, -345.553158653373423709, -606.114272127072479468, -44.830973873640273572, 821.155170837586112498, -357.606149999998992051};
std::vector<long double> nonreals = {0, -56.7987997464239218748, -88.4598663671056244934, 114.599317038853708184, 59.8147392880928162121, -318.732530313386465293, 25.4838243897190054234, 298.856932852788386101, -52.7828546196376580737, 639.085076012777274668, -725.060920075901687409, -217.793354878620727413, -50.8793220693166556787, -81.2345735657510905209, -312.27135476011737128, 13.6098176315475818607, -225.700304988403331885, 267.968070726491263162, -138.317628623756462919, 724.441385521063360764, 358.256926195155114334, 367.869822138617369595, -379.416274255358194706, -378.675193748761023865, -204.906665100901602239, 4.487825376552348945e-13};

/// Known complex data to compute
const std::vector<std::complex<long double>> cft_data = {std::complex<long double>(-15.189251, 0.000000), std::complex<long double>(11.704489, -7.808442), std::complex<long double>(-0.090070, 10.845936), std::complex<long double>(-11.697811, -5.706792), std::complex<long double>(-2.788215, 7.507124), std::complex<long double>(8.873675, -10.790867), std::complex<long double>(-5.838451, 2.531785), std::complex<long double>(4.630811, -6.716717), std::complex<long double>(0.936014, -15.374274), std::complex<long double>(15.496844, -3.780477), std::complex<long double>(6.904307, -2.551331), std::complex<long double>(-4.702844, 12.109802), std::complex<long double>(-4.071020, 15.037189), std::complex<long double>(7.972179, -6.781337), std::complex<long double>(0.624014, -5.571347), std::complex<long double>(4.138358, -0.394291), std::complex<long double>(-1.130739, 0.000000), std::complex<long double>(4.138358, 0.394291), std::complex<long double>(0.624014, 5.571347), std::complex<long double>(7.972179, 6.781337), std::complex<long double>(-4.071020, -15.037189), std::complex<long double>(-4.702844, -12.109802), std::complex<long double>(6.904307, 2.551331), std::complex<long double>(15.496844, 3.780477), std::complex<long double>(0.936014, 15.374274), std::complex<long double>(4.630811, 6.716717), std::complex<long double>(-5.838451, -2.531785), std::complex<long double>(8.873675, 10.790867), std::complex<long double>(-2.788215, -7.507124), std::complex<long double>(-11.697811, 5.706792), std::complex<long double>(-0.090070, -10.845936), std::complex<long double>(11.704489, 7.808442)};

// Known complex result:
const std::vector<std::complex<long double>> cft_output = {std::complex<long double>(47.8646, 0), std::complex<long double>(-60.679, 0), std::complex<long double>(-65.8105, 0), std::complex<long double>(69.5778, 0), std::complex<long double>(99.7526, 0), std::complex<long double>(-90.287, 0), std::complex<long double>(36.9178, 0), std::complex<long double>(80.6327, 0), std::complex<long double>(-88.2723, 0), std::complex<long double>(-62.5006, 0), std::complex<long double>(-76.6374, 0), std::complex<long double>(-52.985, 0), std::complex<long double>(-46.0648, 0), std::complex<long double>(-86.605, 0), std::complex<long double>(-68.5806, 0), std::complex<long double>(-56.903, 0), std::complex<long double>(-97.7982, 0), std::complex<long double>(24.3152, 0), std::complex<long double>(58.614, 0), std::complex<long double>(89.4592, 0), std::complex<long double>(-55.8749, 0), std::complex<long double>(-93.1961, 0), std::complex<long double>(19.1352, 0), std::complex<long double>(-77.2428, 0), std::complex<long double>(25.5402, 0), std::complex<long double>(54.4226, 4.33681e-19), std::complex<long double>(-49.1747, 0), std::complex<long double>(80.9814, 0), std::complex<long double>(-0.730811, 0), std::complex<long double>(-43.9266, 0), std::complex<long double>(-1.95563e-07, -2.1684e-19), std::complex<long double>(6.6234926488113743e-07, 0)};

// Known real data (power of 2)
std::vector<long double> ft2_data = {-9.6733407586,62.9653550514,74.4000694857,99.1967369526,93.4037032815,57.3335037397,101.7345720310,18.1861254434,14.4572532371,-16.1701715992,-90.1191210720,65.5052860315,-50.6822660903,85.6097778570,64.9082173196,20.7108693629,80.0814240580,105.6846021807,65.2129539715,-85.4664831284,-11.1276864500,-99.0217522157,-104.1126140924,-12.2773308137,100.2898453047,-82.4167709429,103.5442857695,-81.0976502582,-109.1820435246,30.8008533158,29.1963157524,-11.3502179318,-6.2240021300,-38.8990464614,-52.9055938733,48.6190705661,50.3520832266,-62.9548072614,-69.0177881626,104.6779683739,39.5905650836,-101.7229133271,43.2043503664,47.9158760699,-56.6139623555,-21.0172069627,-31.0987360694,-16.0686666005,-31.4280055848,-76.0297646764,-9.3888213890,82.1388450892,-25.3763959200,-14.4709267300,-51.6942710109,-19.6178678761,-3.1049798902,-24.6729543717,11.6011396251,-55.8205348801,-46.1782259805,-75.5780087238,-95.4761768191,3.0709157917};

// Known complex result:
std::vector<std::complex<long double>> ft2_output = {std::complex<long double>(-43.664541595800022, 0), std::complex<long double>(359.45181848469754, -565.11450941061558), std::complex<long double>(8.6293957918963725, -753.12168792311656), std::complex<long double>(-46.863123137862357, -131.23397931250944), std::complex<long double>(239.56283276052127, -481.96012333692197), std::complex<long double>(224.101183608534, -1016.8846705832734), std::complex<long double>(-509.15587956189165, -464.98325585412137), std::complex<long double>(-389.00758691682769, 224.57211055070545), std::complex<long double>(264.70720400647352, -274.02490505960353), std::complex<long double>(-93.651889962216083, -417.15007283068046), std::complex<long double>(275.92137264991444, 368.38896499036456), std::complex<long double>(466.14362933356768, -209.80187776532449), std::complex<long double>(-402.08144966580375, 93.785616391165261), std::complex<long double>(62.754539854000547, -189.2984833849671), std::complex<long double>(-288.70843453473079, 107.18804062488756), std::complex<long double>(464.62263270044914, -213.62987034748249), std::complex<long double>(38.595183674499992, 478.88317332010003), std::complex<long double>(-518.36322233252044, -153.35268279949373), std::complex<long double>(326.60869521036493, -30.208229145220096), std::complex<long double>(100.10624013796194, 414.4482393462402), std::complex<long double>(24.120287213970933, -761.33411973294775), std::complex<long double>(-240.17910982830549, 805.60902291646104), std::complex<long double>(158.69833481771356, 60.469890206290065), std::complex<long double>(-21.543578704467073, -320.99769573919335), std::complex<long double>(414.07990225872646, 328.19458281099647), std::complex<long double>(-252.32542782790214, -302.63466451478207), std::complex<long double>(-557.5731385339072, 45.443397832052496), std::complex<long double>(-293.40889713883586, 118.58242620276572), std::complex<long double>(-335.50810291108844, 142.55294889496504), std::complex<long double>(426.81576634412209, 143.20747673715266), std::complex<long double>(69.173563266240378, -417.26860597337583), std::complex<long double>(-303.8423926719958, 206.59293747939968), std::complex<long double>(80.810036274799974, 0)};

/**
 * @brief Compares two complex numbers.
 *
 * We compare the real and imaginary
 * parts of the two given complex numbers.
 *
 * TODO: Use more precise output values?
 * I think the absolute error is too low,
 * so more precise values would fix this.
 * 
 * @param first First complex number
 * @param second Second complex number
 */
void compare_complex(std::complex<long double> first, std::complex<long double> second) {

    // Compare real part:

    REQUIRE_THAT(first.real(), Catch::Matchers::WithinAbs(second.real(), 0.0001));

    // Compare imaginary parts:

    REQUIRE_THAT(first.imag(), Catch::Matchers::WithinAbs(second.imag(), 0.0001));
}

/**
 * @brief Generates N complex random numbers
 * 
 * The real and imaginary parts of the complex numbers
 * will be -1 < p < 1.
 * 
 * @param size Number of complex numbers to generate
 * @param out Iterator to output array
 */
void rand_complex(int size, std::vector<std::complex<long double>>::iterator out) {

    // Create a random number generator:

    std::default_random_engine rand;

    // Seed random number generator:

    rand.seed(time(nullptr));

    // Iterate over size of data to create:

    for (int i = 0; i < size; ++i) {

        // Create real value:

        auto real = static_cast<long double>((static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max());

        // Create imaginary value:

        auto nonreal = static_cast<long double>((static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max());
    
        // Set complex value to current iterator:

        *(out + i) = std::complex<long double>(real, nonreal);
    }
}

/**
 * @brief Generates N random numbers
 * 
 * Each number will be -1 < p < 1
 * 
 * @param size Number of complex numbers to generate
 * @param out Iterator to output array
 */
void rand_real(int size, std::vector<long double>::iterator out) {

    // Create random number generator:

    std::default_random_engine rand;

    // Seed random number generator:

    rand.seed(time(nullptr));

    // Iterate over size of data to create:

    for (int i = 0; i < size; ++i) {

        // Create value:

        *(out + i) = (static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max();
    }
}

TEST_CASE("DFT", "[ft][dsp]") {

    SECTION("Size", "Ensure size predictions are accurate") {

        REQUIRE(length_ft(10) == (10 / 2) + 1);

        REQUIRE(length_ift(10) == (10 - 1) * 2);
    }

    SECTION("Random", "Ensures random data can be transformed and reversed") {

        // Configure the signal size, should be even!

        int size = 20;

        // Create a collection of input data:

        std::vector<long double> nums;

        // Preallocate (just for fun)

        nums.reserve(size);

        // Create a random number generator:

        std::default_random_engine rand;

        // Seed random number generator:

        rand.seed(time(nullptr));

        // Create containers for real and non-real parts

        std::vector<long double> nonreal;
        std::vector<long double> real;

        // Reserve parts:

        nonreal.reserve(length_ft(size));
        real.reserve(length_ft(size));

        // Create container for output signal:

        std::vector<long double> output;

        output.reserve(size);

        // Iterate a number of times

        for (int i = 0; i < size; ++i) {

            // Add a random value to the collection:

            nums.push_back(static_cast<long double>(
                (static_cast<long double>(rand()) -
                 (std::default_random_engine::max() / 4.0)) /
                std::default_random_engine::max()));

            // Add a zero to the output signal:

            output.push_back(0);
        }

        for (int i = 0; i < length_ft(size); ++i) {

            // Set each value to zero:

            nonreal.push_back(0);
            real.push_back(0);
        }

        // Now, send the data through the FT algorithm:

        dft(nums.begin(), size, real.begin(), nonreal.begin());

        // Send parts through inverse algorithm:

        inv_dft(real.begin(), nonreal.begin(), length_ft(size), output.begin());

        // Ensure data matches:

        for (int i = 0; i < size; ++i) {

            // Check this value:

            REQUIRE_THAT(nums.at(i), Catch::Matchers::WithinAbs(output.at(i), 0.0000001));
        }
    }

    SECTION("Known", "Ensures DFT on known data is accurate") {

        // Create vectors containing responses:

        std::vector<long double> real;
        std::vector<long double> nonreal;

        // Get length of output data:

        int output_length = length_ft(ft_data.size());

        // Reserve the vectors:

        real.reserve(output_length);
        nonreal.reserve(output_length);

        // Fill the vectors with zeros:

        for (int i = 0; i < output_length; ++i) {

            real.push_back(0);
            nonreal.push_back(0);
        }

        // Now, send the data through the DFT function:

        dft(ft_data.begin(), static_cast<int>(ft_data.size()), real.begin(),
            nonreal.begin());

        // Finally, ensure returned data is correct:

        for (int i = 0; i < output_length; ++i) {

            // Check real part:

            REQUIRE_THAT(reals.at(i), Catch::Matchers::WithinAbs(real.at(i), 0.0000001));

            // Check non-real part:

            REQUIRE_THAT(nonreals.at(i), Catch::Matchers::WithinAbs(nonreal.at(i), 0.0000001));
        }
    }

    SECTION("Inverse Known", "Ensures iDFT on known data is accurate") {

        // Create vector for output data:

        std::vector<long double> output;

        // Determine size of output data:

        int output_size = length_ift(nonreals.size());

        // Reserve the output data:

        output.reserve(output_size);

        // Fill output buffer with 0:

        for (int i = 0; i < output_size; ++i) {

            output.push_back(0);
        }

        // Send data through function:

        inv_dft(reals.begin(), nonreals.begin(), static_cast<int>(reals.size()),
                output.begin());

        // Ensure outgoing data is correct:

        for (int i = 0; i < output_size; ++i) {

            // Ensure returned data is accurate:

            REQUIRE_THAT(ft_data.at(i), Catch::Matchers::WithinAbs(output.at(i), 0.0000001));
        }
    }
}

TEST_CASE("FFT2", "[ft][dsp]") {

    SECTION("Out of place", "Tests the out of place algorithm") {

        SECTION("Forward", "Ensures the FFT2 algorithm works on known data") {

            // Get length of output data:

            int output_length = static_cast<int>(cft_data.size());

            // Create vector of output data:

            std::vector<std::complex<long double>> out(output_length);

            // Fill output buffer with zeroes:

            std::fill_n(out.begin(), output_length, 0);

            // Send data through FFT function:

            fft_c_radix2(cft_data.begin(), output_length, out.begin());

            // Iterate over output data:

            for (int i = 0; i < output_length; ++i) {

                // Compare with known data:

                compare_complex(out.at(i), cft_output.at(i));
            }
        }

        SECTION("Inverse", "Ensures the iFFT2 algorithm works with known data") {

            // Get length of output data:

            int output_length = static_cast<int>(cft_output.size());

            // Create vector of output data:

            std::vector<std::complex<long double>> out(output_length);

            // Reserve output buffer:

            out.reserve(output_length);

            // Fill output buffer with zeros:

            std::fill_n(out.begin(), output_length, 0);

            // Send data through IFFT function:

            ifft_c_radix2(cft_output.begin(), output_length, out.begin());

            // Iterate over output data:

            for (int i = 0; i < output_length; ++i) {

                // Compare with known data:

                compare_complex(out.at(i), cft_data.at(i));
            }
        }

        SECTION("Random", "Ensures the FFT2 algorithm works on random data") {

            // Define size of random data:

            int size = 32;

            // Create vector to hold random data:

            std::vector<std::complex<long double>> rdata(size);
            std::vector<std::complex<long double>> output(size);
            std::vector<std::complex<long double>> final_data(size);

            // Create random complex data:

            rand_complex(size, rdata.begin());

            // Run through Radix2:

            fft_c_radix2(rdata.begin(), size, output.begin());

            // Run through inverse:

            ifft_c_radix2(output.begin(), size, final_data.begin());

            // Check final data matches input:

            for (int i = 0; i < size; ++i) {

                // Compare the complex number:

                compare_complex(rdata.at(i), final_data.at(i));
            }
        }
    }

    SECTION("In Place", "Tests for the in place algorithm") {

        SECTION("Forward", "Ensures the FFT2 algorithm works on known data") {

            // Get length of output data:

            int output_length = static_cast<int>(cft_data.size());

            // Create vector of output data:

            std::vector<std::complex<long double>> out(output_length);

            // Fill output buffer with data from input:

            std::copy(cft_data.begin(), cft_data.end(), out.begin());

            // Send data through FFT function:

            fft_c_radix2(out.begin(), output_length);

            // Iterate over output data:

            bit_reverse(output_length, out.begin());

            for (int i = 0; i < output_length; ++i) {

                // Compare with known data:

                compare_complex(out.at(i), cft_output.at(i));
            }
        }

        SECTION("Inverse", "Ensures the iFFT2 algorithm works on known data") {

            // Get length of output data:

            int output_length = static_cast<int>(cft_output.size());

            // Create vector of output data:

            std::vector<std::complex<long double>> out(output_length);

            // Fill output buffer with data from input:

            std::copy(cft_output.begin(), cft_output.end(), out.begin());

            // Send data through FFT function:

            ifft_c_radix2(out.begin(), output_length);

            // Iterate over output data:

            bit_reverse(output_length, out.begin());

            // Iterate over output data:

            for (int i = 0; i < output_length; ++i) {

                // Compare with known data:

                compare_complex(out.at(i), cft_data.at(i));
            }
        }

        SECTION("Random", "Ensures the FFT2 algorithm works with random data") {

            // Define size of random data:

            int size = 32;

            // Create vector to hold random data:

            std::vector<std::complex<long double>> rdata(size);
            std::vector<std::complex<long double>> output(size);

            // Create random complex data:

            rand_complex(size, rdata.begin());

            // Copy to output:

            std::copy_n(rdata.begin(), size, output.begin());

            // Run through Radix 2:

            fft_c_radix2(output.begin(), size);

            // Do bit reversal:

            bit_reverse(size, output.begin());

            // Run through inverse Radix 2:

            ifft_c_radix2(output.begin(), size);

            // Do bit reversal:

            bit_reverse(size, output.begin());

            // Check final data matches input:

            for (int i = 0; i < size; ++i) {

                // Compare the complex number:

                compare_complex(rdata.at(i), output.at(i));
            }
        }
    }

    SECTION("Real", "Tests for the real FFT2 algorithm") {

        SECTION("Forward", "Ensures the forward algorithm works") {

            // Determine sizes:

            auto size = ft2_data.size();
            int osize = length_ft(size);

            // Create vector to hold real to complex data:

            std::vector<std::complex<long double>> out(osize);

            // Send data through real FFT function:

            fft_r_radix2(ft2_data.begin(), size, out.begin());

            // Ensure output matches input:

            for (int i = 0; i < osize; ++i) {

                // Ensure values match:

                compare_complex(ft2_output.at(i), out.at(i));
            }
        }

        SECTION("Inverse", "Ensures the inverse algorithm works") {

            // Determine sizes:

            auto size = ft2_output.size();
            int osize = length_ift(size);

            // Create vector to hold complex to real data:

            std::vector<long double> out(osize);

            // Send data through real iFFT function:

            ifft_r_radix2(ft2_output.begin(), size, out.begin());

            // Ensure output matches input:

            for (int i = 0; i < osize; ++i) {

                // Ensure values match:

                REQUIRE_THAT(ft2_data.at(i), Catch::Matchers::WithinAbs(out.at(i), 0.00001));
            }
        }
    }
}
