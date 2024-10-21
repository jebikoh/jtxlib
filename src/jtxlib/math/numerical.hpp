/**
 * This file contains numerical utility functions primarily used by debug asserts
 */

#pragma once

#include <stdexcept>
#include <cmath>
#include <jtxlib.hpp>
#include <jtxlib/math/constants.hpp>
#include <cstdint>
#include <bit>

#define JTX_NUM_ONLY(TypeName) template<typename TypeName = T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>

#define JTX_NUM_ONLY_T template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
#define JTX_FP_ONLY_T template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
#define JTX_INT_ONLY_T template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>

#define JTX_ENABLE_FP_BOOL typename std::enable_if_t<std::is_floating_point_v<T>, bool>
#define JTX_ENABLE_INT_BOOL typename std::enable_if_t<std::is_integral_v<T>, bool>
#define JTX_ENABLE_FP_T typename std::enable_if_t<std::is_floating_point_v<T>, T>
#define JTX_ENABLE_INT_T typename std::enable_if_t<std::is_integral_v<T>, T>

namespace jtx {
    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_FP_BOOL
    isNaN(T v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::isnan(v);
#else
        return std::isnan(v);
#endif
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_INT_BOOL
    isNaN(T v) {
        return false;
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_FP_T
    ceil(T v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::ceil(v);
#else
        return std::ceil(v);
#endif
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_INT_T
    ceil(T v) {
        return v;
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_FP_T
    floor(T v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::floor(v);
#else
        return std::floor(v);
#endif
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE
    JTX_ENABLE_INT_T
    floor(T v) {
        return v;
    }

    JTX_FP_ONLY_T
    JTX_HOSTDEV JTX_INLINE
    T copysign(T mag, T sgn) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::copysign(mag, sgn);
#else
        return std::copysign(mag, sgn);
#endif
    }

    JTX_HOST JTX_FP_ONLY_T
    JTX_INLINE
    bool equals(T a, T b, T epsilon = EPSILON) {
        return std::abs(a - b) < epsilon;
    }

    JTX_HOSTDEV JTX_INLINE float round(float v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::roundf(v);
#else
        return std::round(v);
#endif
    }

    JTX_HOSTDEV JTX_INLINE
    long lround(float v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::lroundf()(v);
#else
        return std::lround(v);
#endif
    }

    JTX_HOST JTX_INLINE uint32_t floatToBits(float v) {
        return std::bit_cast<uint32_t>(v);
    }

    JTX_HOST JTX_INLINE float bitsToFloat(uint32_t v) {
        return std::bit_cast<float>(v);
    }
} // jtx
