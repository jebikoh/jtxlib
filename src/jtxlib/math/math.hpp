#pragma once

#include <cmath>
#include <algorithm>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/util/assert.hpp>
#include <jtxlib.hpp>

namespace jtx {
    //region Basic Math Functions
    template<typename T, typename U, typename V>
    JTX_HOST constexpr
    std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>, T>
    clamp(T val, U lo, V hi) {
        if (val < lo) return T(lo);
        else if (val > hi) return T(hi);
        else return val;
    }

    JTX_HOSTDEV JTX_INLINE float clamp(float val, float lo, float hi) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::fminf(::fmaxf(val, lo), hi);
#else
        return std::clamp(val, lo, hi);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double clamp(double val, double lo, double hi) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::fmin(::fmax(val, lo), hi);
#else
        return std::clamp(val, lo, hi);
#endif
    }

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE T max(T a, T b) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::max(a, b);
#else
        return std::max<T>(a, b);
#endif
    }

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE T max(T a, T b, T c) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::max(0, ::max(a, b));
#else
        return std::max<T>({a, b, c});
#endif
    }

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE T min(T a, T b) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::min(a, b);
#else
        return std::min<T>(a, b);
#endif
    }

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE T min(T a, T b, T c) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::min(0, ::min(a, b));
#else
        return std::min<T>({a, b, c});
#endif
    }

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE T abs(T v) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::abs(v);
#else
        return std::abs(v);
