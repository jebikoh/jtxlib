#pragma once

#include<cstdint>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vecmath.hpp>
#include <jtxlib/math/mat4.hpp>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/util/assert.hpp>
#include "./bounds.hpp"

namespace jtx {
//region Spherical coordinates
JTX_INLINE float sphericalTriangleArea(Vec3f &a, Vec3f &b, Vec3f &c) {
    return jtx::abs(2 * jtx::atan2(a.Dot(b.cross(c)), 1 + a.Dot(b) + b.Dot(c) + c.Dot(a)));
}

JTX_INLINE float sphericalQuadArea(Vec3f &a, Vec3f &b, Vec3f &c, Vec3f &d) {
    Vec3f ab = a.cross(b);
    Vec3f bc = b.cross(c);
    Vec3f cd = c.cross(d);
    Vec3f da = d.cross(a);

    if (ab.LenSqr() == 0 || bc.LenSqr() == 0 || cd.LenSqr() == 0 || da.LenSqr() == 0) return 0;

    ab.Normalize();
    bc.Normalize();
    cd.Normalize();
    da.Normalize();

    return jtx::abs(
            jtx::angle(da, -ab) + jtx::angle(ab, -bc) + jtx::angle(bc, -cd) + jtx::angle(cd, -da) - 2 * JTX_PI_F);
}

/**
 * Parameters should be given in radians
 */
JTX_INLINE Vec3f sphericalToCartesian(float sinTheta, float cosTheta, float phi) {
    return {
            jtx::clamp(sinTheta, -1.0f, 1.0f) * jtx::cos(phi),
            jtx::clamp(sinTheta, -1.0f, 1.0f) * jtx::sin(phi),
            jtx::clamp(cosTheta, -1.0f, 1.0f)
    };
}

JTX_INLINE float sphericalTheta(const Vec3f &v) {
    ASSERT(v.lenSqr() == 1.0f);
    return jtx::clampAcos(v.z);
}

JTX_INLINE float sphericalPhi(const Vec3f &v) {
    float p = jtx::atan2(v.y, v.x);
    return (p < 0) ? p + 2 * JTX_PI_F : p;
}

JTX_INLINE float CosTheta(const Vec3f &w) { return w.z; }

JTX_INLINE float Cos2Theta(const Vec3f &w) { return w.z * w.z; }

JTX_INLINE float AbsCosTheta(const Vec3f &w) { return jtx::abs(w.z); }

JTX_INLINE float Sin2Theta(const Vec3f &w) { return jtx::max(0.0f, 1.0f - Cos2Theta(w)); }

JTX_INLINE float SinTheta(const Vec3f &w) { return jtx::Sqrt(Sin2Theta(w)); }

JTX_INLINE float TanTheta(const Vec3f &w) { return SinTheta(w) / CosTheta(w); }

JTX_INLINE float Tan2Theta(const Vec3f &w) {
    const float cos2Theta = Cos2Theta(w);
    const float sin2Theta = jtx::max(0.0f, 1.0f - cos2Theta);
    return sin2Theta / cos2Theta;
}

JTX_INLINE float Tan2Theta(const Vec3f &w, const float cos2Theta) {
    const float sin2Theta = jtx::max(0.0f, 1.0f - cos2Theta);
    return sin2Theta / cos2Theta;
}

JTX_INLINE float CosPhi(const Vec3f &w) {
    const float s = SinTheta(w);
    return (s == 0) ? 1 : jtx::clamp(w.x / s, -1.0f, 1.0f);
}

JTX_INLINE float SinPhi(const Vec3f &w) {
    const float s = SinTheta(w);
    return (s == 0) ? 0 : jtx::clamp(w.y / s, -1.0f, 1.0f);
}

JTX_INLINE float cosDPhi(const Vec3f &wa, const Vec3f &wb) {
    auto waXY = wa.x * wa.x + wa.y * wa.y;
    auto wbXY = wb.x * wb.x + wb.y * wb.y;
    if (waXY == 0 || wbXY == 0) return 1;

    return jtx::clamp((wa.x * wb.x + wa.y * wb.y) / jtx::Sqrt(waXY * wbXY), -1.0f, 1.0f);
}
//endregion

//region Octahedral
class OctahedralVec {
public:
    // WARNING: PBRT modified the input vector, but this version doesn't
    explicit OctahedralVec(const Vec3f &v) {
        ASSERT(jtx::equals(v.lenSqr(), 1.0f, 1e-6f));
        auto vec = v / v.l1norm();
        if (vec.z >= 0) {
            x = encode(vec.x);
            y = encode(vec.y);
        } else {
            x = encode((1 - jtx::abs(vec.y)) * sign(vec.x));
            y = encode((1 - jtx::abs(vec.x)) * sign(vec.y));
        }
    };

