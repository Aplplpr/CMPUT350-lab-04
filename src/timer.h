#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cstdint>

class Timer {
public:
    using Nanos = std::chrono::nanoseconds;
    using Micros = std::chrono::microseconds;
    using Millis = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;

    Timer() {
        start = std::chrono::steady_clock::now();
    }

    void restart() {
        start = std::chrono::steady_clock::now();
    }

    template <typename T> uint64_t click() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<T>(now - start).count();

        start = now;

        return elapsed;
    }

    template <typename T> uint64_t glance() const{
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<T>(now - start).count();

        return elapsed;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start;
};

#endif  // TIMER_H