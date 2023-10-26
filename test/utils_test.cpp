/**
 * @file utils_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Testing for various utility components
 * @version 0.1
 * @date 2023-03-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>

#include "utils.hpp"

class DummyObject {

    private:

        /// Dummy value for testing
        int val = 0;

    public:

        DummyObject() =default;

        explicit DummyObject(int thing) : val(thing) {}

        void set_val(int thing) { this->val = thing; }

        int get_val() const { return this->val; }
};

TEST_CASE("Collection Tests", "[utils][collection]") {

    // Construct a Collection:

    Collection<DummyObject> coll;

    SECTION("Add Objects", "Ensures objects can be added to the collection") {

        // Create some objects:

        auto obj1 = std::make_unique<DummyObject>(1);
        auto obj2 = std::make_unique<DummyObject>(2);
        auto obj3 = std::make_unique<DummyObject>(3);

        auto* obj1p = obj1.get();
        auto* obj2p = obj2.get();
        auto* obj3p = obj3.get();

        // Add each object:

        coll.add_object(obj1);

        REQUIRE(1 == coll.num_objects());
        REQUIRE(obj1p == coll.get_object(0));

        coll.add_object(obj2);

        REQUIRE(2 == coll.num_objects());
        REQUIRE(obj2p == coll.get_object(1));

        coll.add_object(obj3);

        REQUIRE(3 == coll.num_objects());
        REQUIRE(obj3p == coll.get_object(2));

        SECTION("Free Objects", "Ensures Object Freeing works correctly") {

            // Release the second object:

            auto* release = coll.release_object(1);

            REQUIRE(obj2p == release);
            REQUIRE(2 == release->get_val());

            // Free the first object:

            coll.free_object(0);

            // Finally, ensure clear works correctly:

            coll.clear();

            REQUIRE(0 == coll.num_objects());
        }
    }
}
