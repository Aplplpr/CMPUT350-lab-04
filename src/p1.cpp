#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

#include "timer.h"

constexpr size_t ROWS = 4000;
constexpr size_t COLS = 4000;
constexpr size_t SIZE = ROWS * COLS;

int main() {
    // Allocate two contiguous arrays on the heap
    uint64_t* rowMajor = new uint64_t[SIZE];
    uint64_t* columnMajor = new uint64_t[SIZE];

    std::mt19937_64 rng(0);

    // Fill the first array in row-major order
    for (size_t row = 0; row < ROWS; row++) {
        for (size_t col = 0; col < COLS; col++) {
            rowMajor[row * COLS + col] = rng();
        }
    }

    // Reset the random number generator
    rng.seed(0);

    // Fill the second array in column-major order
    for (size_t col = 0; col < COLS; col++) {
        for (size_t row = 0; row < ROWS; row++) {
            columnMajor[row * COLS + col] = rng();
        }
    }

    Timer timer;

    // Sum the first array in row-major order
    uint64_t rowMajorSum = 0;

    timer.restart();

    for (size_t row = 0; row < ROWS; row++) {
        for (size_t col = 0; col < COLS; col++) {
            rowMajorSum += rowMajor[row * COLS + col];
        }
    }

    uint64_t rowMajorTime = timer.click<Timer::Micros>();

    // Sum the second array in column-major order
    uint64_t columnMajorSum = 0;

    timer.restart();

    for (size_t col = 0; col < COLS; col++) {
        for (size_t row = 0; row < ROWS; row++) {
            columnMajorSum += columnMajor[row * COLS + col];
        }
    }

    uint64_t columnMajorTime = timer.click<Timer::Micros>();

    std::cout << rowMajorTime << " " << rowMajorSum << "\n";
    std::cout << columnMajorTime << " " << columnMajorSum << "\n";

    // Free heap memory
    delete[] rowMajor;
    delete[] columnMajor;

    return 0;
}