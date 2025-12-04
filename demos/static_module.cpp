/**
 * @file static_module.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Some tests and experiments for static modules
 * @version 0.1
 * @date 2024-06-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <chrono>
#include <type_traits>
#include <iostream>

#include "audio_module.hpp"
#include "source_module.hpp"
#include "sink_module.hpp"
#include "meta_audio.hpp"
#include "amp_module.hpp"

template<typename T>
concept MModule = std::is_base_of_v<AudioModule, T>;

template <MModule B>
class StaticModule : public AudioModule {

private:

    /// Backward module
    B backward{};

public:

    StaticModule() =default;

    StaticModule(B& back) : backward(std::move(back)) {}

    void bind(B mod) { backward = mod; this->backward.set_forward(this); }

    void set_backward(B back) { this->backward = std::move(back); }

    B* get_backward() { return &(this->backward);}

    void meta_process() override {

        // Call backward module:

        this->backward.meta_process();

        this->set_buffer(this->backward.get_buffer());

        // Call processing method of our own:

        this->process();
    }
};

template <MModule B>
class StaticSink : public StaticModule<B> {

public:
    StaticSink() =default;

    StaticSink(B backw) : StaticModule<B>(nullptr, backw) {}

    void meta_process() override {

        // Just call process method:

        this->process();
    }

    void process() override {

        // Create a new buffer:

        this->set_buffer(this->create_buffer());

        // Fill the buffer with the value:

        std::fill(this->buff->sbegin(), this->buff->send(), 0);
    }
};

template <MModule B>
class StaticMult : public StaticModule<B> {

private:

    double add = 0.;

public:

    StaticMult() =default;

    void set_value(double val) {add = val;}

    double get_value() const { return this->add; }

    void process() override {

        std::transform(this->buff->ibegin(), this->buff->iend(), this->buff->ibegin(), [this](long double inv) {

            // Scale the value:

            return this->get_value() * inv;
        });
    }
};

template <typename T>
class DynamicWrap : public StaticModule<T> {

};

int main() {

    const int iter = 2000;  // Number of iterations to preform
    const int size = 2000;  // Buffer size to work with

    std::cout << "+================================+" << '\n';
    std::cout << " !Benchmarking chain performance!" << '\n';
    std::cout << "+================================+" << '\n';

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << '\n';
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    //
    // Static chain
    //

    long double screate_time = 0;
    long double sproc_time = 0;

    std::cout << " --== [ Testing Static Chain ] ==--" << std::endl;

    for (int i = 0; i < iter; ++i) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Create static chain and configure:

        StaticSink<StaticMult<StaticMult<StaticMult<StaticMult<StaticMult<StaticMult<StaticMult<StaticMult<StaticMult<SourceModule>>>>>>>>>> ssink;

        auto b1 = ssink.get_backward();
        auto b2 = b1->get_backward();
        auto b3 = b2->get_backward();
        auto b4 = b3->get_backward();
        auto b5 = b4->get_backward();
        auto b6 = b5->get_backward();
        auto b7 = b6->get_backward();
        auto b8 = b7->get_backward();
        auto b9 = b8->get_backward();
        auto b10 = b9->get_backward();


        b1->set_value(1);
        b2->set_value(2);
        b3->set_value(3);
        b4->set_value(4);
        b5->set_value(5);
        b6->set_value(6);
        b7->set_value(7);
        b8->set_value(8);
        b9->set_value(9);

        b5->get_info()->out_buffer = size;

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        auto ctime_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Static creation time [" << i << "]: " << diff << " ms" << "\n"; 

        screate_time += ctime_ms;

        // Start the clock:

        start = std::chrono::high_resolution_clock::now();

        // Preform process:

        ssink.meta_process();

        // Stop the clock:

        end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        diff = end - start;

        std::cout << "Static process time [" << i << "]: " << diff << " ms" << "\n"; 

        auto ptime_ms = std::chrono::duration<double, std::milli>(diff).count();

        sproc_time += ptime_ms;
    }

    //
    // Dynamic chain
    //

    long double dcreate_time = 0;
    long double dproc_time = 0;

    std::cout << "--== [ Testing Dynamic Chain ] ==--" << std::endl;

    for (int i = 0; i < iter; ++i) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Create the chain:

        SinkModule sink;

        AmplitudeScale a1(1);
        AmplitudeScale a2(2);
        AmplitudeScale a3(3);
        AmplitudeScale a4(4);
        AmplitudeScale a5(5);
        AmplitudeScale a6(6);
        AmplitudeScale a7(7);
        AmplitudeScale a8(8);
        AmplitudeScale a9(9);

        ConstModule source;

        sink.bind(&a1)->bind(&a2)->bind(&a3)->bind(&a4)->bind(&a5)->bind(&a6)->bind(&a7)->bind(&a8)->bind(&a9)->bind(&source);

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        auto stime_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Dynamic creation time [" << i << "]: " << diff << " ms" << "\n"; 

        dcreate_time += stime_ms;

        // Start the clock:

        start = std::chrono::high_resolution_clock::now();

        // Preform processor:

        sink.meta_process();

        end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        diff = end - start;

        auto ptime_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Dynamic process time [" << i << "]: " << diff << " ms" << "\n";

        dproc_time += ptime_ms;
    }

    std::cout << "+=======================================+" << "\n";
    std::cout << "--== [ Results ] ==--" << "\n";

    double asproc = sproc_time / iter;
    double adproc = dproc_time / iter;

    std::cout << "Average Static Creation Time: " << screate_time / iter << "\n";
    std::cout << "Average Static Process Time: " << asproc << "\n";
    std::cout << "Average Dynamic Creation Time: " << dcreate_time / iter << "\n";
    std::cout << "Average Dynamic Process Time: " << adproc << "\n";

    // Determine the percent difference:

    double per = (adproc - asproc) / ((adproc + asproc) / 2) * 100;

    std::cout << "Static processing percent: " << per << std::endl;

    return 0;
}