#endif
    }
    //endregion

    //region Trig Functions
    // clang-format off
    JTX_HOSTDEV JTX_INLINE float sin(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::sinf(theta);
#else
        return std::sin(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double sin(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::sin(theta);
#else
        return std::sin(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float cos(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::cosf(theta);
#else
        return std::cos(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double cos(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::cos(theta);
#else
        return std::cos(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float tan(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::tanf(theta);
#else
        return std::tan(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double tan(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::tan(theta);
#else
        return std::tan(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float asin(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::asinf(theta);
#else
        return std::asin(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double asin(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::asin(theta);
#else
        return std::asin(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float acos(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::acosf(theta);
#else
        return std::acos(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double acos(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::acos(theta);
#else
        return std::acos(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float atan(float theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::atanf(theta);
#else
        return std::atan(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE double atan(double theta) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::atan(theta);
#else
        return std::atan(theta);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float atan2(float y, float x) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::atan2f(y, x);
#else
        return std::atan2(y, x);
#endif
    }

    JTX_HOSTDEV JTX_INLINE float clampAsin(float theta) {
        ASSERT(theta >= -1.0001f && theta <= 1.0001f);
        return jtx::asin(jtx::clamp(theta, -1.0f, 1.0f));
    }

    JTX_HOSTDEV JTX_INLINE double clampAsin(double theta) {
        ASSERT(theta >= -1.0001f && theta <= 1.0001);
        return jtx::asin(jtx::clamp(theta, -1.0, 1.0));
    }

    JTX_HOSTDEV JTX_INLINE float clampAcos(float theta) {
        ASSERT(theta >= -1.0001f && theta <= 1.0001f);
        return jtx::acos(jtx::clamp(theta, -1.0f, 1.0f));
    }

    JTX_HOSTDEV JTX_INLINE double clampAcos(double theta) {
        ASSERT(theta >= -1.0001 && theta <= 1.0001);
        return jtx::acos(jtx::clamp(theta, -1.0, 1.0));
    }

    JTX_HOSTDEV JTX_INLINE float sinXOverX(float theta) {
        if (1.0f + theta * theta == 1.0f) return 1.0f;
        return jtx::sin(theta) / theta;
    }

    JTX_HOSTDEV JTX_INLINE float radians(float deg) {
        return deg * PI_F / 180.0f;
    }

    JTX_HOSTDEV JTX_INLINE float degrees(float rad) {
        return rad * 180.0f / PI_F;
    }
    // clang-format on
    //endregion

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    JTX_HOSTDEV JTX_INLINE T fma(T a, T b, T c) {
        return a * b + c;
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE typename std::enable_if_t<std::is_floating_point_v<T>, T>
    dop(T a, T b, T c, T d) {
        auto cd = c * d;
        auto alpha = jtx::fma(a, b, -cd);
        auto beta = jtx::fma(-c, d, cd);
        return alpha + beta;
    }

    template<typename T>
    JTX_HOSTDEV JTX_INLINE typename std::enable_if_t<std::is_integral_v<T>, T>
    dop(T a, T b, T c, T d) {
        return a * b - c * d;
    }

    JTX_FP_ONLY_T
    JTX_HOSTDEV JTX_INLINE T
    sqrt(T val) {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        return ::sqrt(val);
#else
        return std::sqrt(val);
#endif
    }

    // pow
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
    template <int n>
    JTX_HOSTDEV JTX_INLINE float pow(float v) {
        return ::powf(v, n);
    }
#else
    template <int n>
    JTX_HOSTDEV JTX_INLINE float pow(float v) {
        if constexpr (n < 0) return 1 / pow<-n>(v);
        float n2 = pow<n / 2>(v);
        return n2 * n2 * pow<n & 1>(v);
    }

    template <>
    JTX_HOSTDEV JTX_INLINE float pow<1>(float v) {
        return v;
    }

    template <>
    JTX_HOSTDEV JTX_INLINE float pow<0>(float v) {
        return 1;
    }
#endif

    JTX_FP_ONLY_T
    JTX_HOSTDEV JTX_INLINE T safeSqrt(T v) {
        return jtx::sqrt(jtx::max(0.0f, v));
    }

    JTX_HOSTDEV JTX_INLINE float lerp(float a, float b, float t) {
        return (1 - t) * a + t * b;
    }


    template<typename T, typename C, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<C>>>
    inline
    constexpr T evalPolynomial(T t, C c) {
        return c;
    }

    template<typename T, typename C, typename... Coeffs,
            typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<C>>>
    inline
    constexpr T evalPolynomial(T t, C c, Coeffs... coeffs) {
        return jtx::fma(t, evalPolynomial(t, coeffs...), c);
    }

#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
    JTX_HOSTDEV JTX_INLINE float fastExp(float x) {
        return ::expf(x);
    }
#else
    // Straight from PBRTv4
    JTX_HOST JTX_INLINE float fastExp(float x) {
        float xp = x * 1.442695041f;
        float fxp = jtx::floor(xp);
        float f = xp - fxp;
        int i = (int) fxp;
        float twoToF = evalPolynomial(f, 1.0f, 0.695556856f, 0.226173572f, 0.0781455737f);

        int exponent = i;
        if (exponent < -126) return 0;
        if (exponent > 127) return INFINITY_F;
        uint32_t bits = floatToBits(twoToF);
        bits &= 0b10000000011111111111111111111111u;
        bits |= (exponent + 127) << 23;
        return bitsToFloat(bits);
    }
#endif

    //region EFT
    struct FloatEFT {
    public:
        float v;
        float err;

        FloatEFT(float v, float err) : v(v), err(err) {}

        explicit operator float() const { return v + err; }

        float operator*(float a) const {
            return a * float(*this);
        }

        friend float operator*(float a, const FloatEFT &b) {
            return a * float(b);
        }
    };

    inline FloatEFT twoProd(float a, float b) {
        float ab = a * b;
        return {ab, jtx::fma(a, b, -ab)};
    }

    inline FloatEFT twoSum(float a, float b) {
        float s = a + b;
        float delta = s - a;
        return {s, (a - (s - delta)) + (b - delta)};
    }

    inline FloatEFT innerProd(float a, float b) {
        return twoProd(a, b);
    }

    template<typename... T>
    inline FloatEFT innerProd(float a, float b, T... terms) {
        FloatEFT ab = twoProd(a, b);
        FloatEFT tp = innerProd(terms...);
        FloatEFT sum = twoSum(ab.v, tp.v);
        return {sum.v, ab.err + tp.err + sum.err};
    }

    template<typename... T>
    inline float innerProdf(float a, float b, T... terms) {
        return float(innerProd(a, b, terms...));
    }
    //endregion
}