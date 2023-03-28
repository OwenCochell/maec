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

#include <gtest/gtest.h>

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

/**
 * @brief Ensures the Collection constructs correctly
 * 
 */
TEST(CollectionTest, Construct) {

    Collection<DummyObject> coll;
}

/**
 * @brief Ensures adding and getting objects works correctly
 * 
 */
TEST(CollectionTest, AddGet) {

    Collection<DummyObject> coll;

    // Create some objects:

    auto obj1 = std::make_unique<DummyObject>(1);
    auto obj2 = std::make_unique<DummyObject>(2);
    auto obj3 = std::make_unique<DummyObject>(3);

    auto* obj1p = obj1.get();
    auto* obj2p = obj2.get();
    auto* obj3p = obj3.get();

    // Add each object:

    coll.add_object(obj1);

    ASSERT_EQ(1, coll.num_objects());
    ASSERT_EQ(obj1p, coll.get_object(0));

    coll.add_object(obj2);

    ASSERT_EQ(2, coll.num_objects());
    ASSERT_EQ(obj2p, coll.get_object(1));

    coll.add_object(obj3);

    ASSERT_EQ(3, coll.num_objects());
    ASSERT_EQ(obj3p, coll.get_object(2));

    // Finally, ensure clear works correctly:

    coll.clear();

    ASSERT_EQ(0, coll.num_objects());

}

/**
 * @brief Ensures freeing and releasing objects work
 * 
 */
TEST(CollectionTest, FreeRelease) {

    Collection<DummyObject> coll;

    // Create some objects:

    auto obj1 = std::make_unique<DummyObject>(1);
    auto obj2 = std::make_unique<DummyObject>(2);

    auto* obj2p = obj2.get();

    // Add each object:

    coll.add_object(obj1);
    coll.add_object(obj2);

    // Release the second object:

    auto* release = coll.release_object(1);

    ASSERT_EQ(obj2p, release);
    ASSERT_EQ(release->get_val(), 2);

    // Free the first object

    coll.free_object(0);

}
