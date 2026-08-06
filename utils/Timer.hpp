#pragma once

#include <chrono>
#include <string>
#include <iostream>

class Timer
{
public:

    Timer(const std::string& name)
        :
        name_(name),
        start_(std::chrono::high_resolution_clock::now())
    {
    }

    ~Timer()
    {
        stop();
    }

    void stop()
    {
        if (!running_)
        {
            return;
        }
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration<double>(end - start_).count();

        std::cout
            << "[TIMER] "
            << name_
            << ": "
            << elapsed
            << " s\n";

        running_ = false;
    }

private:

    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
    bool running_ = true;
};