    explicit operator Vec3f() const {
        Vec3f v;
        v.x = -1 + 2 * (static_cast<float>(x) / JTX_BITS_16);
        v.y = -1 + 2 * (static_cast<float>(y) / JTX_BITS_16);
        v.z = 1 - jtx::abs(v.x) - jtx::abs(v.y);
        if (v.z < 0) {
            auto xo = v.x;
            v.x = (1 - jtx::abs(v.y)) * sign(xo);
            v.y = (1 - jtx::abs(xo)) * sign(v.y);
        }
        return v.Normalize();
    }

private:
    static JTX_INLINE float sign(float f) { return jtx::copysign(1.0f, f); }

    static JTX_INLINE uint16_t encode(float f) {
        return static_cast<uint16_t>(jtx::round(jtx::clamp((f + 1) / 2, 0, 1) * JTX_BITS_16));
    }

    uint16_t x, y;
};
//endregion

//region Square-Sphere
Vec3f equalAreaSquareToSphere(const Point2f &p);

Point2f equalAreaSphereToSquare(const Point3f &d);

JTX_INLINE Point2f wrapEqualAreaSquare(Point2f p) {
    if (p.x < 0) {
        p.x = -p.x;
        p.y = 1 - p.y;
    } else if (p.x > 1) {
        p.x = 2 - p.x;
        p.y = 1 - p.y;
    }
    if (p.y < 0) {
        p.x = 1 - p.x;
        p.y = -p.y;
    } else if (p.y > 1) {
        p.x = 1 - p.x;
        p.y = 2 - p.y;
    }
    return p;
}
//endregion

//region DirectionCone
class DirectionCone {
public:
    jtx::Vec3f dir;
    float cosTheta = jtx::JTX_INFINITY_F;

    [[nodiscard]] JTX_INLINE bool isEmpty() const { return cosTheta == jtx::JTX_INFINITY_F; }

    //region Constructors
    DirectionCone() = default;

    DirectionCone(const Vec3f &dir, float cosTheta) : dir(jtx::Normalize(dir)), cosTheta(cosTheta) {}

    explicit DirectionCone(const jtx::Vec3f &dir) : DirectionCone(dir, 1.0f) {}
    //endregion

    JTX_INLINE bool operator==(const DirectionCone &other) const {
        return dir == other.dir && cosTheta == other.cosTheta;
    }

    [[nodiscard]] JTX_INLINE bool equals(const DirectionCone &other, float epsilon) const {
        return dir.equals(other.dir, epsilon) && jtx::equals(cosTheta, other.cosTheta, epsilon);
    }

    static DirectionCone entireSphere() { return {Vec3f(0, 0, 0), -1}; }
};

JTX_INLINE bool inside(const DirectionCone &cone, const Vec3f &v) {
    return !cone.isEmpty() && cone.dir.Dot(v) >= cone.cosTheta;
}

JTX_INLINE DirectionCone boundSubtendedDirection(const BBox3f &bounds, const Vec3f &p) {
    float r;
    Point3f c;
    bounds.boundingSphere(&c, &r);
    if (jtx::distanceSqr(p, c) < r * r) return DirectionCone::entireSphere();

    Vec3f w = jtx::Normalize(c - p);
    return {w, jtx::SafeSqrt(1 - (r * r) / jtx::distanceSqr(p, c))};
}

JTX_INLINE DirectionCone merge(const DirectionCone &a, const DirectionCone &b) {
    if (a.isEmpty()) return b;
    if (b.isEmpty()) return a;

    // Case 1: One cone is inside the other
    float theta_a = jtx::clampAcos(a.cosTheta);
    float theta_b = jtx::clampAcos(b.cosTheta);
    float theta_d = jtx::angle(a.dir, b.dir);

    if (jtx::min(theta_d + theta_b, JTX_PI_F) <= theta_a) return a;
    if (jtx::min(theta_d + theta_a, JTX_PI_F) <= theta_b) return b;

    // Case 2: New cone
    float theta_o = (theta_a + theta_b + theta_d) / 2;
    if (theta_o >= JTX_PI_F) return DirectionCone::entireSphere();

    float theta_r = theta_o - theta_a;
    Vec3f wr = jtx::Cross(a.dir, b.dir);
    if (wr.LenSqr() == 0) return DirectionCone::entireSphere();
    auto w = jtx::rotate(jtx::degrees(theta_r), wr).applyToVec(a.dir);
    return {w, jtx::cos(theta_o)};
}

JTX_HOSTDEV JTX_INLINE bool equals(const DirectionCone &a, const DirectionCone &b, float epsilon = JTX_EPSILON) {
    return a.equals(b, epsilon);
}
//endregion
JTX_HOSTDEV JTX_INLINE bool SameHemisphere(Vec3f w, Vec3f wp) {
    return w.z * wp.z > 0;
}
}