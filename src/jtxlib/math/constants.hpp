#pragma once

#include <limits>
#include <jtxlib.hpp>

namespace jtx {
    // TODO: expand as needed
    // Structure from https://github.com/behindthepixels/EDXUtil/
    struct Zero {
        JTX_HOSTDEV explicit operator uint32_t() const { return 0; }
        JTX_HOSTDEV explicit operator uint64_t() const { return 0; }
        JTX_HOSTDEV explicit operator int32_t() const { return 0; }
        JTX_HOSTDEV explicit operator int64_t() const { return 0; }
        JTX_HOSTDEV explicit operator float() const { return 0.0f; }
        JTX_HOSTDEV explicit operator double() const { return 0.0; }

        template<typename T>
        JTX_HOSTDEV bool operator==(const T &other) const {
            return static_cast<T>(*this) == other;
        }

        template<typename T>
        JTX_HOSTDEV bool operator!=(const T &other) const {
            return static_cast<T>(*this) != other;
        }
    };

#if defined(JTXLIB_CUDA_ENABLED)
#include <cuda/std/limits>
    JTX_DEV __constant__ Zero JTX_ZERO;
    JTX_DEV __constant__ double PI = 3.14159265358979323846;
    JTX_DEV __constant__ float PI_F = 3.14159265358979323846f;
    JTX_DEV __constant__ float BITS_16 = 65535.0f;
    JTX_DEV __constant__ float INFINITY_F = cuda::std::numeric_limits<float>::max();
    JTX_DEV __constant__ double INFINITY_D = cuda::std::numeric_limits<double>::max();
    JTX_DEV __constant__ float NEG_INFINITY_F = -cuda::std::numeric_limits<float>::max();
    JTX_DEV __constant__ double NEG_INFINITY_D = -cuda::std::numeric_limits<double>::max();
    JTX_DEV __constant__ float EPSILON = 0.00001f;
#else
    constexpr Zero JTX_ZERO;
    constexpr double JTX_PI = 3.14159265358979323846;
    constexpr float JTX_PI_F = 3.14159265358979323846f;
    constexpr float JTX_BITS_16 = 65535.0f;
    constexpr float JTX_INFINITY_F = std::numeric_limits<float>::infinity();
    constexpr double JTX_INFINITY_D = std::numeric_limits<double>::infinity();
    constexpr float JTX_NEG_INFINITY_F = -std::numeric_limits<float>::infinity();
    constexpr double JTX_NEG_INFINITY_D = -std::numeric_limits<double>::infinity();
    constexpr float JTX_EPSILON = 0.00001f;
    constexpr float JTX_ONE_MINUS_EPSILON = 0x1.fffffep-1;
#endif
}
