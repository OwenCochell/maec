/**
 * @file util_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Test for utility functions
 * @version 0.1
 * @date 2026-05-30
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "dsp/util.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <numbers>
#include <vector>

namespace {

constexpr double TOL = 1e-9;
const double kPi = std::numbers::pi_v<double>;
const double kTwoPi = 2.0 * kPi;

/// Test data for radian/degree conversion
const std::array<std::pair<double, double>, 5> raddeg{
    {{0., 0.},
     {std::numbers::pi_v<double> / 2, 90.},
     {std::numbers::pi_v<double>, 180.},
     {std::numbers::pi_v<double> * (3. / 2.), 270},
     {std::numbers::pi_v<double> * 2, 360.}}};

TEST_CASE("Radian/Degree", "[util][conv]") {

    // Iterate over data to test

    for (const auto& data : raddeg) {

        SECTION("Radian -> Degree", "Convert radians to degrees") {

            auto deg = dsp::util::radian_deg(data.first);

            // Ensure the values match

            REQUIRE_THAT(deg, Catch::Matchers::WithinAbs(data.second, 0.0001));

            SECTION("Revert", "Ensures we can revert back to radians") {

                auto rad = dsp::util::deg_radian(deg);

                REQUIRE_THAT(rad,
                             Catch::Matchers::WithinAbs(data.first, 0.0001));
            }
        }

        SECTION("Degree -> Radian", "Convert degrees to radians") {

            auto rad = dsp::util::deg_radian(data.second);

            REQUIRE_THAT(rad, Catch::Matchers::WithinAbs(data.first, 0.0001));

            SECTION("Revert", "Ensures we can revert back to radians") {

                auto deg = dsp::util::radian_deg(rad);

                REQUIRE_THAT(deg,
                             Catch::Matchers::WithinAbs(data.second, 0.0001));
            }
        }
    }
}

TEST_CASE("Unwrap", "[util][unwrap]") {

    SECTION("No corrections", "Ensures we do nothing without discontinuities") {

        SECTION("Single element") {
            std::array<double, 1> arr = {3.14};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(3.14, TOL));
        }

        SECTION("Flat line") {
            std::array<double, 4> arr = {2.0, 2.0, 2.0, 2.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            for (const auto val : arr) {
                REQUIRE_THAT(val, Catch::Matchers::WithinAbs(2.0, TOL));
            }
        }

        SECTION("Monotonically increasing with diffs well below offset") {
            // diffs = 0.5 < discont=2.0 -> no correction
            std::array<double, 5> arr = {0.0, 0.5, 1.0, 1.5, 2.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(0.5, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(1.5, TOL));
            REQUIRE_THAT(arr[4], Catch::Matchers::WithinAbs(2.0, TOL));
        }

        SECTION("Jump below discontinuity threshold") {
            // diff = 1.5 < discont=2.0 -> no correction
            std::array<double, 2> arr = {0.0, 1.5};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.5, TOL));
        }

        SECTION("First element is always preserved") {
            // unwrap never modifies arr[0]
            std::array<double, 3> arr = {99.0, 104.0, 109.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(99.0, TOL));
        }
    }

    SECTION("Single positive wrap",
            "Ensures we can prefrom simple positive corrections") {

        SECTION("Jump of 1.25x offset") {
            // diff=5.0 >= discont=2.0, round(5.0/4.0)=1, corrected=5.0-4.0=1.0
            std::array<double, 2> arr = {0.0, 5.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
        }

        SECTION("Jump of exactly one offset") {
            // diff=4.0 >= discont=2.0, round(4.0/4.0)=1, corrected=5.0-4.0=1.0
            std::array<double, 2> arr = {1.0, 5.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
        }

        SECTION("Jump of 2.25x offset") {
            // diff=9.0 >= discont=2.0, round(9.0/4.0)=round(2.25)=2,
            // corrected=9.0-8.0=1.0
            std::array<double, 2> arr = {0.0, 9.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
        }

        SECTION("Jump of 2.625x offset") {
            // diff=10.5 >= discont=2.0, round(10.5/4.0)=round(2.625)=3,
            // corrected=10.5-12.0=-1.5
            std::array<double, 2> arr = {0.0, 10.5};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(-1.5, TOL));
        }

        SECTION("Jump of exactly three offsets") {
            // diff=12.0 >= discont=2.0, round(12.0/4.0)=3,
            // corrected=12.0-12.0=0.0
            std::array<double, 2> arr = {0.0, 12.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(0.0, TOL));
        }
    }

    SECTION("Single negative wrap",
            "Ensures we can prefrom simple negative corrections") {

        SECTION("Jump of -1.25x offset") {
            // diff=-5.0, abs=5.0 >= discont=2.0, round(5.0/4.0)=1, sign=-1
            // corrected = -5.0 - (-1)(4.0)(1) = -5.0 + 4.0 = -1.0
            std::array<double, 2> arr = {0.0, -5.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(-1.0, TOL));
        }

        SECTION("Jump of exactly negative one offset") {
            // diff=-3.0-1.0=-4.0, abs=4.0 >= discont=2.0, round(4.0/4.0)=1
            // corrected = -3.0 - (-1)(4.0)(1) = -3.0 + 4.0 = 1.0
            std::array<double, 2> arr = {1.0, -3.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
        }

        SECTION("Jump of -2.25x offset") {
            // diff=-9.0, abs=9.0 >= discont=2.0, round(9.0/4.0)=round(2.25)=2
            // corrected = -9.0 - (-1)(4.0)(2) = -9.0 + 8.0 = -1.0
            std::array<double, 2> arr = {0.0, -9.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(-1.0, TOL));
        }
    }

    SECTION("Cascading corrections",
            "We can make multiple corrections in a row if the diffs are large "
            "enough") {

        SECTION("Monotonically increasing wrapped sequence") {
            // Each value is 5.0 more than the original previous; corrections
            // cascade because prev is updated to the corrected value each step.
            // i=1: prev=0, diff=5,       round(5/4)=1,  curr=1.0, prev=1.0
            // i=2: prev=1, diff=10-1=9,  round(9/4)=2,  curr=2.0, prev=2.0
            // i=3: prev=2, diff=15-2=13, round(13/4)=3, curr=3.0, prev=3.0
            std::array<double, 4> arr = {0.0, 5.0, 10.0, 15.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(2.0, TOL));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(3.0, TOL));
        }

        SECTION("Alternating large and small jumps") {
            // Jumps >= discont=2.0 are corrected; smaller ones are left alone.
            // i=1: prev=0.0, diff=5.0,  abs>=discont, round=1, curr=1.0,
            // prev=1.0 i=2: prev=1.0, diff=-0.5, abs<discont,  no change,
            // prev=0.5 i=3: prev=0.5, diff=5.0,  abs>=discont, round=1,
            // curr=1.5, prev=1.5 i=4: prev=1.5, diff=-0.5, abs<discont,  no
            // change,         prev=1.0
            std::array<double, 5> arr = {0.0, 5.0, 0.5, 5.5, 1.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(0.5, TOL));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(1.5, TOL));
            REQUIRE_THAT(arr[4], Catch::Matchers::WithinAbs(1.0, TOL));
        }

        SECTION("Mixed positive and negative wraps") {
            // i=1: prev=0,  diff=5,  round(5/4)=1,   sign=+1, curr=1.0,
            // prev=1.0 i=2: prev=1,  diff=-5, round(5/4)=1,   sign=-1,
            // curr=0.0,  prev=0.0 i=3: prev=0,  diff=6,  round(6/4)=2, sign=+1,
            // curr=-2.0, prev=-2.0 i=4: prev=-2, diff=-1, abs=1<discont=2, no
            // correction,      prev=-3.0
            std::array<double, 5> arr = {0.0, 5.0, -4.0, 6.0, -3.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(-2.0, TOL));
            REQUIRE_THAT(arr[4], Catch::Matchers::WithinAbs(-3.0, TOL));
        }
    }

    SECTION("Phase Data", "Phase data (offset = 2*pi)") {

        SECTION("Small phase changes are not corrected") {
            // diffs = 0.5 < discont=pi -> unchanged
            std::array<double, 4> arr = {0.0, 0.5, 1.0, 1.5};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(0.5, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(1.0, TOL));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(1.5, TOL));
        }

        SECTION("Exact pi-to-negative-pi transition") {
            // diff = -pi - pi = -2*pi  (exact in IEEE 754: negating + adding
            // equal values is exact). abs=2*pi > discont=pi -> corrected.
            // round(2*pi/2*pi) = round(1.0) = 1
            // corrected = -pi - (-1)(2*pi)(1) = -pi + 2*pi = pi
            std::array<double, 3> arr = {0.0, kPi, -kPi};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(kPi, TOL));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(kPi, TOL));
        }

        SECTION("Positive jump larger than 2*pi") {
            // diff=7.0 >= discont=pi, round(7.0/2*pi)=round(1.114)=1
            // corrected = 7.0 - 2*pi
            std::array<double, 2> arr = {0.0, 7.0};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[1],
                         Catch::Matchers::WithinAbs(7.0 - kTwoPi, 1e-9));
        }

        SECTION("Negative jump larger than 2*pi") {
            // diff=-7.0, abs=7.0 >= discont=pi, round(7.0/2*pi)=1, sign=-1
            // corrected = -7.0 + 2*pi
            std::array<double, 2> arr = {0.0, -7.0};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[1],
                         Catch::Matchers::WithinAbs(-7.0 + kTwoPi, 1e-9));
        }

        SECTION("Jump larger than 4*pi") {
            // diff=14.0 >= discont=pi, round(14.0/2*pi)=round(2.228)=2
            // corrected = 14.0 - 4*pi
            std::array<double, 2> arr = {0.0, 14.0};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(
                                     14.0 - (2.0 * kTwoPi), 1e-9));
        }

        SECTION("Realistic wrapped phase signal") {
            // True phase: 0, 1, 2, 3, 4, 5, 6 radians (linear).
            // Values 4, 5, 6 have been wrapped into [-pi, pi] by subtracting
            // 2*pi.
            std::array<double, 7> arr = {
                0.0, 1.0, 2.0, 3.0, 4.0 - kTwoPi, 5.0 - kTwoPi, 6.0 - kTwoPi};
            dsp::util::unwrap(arr, kPi, kTwoPi);
            REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, 1e-9));
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, 1e-9));
            REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(2.0, 1e-9));
            REQUIRE_THAT(arr[3], Catch::Matchers::WithinAbs(3.0, 1e-9));
            REQUIRE_THAT(arr[4], Catch::Matchers::WithinAbs(4.0, 1e-9));
            REQUIRE_THAT(arr[5], Catch::Matchers::WithinAbs(5.0, 1e-9));
            REQUIRE_THAT(arr[6], Catch::Matchers::WithinAbs(6.0, 1e-9));
        }
    }

    SECTION("Non-angle corrections", "Discontinuity threshold gating") {

        SECTION("Jump above discont is corrected") {
            // diff=3.0 >= discont=2.0, round(3.0/4.0)=round(0.75)=1
            // corrected = 3.0 - 4.0 = -1.0
            std::array<double, 2> arr = {0.0, 3.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(-1.0, TOL));
        }

        SECTION("Jump below discont is not corrected") {
            // diff=1.5 < discont=2.0 -> no correction
            std::array<double, 2> arr = {0.0, 1.5};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.5, TOL));
        }

        SECTION("Jump at exactly the discont boundary is not corrected") {
            // diff=2.0, discont=2.0: 2.0 > 2.0 is false -> no correction
            std::array<double, 2> arr = {0.0, 2.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(2.0, TOL));
        }

        SECTION("Large discont suppresses correction") {
            // diff=5.0 < discont=100.0 -> no correction
            std::array<double, 2> arr = {0.0, 5.0};
            dsp::util::unwrap(arr, 100.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(5.0, TOL));
        }

        SECTION("Negative jump at exactly discont boundary is not corrected") {
            // diff=-2.0, abs=2.0, discont=2.0: 2.0 > 2.0 is false -> no
            // correction
            std::array<double, 2> arr = {0.0, -2.0};
            dsp::util::unwrap(arr, 2.0, 4.0);
            REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(-2.0, TOL));
        }

        SECTION(
            "Different discont values produce different outputs for same "
            "input") {
            std::array<double, 2> tight = {0.0, 3.0};
            std::array<double, 2> loose = {0.0, 3.0};
            dsp::util::unwrap(tight, 1.0,
                              4.0);  // discont=1.0 <= 3.0: corrected
            dsp::util::unwrap(loose, 10.0,
                              4.0);  // discont=10.0 > 3.0: not corrected
            REQUIRE_THAT(tight[1], Catch::Matchers::WithinAbs(-1.0, TOL));
            REQUIRE_THAT(loose[1], Catch::Matchers::WithinAbs(3.0, TOL));
        }
    }

    SECTION(
        "Container",
        "Ensures we can work with any container with size() and operator[]") {
        // The template accepts any container with size() and operator[],
        // including std::vector.
        // i=1: prev=0, diff=5, round=1, curr=1.0, prev=1.0
        // i=2: prev=1, diff=9, round=2, curr=2.0, prev=2.0
        std::vector<double> arr = {0.0, 5.0, 10.0};
        dsp::util::unwrap(arr, 2.0, 4.0);
        REQUIRE_THAT(arr[0], Catch::Matchers::WithinAbs(0.0, TOL));
        REQUIRE_THAT(arr[1], Catch::Matchers::WithinAbs(1.0, TOL));
        REQUIRE_THAT(arr[2], Catch::Matchers::WithinAbs(2.0, TOL));
    }
}

}  // namespace
