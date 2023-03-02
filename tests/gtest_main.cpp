/**
 * @file gtest_main.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Main gtest entry point
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "gtest/gtest.h"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
