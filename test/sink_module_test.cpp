/**
 * @file sink_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for sink modules
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <catch2/catch_test_macros.hpp>

#include "sink_module.hpp"
#include "meta_audio.hpp"

TEST_CASE("SinkModule Test", "[sink]") {

    // Create a SinkModule:

    SinkModule sink;

    SECTION("InfoSync", "Ensures we sync info from ChainInfo") {

        // Set some crazy values:

        auto* info = sink.get_chain_info();

        info->buffer_size = 123;
        info->channels = 55;
        info->sample_rate = 678;

        // Preform the info sync:

        sink.info_sync();

        // Ensure values are correct:

        auto* ainfo = sink.get_info();

        REQUIRE(ainfo->in_buffer == info->buffer_size);
        REQUIRE(ainfo->out_buffer == info->buffer_size);
        REQUIRE(ainfo->channels == info->channels);
        REQUIRE(ainfo->sample_rate == info->sample_rate);
    }

    SECTION("ChainInfo", "Ensures the chain info is correct") {

        // Ensures the ChainInfo is present:
        
        REQUIRE(sink.get_chain_info() != nullptr);
    }
}

TEST_CASE("PeriodSink", "[sink]") {

    // Create a period sink:

    PeriodSink sink;

    SECTION("Period", "Ensure the period getter/setter is correct") {

        // Ensure that the default value is correct:

        REQUIRE(sink.get_period() == 1);

        // Change the value:

        sink.set_period(5);

        // Check this value is now correct:

        REQUIRE(sink.get_period() == 5);
    }

    SECTION("MetaProcess", "Ensures meta processing works correctly") {

        // Create a ConstantOscillator:

        ConstModule oconst(5);

        // Create a count module:

        Counter count;

        // Bind the two modules together:

        sink.bind(&count)->bind(&oconst);

        // Meta process the sink:

        sink.meta_process();

        // Get the audio info:

        auto* info = oconst.get_info();

        // Ensure the size is correct:

        REQUIRE(count.samples() == info->out_buffer * sink.get_period());
        REQUIRE(count.processed() == sink.get_period());

        // Now, test with different period:

        const int period = 5;
        sink.set_period(period);
        count.reset();

        // Again, meta process the sink:

        sink.meta_process();

        // Finally, ensure size is correct:

        REQUIRE(count.samples() == info->out_buffer * sink.get_period());
        REQUIRE(count.processed() == sink.get_period());
    }
}
