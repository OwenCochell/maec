/**
 * @file parameter_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for ModuleParameters
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <vector>
#include <memory>

#include "module_param.hpp"
#include "base_oscillator.hpp"
#include "fund_oscillator.hpp"
#include "meta_audio.hpp"

TEST_CASE("Module Parameter Tests", "[param]") {

    // Construct the parameters:

    const ModuleParameter param;
    ModuleParameter const_param(5.0);

    SECTION("Constant Parameter Functionality", "Ensure we can use constant parameters") {

        // Ensure we can get a value:

        REQUIRE_THAT(const_param.get(), Catch::Matchers::WithinAbs(5., 0.0001));

        // Now, change the value:

        const_param.set_constant(3.0);

        // Test the values multiple times:

        for(int i = 0; i < 20; ++i) {

            REQUIRE_THAT(const_param.get(), Catch::Matchers::WithinAbs(3., 0.0001));
        }
    }

    SECTION("Module Parameter Functionality", "Ensure we can bind modules to parameters") {

        // Dummy data to utilize

        std::vector<long double> data = {1,2,3,4,5,6,7,8,9,10};

        // Create a module to use:

        ConstantOscillator osc(3.0);

        // Create a parameter:

        ModuleParameter mod_param(&osc);

        // Ensure the value is accurate:

        REQUIRE_THAT(mod_param.get(), Catch::Matchers::WithinAbs(3.0, 0.0001));

        // Do something a bit crazier:

        const SineOscillator sine(440.0);

        // Create a dummy module:

        BufferModule src;

        // Bind dummy data buffer:

        AudioBuffer buff(data);

        src.set_rbuffer(&buff);

        // Set the source module

        mod_param.set_module(&src);

        // Now, ensure values are accurate:

        for(int i = 0; i < 10 * 4; ++i) {

            REQUIRE_THAT(mod_param.get(), Catch::Matchers::WithinAbs(data.at(i % 10), 0.0001));
        }
    }
}
