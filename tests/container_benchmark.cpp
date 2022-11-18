/**
 * @file container_benchmark.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains benchmarks for container types
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * These benchmarks are designed to test the performance of various containers,
 * in a way that may be similar to how they are used in the project.
 * This benchmark is poorly designed and the code is a mess, but it works.
 * 
 * Key takeaways:
 * 
 * std::array is the fastest container type (in scenarios similar to this benchmark)
 * by a large amount, around 2-4% faster for writing 
 * and 90-100% faster (!) for reading.
 * 
 * In a perfect world, we would use std::array, but we can't
 * as our buffer sizes are variable and we can't guarantee the size to the compiler.
 * The next best thing (while utilizing the standard library) is std::vector preallocated.
 * 
 * Believe it or not, dynamic arrays have similar performance to the preallocated vector.
 * Because dynamic arrays do not have the features of vector (iterators that allow the use of algorithms),
 * we will be using the preallocated vector.
 */

#include <vector>
#include <array>
#include <chrono>
#include <iostream>

/**
 * @brief Templated array - Used for testing
 * 
 */
template<std::size_t N> 
struct my_static_vector
{
    explicit my_static_vector(size_t size) { } // ...
    size_t size() const noexcept { return curr_size; }
    static size_t capacity() { return N; }
    long double& operator[](size_t pos) { return data[pos]; }
    void push_back(const long double& value) { data[curr_size++] = value; }
    // ...
private:
    std::array<long double, N> data;
    std::size_t curr_size;
};


