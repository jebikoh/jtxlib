/**
 * This class is very similar to Mat4, but takes the extra step of calculating the inverse.
 */
#pragma once

#include <jtxlib/math/mat4.hpp>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vecmath.hpp>
#include <jtxlib/math/ray.hpp>
#include <jtxlib/math/bounds.hpp>


namespace jtx {
    struct Transform {
        //region Constructors
        Transform() : m(Mat4(1.0f)), mInv(Mat4(1.0f)) {};

        Transform(const Mat4 &m, const Mat4 &mInv) : m(m), mInv(mInv) {};

        explicit Transform(const Mat4 &m) : m(m) {
            std::optional<Mat4> inv = m.inverse();
            if (inv.has_value()) {
                mInv = inv.value();
            } else {
                float nan = std::numeric_limits<float>::signaling_NaN();
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        mInv[i][j] = nan;
                    }
                }
            }
        }

        explicit Transform(const float mat[4][4]) : Transform(Mat4(mat)) {};

        explicit Transform(const Frame &f) : Transform(Mat4(f)) {};

        ~Transform() = default;
        //endregion

        //region Getters
        [[nodiscard]] const Mat4 &getMatrix() const {
            return m;
        }

        [[nodiscard]] const Mat4 &getInverseMatrix() const {
            return mInv;
        }
        //endregion

        //region Operators
        bool operator==(const Transform &other) const {
            return m == other.m;
        }

        bool operator!=(const Transform &other) const {
            return m != other.m;
        }

        JTX_INLINE Transform operator*(const Transform &other) const {
            return {m * other.m, other.mInv * mInv};
        }
        //endregion

        //region Methods
        [[nodiscard]] bool isIdentity() const { return m.isIdentity(); }

        [[nodiscard]] JTX_INLINE Transform inverse() const {
            return {mInv, m};
        }

        [[nodiscard]] JTX_INLINE Transform transpose() const {
            return {m.transpose(), mInv.transpose()};

        }

        // Need to differentiate because points and normals are just typedefs

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Point3<T> applyToPoint(const Point3<T> &p) const {
            return m.applyToPoint(p);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Point3<T> applyInverseToPoint(const Point3<T> &p) const {
            return mInv.applyToPoint(p);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Vec3<T> applyToVec(const Vec3<T> &v) const {
            return m.applyToVec(v);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Vec3<T> applyInverseToVec(const Vec3<T> &v) const {
            return mInv.applyToVec(v);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Normal3<T> applyToNormal(const Normal3<T> &n) const {
            return mInv.applyToNormal(n);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_INLINE Normal3<T> applyInverseToNormal(const Normal3<T> &n) const {
            return m.applyToNormal(n);
        }

        // TODO: add edge of error corrections
        [[nodiscard]] JTX_INLINE Rayf applyToRay(const Rayf &ray) const {
            return m.applyToRay(ray);
        }

        [[nodiscard]] JTX_INLINE Rayf applyInverseToRay(const Rayf &ray) const {
            return mInv.applyToRay(ray);
        }

        [[nodiscard]] JTX_INLINE RayfDifferential applyToRayDiff(const RayfDifferential &ray) const {
            return m.applyToRayDiff(ray);
        }

        [[nodiscard]] JTX_INLINE RayfDifferential applyInverseToRayDiff(const RayfDifferential &ray) const {
            return mInv.applyToRayDiff(ray);
        }

        [[nodiscard]] JTX_INLINE BBox3f applyToBBox(const BBox3f &bbox) const {
            return m.applyToBBox(bbox);
        }

        [[nodiscard]] JTX_INLINE BBox3f applyInverseToBBox(const BBox3f &bbox) const {
            return mInv.applyToBBox(bbox);
        }

        [[nodiscard]] JTX_INLINE bool swapsHandedness() const {
            return m.determinant3x3() < 0;
        }
        //endregion

        //region Static methods
        static Transform translate(const float delta) {
            return {jtx::translate(delta), jtx::translate(-delta)};
        }

        static Transform translate(float x, float y, float z) {
            return {jtx::translate(x, y, z), jtx::translate(-x, -y, -z)};
        }

        static Transform translate(const Vec3f &v) {
            return {jtx::translate(v.x, v.y, v.z), jtx::translate(-v.x, -v.y, -v.z)};
        }

        static Transform scale(float sx, float sy, float sz) {
            ASSERT(sx != 0 && sy != 0 && sz != 0);
            return {jtx::scale(sx, sy, sz), jtx::scale(1 / sx, 1 / sy, 1 / sz)};
        }

        static Transform rotateX(float theta) {
            auto m = jtx::rotateX(theta);
            return {m, m.transpose()};
        }

        static Transform rotateY(float theta) {
            auto m = jtx::rotateY(theta);
            return {m, m.transpose()};
        }

        static Transform rotateZ(float theta) {
            auto m = jtx::rotateZ(theta);
            return {m, m.transpose()};
        }

        static Transform rotate(float sinTheta, float cosTheta, const Vec3f& axis) {
            Mat4 m = jtx::rotate(sinTheta, cosTheta, axis);
            return {m, m.transpose()};
        }

        static Transform rotate(float theta, const Vec3f& axis) {
            Mat4 m = jtx::rotate(theta, axis);
            return {m, m.transpose()};
        }

        // NOTE: should only be used on vectors
        static Transform rotateFromTo(const Vec3f& from, const Vec3f& to) {
            Mat4 m = jtx::rotateFromTo(from, to);
            return {m, m.transpose()};
        }

        // Note to self, PBRT calculates worldFromCamera, then inverts it
        // Old JTX calculated cameraFromWorld (and also was row-major)
        static Transform lookAt(const Point3f &pos, const Point3f &target, const Vec3f &up) {
            Vec3f dir = jtx::normalize(target - pos);
            ASSERT((jtx::cross(jtx::normalize(up), dir)).len() != 0);
            Vec3f right = jtx::normalize(jtx::cross(jtx::normalize(up), dir));
            Vec3f newUp = jtx::cross(dir, right);

            Mat4 m(right.x, newUp.x, dir.x, pos.x,
                   right.y, newUp.y, dir.y, pos.y,
                   right.z, newUp.z, dir.z, pos.z,
                   0, 0, 0, 1);
            return {jtx::invert(m), m};
        }
        //endregion

        Mat4 m;
        Mat4 mInv;
    };

    JTX_INLINE Transform inverse(const Transform &t) {
        return t.inverse();
    }

    JTX_INLINE Transform transpose(const Transform &t) {
        return t.transpose();
    }
}