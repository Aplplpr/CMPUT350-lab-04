#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

#include "p2a.h"
#include "timer.h"

constexpr size_t X = 256;
constexpr size_t Y = 256;
constexpr size_t Z = 256;

constexpr size_t KERNEL_SIZE = 4;
constexpr size_t STRIDE = 4;

constexpr size_t OUTPUT_X = 64;
constexpr size_t OUTPUT_Y = 64;
constexpr size_t OUTPUT_Z = 64;

constexpr size_t ARRAY_SIZE = X * Y * Z;
constexpr size_t KERNEL_TOTAL_SIZE = KERNEL_SIZE * KERNEL_SIZE * KERNEL_SIZE;
constexpr size_t OUTPUT_SIZE = OUTPUT_X * OUTPUT_Y * OUTPUT_Z;

// Row-major index for A
size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    return z * Y * X + y * X + x;
}

// Row-major index for the kernel
size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    return z * KERNEL_SIZE * KERNEL_SIZE + y * KERNEL_SIZE + x;
}

// Row-major index for the convolution output
size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    return z * OUTPUT_Y * OUTPUT_X + y * OUTPUT_X + x;
}

int main() {
    // Allocate the input arrays
    uint64_t* A = new uint64_t[ARRAY_SIZE];
    uint64_t* B = new uint64_t[ARRAY_SIZE];

    // Initialize A in row-major memory order
    std::mt19937_64 rng(0);

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        A[i] = rng();
    }

    // Copy A into B using Morton-order indexing
    for (size_t z = 0; z < Z; z++) {
        for (size_t y = 0; y < Y; y++) {
            for (size_t x = 0; x < X; x++) {
                B[morton3d(x, y, z)] =
                    A[rowMajorIndexA(x, y, z)];
            }
        }
    }

    // Allocate the kernels
    uint64_t* Ka = new uint64_t[KERNEL_TOTAL_SIZE];
    uint64_t* Kb = new uint64_t[KERNEL_TOTAL_SIZE];

    // Initialize both kernels
    for (size_t z = 0; z < KERNEL_SIZE; z++) {
        for (size_t y = 0; y < KERNEL_SIZE; y++) {
            for (size_t x = 0; x < KERNEL_SIZE; x++) {
                uint64_t value = x + y + z;

                Ka[rowMajorIndexK(x, y, z)] = value;
                Kb[morton3d(x, y, z)] = value;
            }
        }
    }

    // Allocate convolution outputs
    uint64_t* convA = new uint64_t[OUTPUT_SIZE];
    uint64_t* convB = new uint64_t[OUTPUT_SIZE];

    Timer timer;

    // Row-major convolution
    timer.restart();

    for (size_t oz = 0; oz < OUTPUT_Z; oz++) {
        for (size_t oy = 0; oy < OUTPUT_Y; oy++) {
            for (size_t ox = 0; ox < OUTPUT_X; ox++) {
                uint64_t sum = 0;

                for (size_t kz = 0; kz < KERNEL_SIZE; kz++) {
                    for (size_t ky = 0; ky < KERNEL_SIZE; ky++) {
                        for (size_t kx = 0; kx < KERNEL_SIZE; kx++) {
                            size_t x = ox * STRIDE + kx;
                            size_t y = oy * STRIDE + ky;
                            size_t z = oz * STRIDE + kz;

                            sum +=
                                A[rowMajorIndexA(x, y, z)] *
                                Ka[rowMajorIndexK(kx, ky, kz)];
                        }
                    }
                }

                convA[rowMajorIndexConv(ox, oy, oz)] = sum;
            }
        }
    }

    uint64_t rowMajorTime = timer.click<Timer::Micros>();

    // Morton-order convolution
    timer.restart();

    for (size_t output = 0; output < OUTPUT_SIZE; output++) {
        uint64_t sum = 0;

        for (size_t k = 0; k < KERNEL_TOTAL_SIZE; k++) {
            sum += B[output * KERNEL_TOTAL_SIZE + k] * Kb[k];
        }

        convB[output] = sum;
    }

    uint64_t mortonTime = timer.click<Timer::Micros>();

    // Check that both convolutions produced the same results
    for (size_t z = 0; z < OUTPUT_Z; z++) {
        for (size_t y = 0; y < OUTPUT_Y; y++) {
            for (size_t x = 0; x < OUTPUT_X; x++) {
                assert(
                    convA[rowMajorIndexConv(x, y, z)] == convB[morton3d(x, y, z)]
                );
            }
        }
    }

    // Print the convolution runtimes
    std::cout << rowMajorTime << "\n";
    std::cout << mortonTime << "\n";

    // Free all heap-allocated memory
    delete[] A;
    delete[] B;
    delete[] Ka;
    delete[] Kb;
    delete[] convA;
    delete[] convB;

    return 0;
}