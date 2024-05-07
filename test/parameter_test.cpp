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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>
#include <vector>

#include "audio_buffer.hpp"
#include "meta_audio.hpp"
#include "module_param.hpp"

TEST_CASE("Module Parameter Tests", "[param]") {

    // Construct the parameters:

    ModuleParameter const_param(5.0);

    SECTION("Constant Parameter Functionality", "Ensure we can use constant parameters") {

        // Ensures we can get a constant value:

        auto buff = const_param.get();

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(5., 0.0001));
        }

        // Now, change the value:

        const_param.set_constant(3.0);

        // Test the values multiple times:

        for(int i = 0; i < 5; ++i) {

            buff = const_param.get();

            for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(3., 0.0001));
            }
        }
    }

    SECTION("Module Parameter Functionality", "Ensure we can bind modules to parameters") {

        // Dummy data to utilize

        std::vector<long double> data = {1,2,3,4,5,6,7,8,9,10};

        // Create a module to use:

        ConstModule osc(3.0);

        // Create a parameter:

        ModuleParameter mod_param(&osc);

        // Ensure the value is accurate:

        auto buffp = mod_param.get();

        for (auto iter = buffp->ibegin(); iter < buffp->iend(); ++iter) {

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(3.0, 0.0001));
        }

        // Create a dummy module:

        BufferModule src;

        // Bind dummy data buffer:

        AudioBuffer buff(data);

        src.set_rbuffer(&buff);

        // Set the source module

        mod_param.bind(&src);

        // Now, ensure values are accurate:

        buffp = mod_param.get();

        for (int i = 0; i < data.size(); ++i) {

            REQUIRE_THAT(buffp->at(i), Catch::Matchers::WithinAbs(data.at(i), 0.0001));
        }
    }
}
