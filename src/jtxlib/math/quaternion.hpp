#pragma once
#include <jtxlib/math/vec3.hpp>

namespace jtx {

class Quaternion {
public:
    float w;
    Vec3f v;

    /**
     * Returns an identity quaternion
     */
    JTX_HOSTDEV Quaternion() : w(1), v(0, 0, 0) {}
    JTX_HOSTDEV Quaternion(const float w, const Vec3f &v) : w(w), v(v) {}
    JTX_HOSTDEV Quaternion(const float w, const float x, const float y, const float z) : w(w), v(x, y, z) {}

    JTX_HOSTDEV static Quaternion pure(const Vec3f &v) { return {0, v}; }
    JTX_HOSTDEV static Quaternion real(float w) { return {w, 0, 0, 0}; }

    JTX_HOSTDEV Quaternion operator+(const Quaternion &q) const { return {w + q.w, v + q.v}; }
    JTX_HOSTDEV Quaternion operator-(const Quaternion &q) const { return {w - q.w, v - q.v}; }
    JTX_HOSTDEV Quaternion operator-() const { return {-w, -v}; }
    JTX_HOSTDEV Quaternion operator*(const Quaternion &q) const { return {w * q.w - v.Dot(q.v), w * q.v + q.w * v + v.cross(q.v)}; }
    JTX_HOSTDEV Quaternion operator*(const float s) const { return {w * s, v * s}; }
    JTX_HOSTDEV Quaternion operator/(const float s) const {
        ASSERT(jtx::abs(s) > JTX_EPSILON);
        return {w / s, v / s};
    }

    JTX_HOSTDEV Quaternion &operator+=(const Quaternion &q) {
        w += q.w;
        v += q.v;
        return *this;
    }
    JTX_HOSTDEV Quaternion &operator-=(const Quaternion &q) {
        w -= q.w;
        v -= q.v;
        return *this;
    }
    JTX_HOSTDEV Quaternion &operator*=(const Quaternion &q) {
        *this = *this * q;
        return *this;
    }

    JTX_HOSTDEV Quaternion &operator*=(const float s) {
        w *= s;
        v *= s;
        return *this;
    }
    JTX_HOSTDEV Quaternion &operator/=(const float s) {
        ASSERT(s > 0.0f);
        w /= s;
        v /= s;
        return *this;
    }

    JTX_HOSTDEV friend Quaternion operator*(const float s, const Quaternion &q) { return q * s; }

    JTX_HOSTDEV bool operator==(const Quaternion &q) const { return w == q.w && v == q.v; }

    [[nodiscard]] JTX_HOSTDEV float dot(const Quaternion &q) const { return w * q.w + v.Dot(q.v); }
    [[nodiscard]] JTX_HOSTDEV float len() const { return jtx::Sqrt(this->dot(*this)); }
    /**
     * Normalizes this quaternion
     */
    JTX_HOSTDEV void normalize() {
        const float l = len();
        w /= l;
        v /= l;
    }

    /**
     * Returns a normalized copy of this quaternion
     * @return Normalized quaternion
     */
    JTX_HOSTDEV Quaternion normalized() const {
        const float l = len();
        return {w / l, v / l};
    }

    [[nodiscard]] JTX_HOSTDEV Quaternion conjugate() const { return {w, -v}; }
    [[nodiscard]] JTX_HOSTDEV Quaternion inverse() const { return conjugate() / dot(*this); }

    /**
         * Calculates the angle between this and another quaternion
         * Assumes both quaternions are normalized
         * @param q Other quaternion
         * @return Angle between this and q
         */
    [[nodiscard]] JTX_HOSTDEV float angle(const Quaternion &q) const {
        // This is what PBRT does
        // http://www.plunk.org/~hatch/rightway.html
        float theta = 2.0f * jtx::clampAsin((q - (*this)).len() / 2);
        if (dot(q) < 0) return jtx::JTX_PI_F - theta;
        return theta;
    }

    [[nodiscard]] JTX_HOSTDEV bool equals(const Quaternion &q, float epsilon = JTX_EPSILON) const {
        return jtx::Equals(w, q.w, epsilon) && v.equals(q.v, epsilon);
    }
};

JTX_HOSTDEV JTX_INLINE float Dot(const Quaternion &q1, const Quaternion &q2) { return q1.dot(q2); }
JTX_HOSTDEV JTX_INLINE Quaternion Normalize(const Quaternion &q) { return q / q.len(); }
JTX_HOSTDEV JTX_INLINE float angle(const Quaternion &q1, const Quaternion &q2) { return q1.angle(q2); }

JTX_HOSTDEV JTX_INLINE Quaternion slerp(const Quaternion &q1, const Quaternion &q2, float t) {
    // One again: http://www.plunk.org/~hatch/rightway.html
    const float theta = q1.angle(q2);
    const float sxoxTheta = SinXOverX(theta);
    return SinXOverX((1 - t) * theta) / sxoxTheta * (1 - t) * q1 + (SinXOverX(t * theta) / sxoxTheta) * t * q2;
}

JTX_HOSTDEV JTX_INLINE bool Equals(const Quaternion &a, const Quaternion &b, float epsilon = JTX_EPSILON) {
    return a.equals(b, epsilon);
}
}// namespace jtx