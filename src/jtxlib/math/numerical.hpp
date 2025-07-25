/**
 * This file contains numerical utility functions primarily used by debug asserts
 */

#pragma once

#include <bit>
#include <cmath>
#include <cstdint>
#include <jtxlib.hpp>
#include <jtxlib/math/constants.hpp>
#include <algorithm>
#include <stdexcept>

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
#if defined(JTXLIB_CUDA_ENABLED)
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
#if defined(JTXLIB_CUDA_ENABLED)
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
#if defined(JTXLIB_CUDA_ENABLED)
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
        T
        CopySign(T mag, T sgn) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::copysign(mag, sgn);
#else
    return std::copysign(mag, sgn);
#endif
}

JTX_FP_ONLY_T
JTX_HOST
JTX_INLINE bool
        Equals(T a, T b, T epsilon = JTX_EPSILON) {
    return std::abs(a - b) < epsilon;
}

JTX_HOSTDEV JTX_INLINE float round(float v) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::roundf(v);
#else
    return std::round(v);
#endif
}

JTX_HOSTDEV JTX_INLINE long lround(float v) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::lroundf(v);
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

//region Basic Math Functions
template<typename T, typename U, typename V>
JTX_HOST constexpr
std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>, T>
Clamp(T val, U lo, V hi) {
    if (val < lo) return T(lo);
    else if (val > hi) return T(hi);
    else return val;
}

JTX_HOSTDEV JTX_INLINE float Clamp(float val, float lo, float hi) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::fminf(::fmaxf(val, lo), hi);
#else
    return std::clamp(val, lo, hi);
#endif
}

JTX_HOSTDEV JTX_INLINE double Clamp(double val, double lo, double hi) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::fmin(::fmax(val, lo), hi);
#else
    return std::clamp(val, lo, hi);
#endif
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T Max(T a, T b) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::max(a, b);
#else
    return std::max<T>(a, b);
#endif
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T Max(T a, T b, T c) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::max(0, ::max(a, b));
#else
    return std::max<T>({a, b, c});
#endif
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T min(T a, T b) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::min(a, b);
#else
    return std::min<T>(a, b);
#endif
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T min(T a, T b, T c) {
#if defined(JTXLIB_CUDA_ENABLED)
    return ::min(0, ::min(a, b));
#else
    return std::min<T>({a, b, c});
#endif
}

template<typename P>
JTX_HOSTDEV
JTX_INLINE size_t findInterval(size_t sz, const P &pred) {
    using ssize_t = std::make_signed_t<size_t>;
    ssize_t size = static_cast<ssize_t>(sz) - 2;
    ssize_t first = 1;
    while (size > 0) {
        size_t half = static_cast<size_t>(size) >> 1;
        size_t middle = first + half;
        const bool res = pred(middle);
        first = res ? middle + 1 : first;
        size = res ? size - (half + 1) : half;
    }

    return jtx::Clamp(static_cast<size_t>(first) - 1, 0, sz - 2);
}

}// namespace jtx