int main(int argc, char** argv) {

    // Number of values to test:

    const int num = 440;

    // Number of times to test:

    int iterations = 1000;

    // Create a normal vector:

    std::vector<long double> vec;

    // Create vector pre-allocated:

    std::vector<long double> vec2(num);

    // Create array:

    std::array<long double, (std::size_t)num> vec4;

    // Create dynamic array:

    long double* vec5 = new long double[num];

    // Finally, templated array:

    my_static_vector<(std::size_t)num> vec3(num);

    std::cout << "+====================================+" << std::endl;
    std::cout << " !Benchmarking container performance!" << std::endl;
    std::cout << "+====================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;

    }

    // Alright, test the vector:

    long double vect_read = 0;
    long double vect_write = 0;

    std::cout << " --== [ Testing vector write performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
            
            // Start the clock:
    
            auto start = std::chrono::high_resolution_clock::now();
    
            // Add values to the vector:
    
            for (int j = 0; j < num; j++) {
    
                vec.push_back((long double)j);
    
            }
    
            // Stop the clock:
    
            auto end = std::chrono::high_resolution_clock::now();
    
            // Calculate the time:
    
            auto diff = end - start;
    
            // Print the time:
    
            std::cout << "Vector write time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

            // Add to the total:

            vect_write += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the read of the vector:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing vector read performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
    
            // Start the clock:

            auto start = std::chrono::high_resolution_clock::now();

            // Add values to the vector:

            for (int j = 0; j < num; j++) {

                long double val = vec[j];

            }

            // Stop the clock:

            auto end = std::chrono::high_resolution_clock::now();

            // Calculate the time:

            auto diff = end - start;

            // Print the time:

            std::cout << "Vector read time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

            // Add to the total:

            vect_read += std::chrono::duration <double, std::milli> (diff).count();

    }

    // Alright, test the pre allocated vector:

    long double vect2_read = 0;
    long double vect2_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing pre-allocated vector write performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
 
        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec2[j] = (long double)j;

        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Prealloc-Vector write time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        // Add to the total:

        vect2_write += std::chrono::duration <double, std::milli> (diff).count();

    }

    // Test the read of the prealloc-vector:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing pre-allocated vector read performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
            
        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();
    
        // Add values to the vector:
    
        for (int j = 0; j < num; j++) {
    
            long double val = vec2[j];
    
        }
    
        // Stop the clock:
    
        auto end = std::chrono::high_resolution_clock::now();
    
        // Calculate the time:
    
        auto diff = end - start;
    
        // Print the time:
    
        std::cout << "Prealloc-Vector read time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
        // Add to the total:
    
        vect2_read += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the templated array:

    long double vect3_read = 0;
    long double vect3_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing templated array write performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
            
        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();
    
        // Add values to the vector:
    
        for (int j = 0; j < num; j++) {
    
            vec3[j] = (long double)j;
    
        }
    
        // Stop the clock:
    
        auto end = std::chrono::high_resolution_clock::now();
    
        // Calculate the time:
    
        auto diff = end - start;
    
        // Print the time:
    
        std::cout << "Templated-Array write time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
        // Add to the total:
    
        vect3_write += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the read of the templated array:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing templated array read performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
    
        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();
    
        // Add values to the vector:
    
        for (int j = 0; j < num; j++) {
    
            long double val = vec3[j];
    
        }
    
        // Stop the clock:
    
        auto end = std::chrono::high_resolution_clock::now();
    
        // Calculate the time:
    
        auto diff = end - start;
    
        // Print the time:
    
        std::cout << "Templated-Array read time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
        // Add to the total:
    
        vect3_read += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the read of the array:

    long double vect4_read = 0;
    long double vect4_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ testing array write performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
            
        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();
    
        // Add values to the vector:
    
        for (int j = 0; j < num; j++) {
    
            vec4[j] = (long double)j;
    
        }
    
        // Stop the clock:
    
        auto end = std::chrono::high_resolution_clock::now();
    
        // Calculate the time:
    
        auto diff = end - start;
    
        // Print the time:
    
        std::cout << "Array write time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
        // Add to the total:
    
        vect4_write += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the read of the templated array:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing array read performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {
    
        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();
    
        // Add values to the vector:
    
        for (int j = 0; j < num; j++) {
    
            long double val = vec4[j];
    
        }
    
        // Stop the clock:
    
        auto end = std::chrono::high_resolution_clock::now();
    
        // Calculate the time:
    
        auto diff = end - start;
    
        // Print the time:
    
        std::cout << "Array read time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
    
        // Add to the total:
    
        vect4_read += std::chrono::duration <double, std::milli> (diff).count();
    
    }

    // Test the read of the array:

    long double vect5_read = 0;
    long double vect5_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ testing dynamic array write performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec5[j] = (long double)j;

        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Array write time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        // Add to the total:

        vect5_write += std::chrono::duration <double, std::milli> (diff).count();

    }

    // Test the read of the templated array:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing dynamic array read performance... ] ==--" << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            long double val = vec5[j];

        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Array read time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        // Add to the total:

        vect5_read += std::chrono::duration <double, std::milli> (diff).count();

    }

    // Return the results:

    std::cout << "+================================================+" << std::endl;
    std::cout << "         --== [ Vector Write Times ] ==--" << std::endl;

    std::cout << "Vector average write time: " << vect_write / iterations << " ms" << std::endl;
    std::cout << "Prealloc-Vector average write time: " << vect2_write / iterations << " ms" << std::endl;
    std::cout << "Templated-Array average write time: " << vect3_write / iterations << " ms" << std::endl;
    std::cout << "Array average write time: " << vect4_write / iterations << " ms" << std::endl;
    std::cout << "Dynamic array average write time: " << vect5_write / iterations << " ms" << std::endl;


    std::cout << "  --== [ Vector Read Times: ] ==--" << std::endl;

    std::cout << "Vector average read time: " << vect_read / iterations << " ms" << std::endl;
    std::cout << "Prealloc-Vector average read time: " << vect2_read / iterations << " ms" << std::endl;
    std::cout << "Templated-Array average read time: " << vect3_read / iterations << " ms" << std::endl;
    std::cout << "Array average read time: " << vect4_read / iterations << " ms" << std::endl;
    std::cout << "Dynamic array average read time: " << vect5_read / iterations << " ms" << std::endl;


    std::cout << "+================================================+" << std::endl;
    std::cout << " --== [ Comparisons ] ==--" << std::endl;
    std::cout << "Array write time is " << vect4_write / vect2_write << " times faster than preallocated vector write time." << std::endl;
    std::cout << "Array read time is " << vect4_read / vect2_read << " times faster than preallocated vector read time." << std::endl;

    std::cout << "Prealloc-Vector write time is " << vect2_write / vect_read << " percent faster than normal vector write time." << std::endl;
    std::cout << "Prealloc-Vector read time is " << vect2_read / vect_read << " percent faster than normal vector write time." << std::endl;

    return 0;

}
