#ifndef P2A_H
#define P2A_H

#include <cassert>
#include <cstdint>

// Expands the binary representation of input by a factor of scale
inline uint64_t expand(uint64_t input, uint32_t scale) {
    assert(scale > 0);

    uint64_t result = 0;

    for (uint32_t i = 0; i <= 63 / scale; i++) {
        if (((input >> i) & 1ull) == 1) {
            result |= (1ull << (i * scale));
        }
    }

    return result;
}

// Returns the 3D Morton code for coordinates x, y, and z
inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z) {
    return expand(x, 3) |
           (expand(y, 3) << 1) |
           (expand(z, 3) << 2);
}
#endif