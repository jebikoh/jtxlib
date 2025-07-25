#pragma once

#include <jtxlib.hpp>
#include <jtxlib/math/math.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/math/vec2.hpp>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vec4.hpp>

namespace jtx {
#pragma region Dot Product
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T Dot(const Vec2<T> &a, const Vec2<T> &b) {
    return a.x * b.x + a.y * b.y;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T Dot(const Vec3<T> &a, const Vec3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T Dot(const Vec4<T> &a, const Vec4<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
#pragma endregion

#pragma region ABS
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> Abs(const Vec2<T> &v) {
    return {jtx::Abs(v.x), jtx::Abs(v.y)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> Abs(const Vec3<T> &v) {
    return {jtx::Abs(v.x), jtx::Abs(v.y), jtx::Abs(v.z)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> Abs(const Vec4<T> &v) {
    return {jtx::Abs(v.x), jtx::Abs(v.y), jtx::Abs(v.z), jtx::Abs(v.w)};
}
#pragma endregion

#pragma region ABS Dot Product
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T AbsDot(const Vec2<T> &a, const Vec2<T> &b) {
    return jtx::Abs(a.Dot(b));
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T AbsDot(const Vec3<T> &a, const Vec3<T> &b) {
    return jtx::Abs(a.Dot(b));
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T AbsDot(const Vec4<T> &a, const Vec4<T> &b) {
    return jtx::Abs(a.Dot(b));
}
#pragma endregion

#pragma region Normalize
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> Normalize(const Vec2<T> &v) {
    float l = v.Length();
    if (l != 0) {
        return v / l;
    } else {
        return Vec2<T>{};
    }
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> Normalize(const Vec3<T> &v) {
    float l = v.Length();
    if (l != 0) {
        return v / l;
    } else {
        return Vec3<T>{};
    }
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> Normalize(const Vec4<T> &v) {
    float l = v.Length();
    if (l != 0) {
        return v / l;
    } else {
        return Vec4<T>{};
    }
}
#pragma endregion

#pragma region Ceil & Floor
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> ceil(const Vec2<T> &v) {
    return {jtx::ceil(v.x), jtx::ceil(v.y)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> ceil(const Vec3<T> &v) {
    return {jtx::ceil(v.x), jtx::ceil(v.y), jtx::ceil(v.z)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> ceil(const Vec4<T> &v) {
    return {jtx::ceil(v.x), jtx::ceil(v.y), jtx::ceil(v.z), jtx::ceil(v.w)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> floor(const Vec2<T> &v) {
    return {jtx::floor(v.x), jtx::floor(v.y)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> floor(const Vec3<T> &v) {
    return {jtx::floor(v.x), jtx::floor(v.y), jtx::floor(v.z)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> floor(const Vec4<T> &v) {
    return {jtx::floor(v.x), jtx::floor(v.y), jtx::floor(v.z), jtx::floor(v.w)};
}
#pragma endregion

#pragma region Min & Max
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> min(const Vec2<T> &a, const Vec2<T> &b) {
    return {std::min(a.x, b.x), std::min(a.y, b.y)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> min(const Vec3<T> &a, const Vec3<T> &b) {
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> min(const Vec4<T> &a, const Vec4<T> &b) {
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> Max(const Vec2<T> &a, const Vec2<T> &b) {
    return {std::max(a.x, b.x), std::max(a.y, b.y)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> Max(const Vec3<T> &a, const Vec3<T> &b) {
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> Max(const Vec4<T> &a, const Vec4<T> &b) {
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
}
#pragma endregion

#pragma region Horizontal Product
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T hprod(const Vec2<T> &v) {
    return v.x * v.y;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T hprod(const Vec3<T> &v) {
    return v.x * v.y * v.z;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE T hprod(const Vec4<T> &v) {
    return v.x * v.y * v.z * v.w;
}
#pragma endregion

#pragma region LERP
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE auto Lerp(const Vec2<T> &a, const Vec2<T> &b, T t) {
    return (1 - t) * a + b * t;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE auto
Lerp(const Vec3<T> &a, const Vec3<T> &b, T t) {
    return (1 - t) * a + b * t;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE auto Lerp(const Vec4<T> &a, const Vec4<T> &b, T t) {
    return (1 - t) * a + b * t;
}
#pragma endregion

#pragma region FMA
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> fma(const Vec2<T> &a, const Vec2<T> &b, const Vec2<T> &c) {
    return a * b + c;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> fma(const Vec3<T> &a, const Vec3<T> &b, const Vec3<T> &c) {
    return a * b + c;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec4<T> fma(const Vec4<T> &a, const Vec4<T> &b, const Vec4<T> &c) {
    return a * b + c;
}
#pragma endregion

#pragma region Angle between
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float angle(const Vec2<T> &a, const Vec2<T> &b) {
    if (a.Dot(b) < 0) {
        return JTX_PI_F - 2 * jtx::clampAsin((a - b).Length() / 2);
    }
    return 2 * jtx::clampAsin((a - b).Length() / 2);
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float angle(const Vec3<T> &a, const Vec3<T> &b) {
    if (a.Dot(b) < 0) {
        return JTX_PI_F - 2 * jtx::clampAsin((a - b).Length() / 2);
    }
    return 2 * jtx::clampAsin((a - b).Length() / 2);
}
#pragma endregion

#pragma region Gram-Schmidt
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec2<T> gramSchmidt(const Vec2<T> &a, const Vec2<T> &b) {
    return a - (b * a.Dot(b));
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> gramSchmidt(const Vec3<T> &a, const Vec3<T> &b) {
    return a - (b * a.Dot(b));
}
#pragma endregion

#pragma region Distance
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distanceSqr(const Vec2<T> &a, const Vec2<T> &b) {
    return (a - b).LengthSquared();
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distanceSqr(const Vec3<T> &a, const Vec3<T> &b) {
    return (a - b).LengthSquared();
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distanceSqr(const Vec4<T> &a, const Vec4<T> &b) {
    return (a - b).LengthSquared();
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distance(const Vec2<T> &a, const Vec2<T> &b) {
    return (a - b).Length();
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distance(const Vec3<T> &a, const Vec3<T> &b) {
    return (a - b).Length();
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE float distance(const Vec4<T> &a, const Vec4<T> &b) {
    return (a - b).Length();
}
#pragma endregion

#pragma region Vec3 Specific
JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> Cross(const Vec3<T> &a, const Vec3<T> &b) {
#ifdef JTXLIB_MINIMIZE_FP_ERROR
    return {jtx::dop(a.y, b.z, a.z, b.y),
            jtx::dop(a.z, b.x, a.x, b.z),
            jtx::dop(a.x, b.y, a.y, b.x)};
#else
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
#endif
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> align(const Vec3<T> &a, const Vec3<T> &b) {
    return (a.Dot(b) < 0.0f) ? -a : a;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> reflect(const Vec3<T> &v, const Vec3<T> &n) {
    return v - 2 * Dot(v, n) * n;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV JTX_INLINE Vec3<T> refract(const Vec3<T> &uv, const Vec3<T> &n, T etai_over_etat) {
    auto cos_theta = ::fminf(jtx::Dot(-uv, n), 1.0);
    auto r_out_perp = etai_over_etat * (uv + cos_theta * n);
    auto r_out_prll = -::sqrtf(::fabs(1.0 - r_out_perp.LengthSquared())) * n;
    return r_out_perp + r_out_prll;
}
#pragma endregion
}// namespace jtx