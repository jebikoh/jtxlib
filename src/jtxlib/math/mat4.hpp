#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#pragma once

#include <span>
#include <optional>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/util/assert.hpp>
#include <jtxlib/math/vec4.hpp>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vecmath.hpp>
#include <jtxlib/math/ray.hpp>
#include <jtxlib/math/bounds.hpp>

#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)

#include <cuda/std/optional>
#include <cuda/std/span>

#endif

namespace jtx {
    class Mat4 {
    public:
        float data[4][4];
        //region Constructors
        JTX_HOSTDEV Mat4() {
            for (auto &i: data) {
                for (float &j: i) {
                    j = 0.0f;
                }
            }
        }

        JTX_HOSTDEV explicit Mat4(float diag) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    data[i][j] = (i == j) ? diag : 0.0f;
                }
            }
        }

        JTX_HOSTDEV Mat4(const Mat4 &mat) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    data[i][j] = mat[i][j];
                }
            }
        }

        JTX_HOSTDEV Mat4(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33) {
            data[0][0] = m00;
            data[0][1] = m01;
            data[0][2] = m02;
            data[0][3] = m03;
            data[1][0] = m10;
            data[1][1] = m11;
            data[1][2] = m12;
            data[1][3] = m13;
            data[2][0] = m20;
            data[2][1] = m21;
            data[2][2] = m22;
            data[2][3] = m23;
            data[3][0] = m30;
            data[3][1] = m31;
            data[3][2] = m32;
            data[3][3] = m33;
        }

        JTX_HOSTDEV explicit Mat4(const float mat[4][4]) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    data[i][j] = mat[i][j];
                }
            }
        }

        JTX_HOSTDEV explicit Mat4(const Frame &f) : Mat4(f.x.x, f.y.x, f.z.x, 0.0f,
                                                        f.x.y, f.y.y, f.z.y, 0.0f,
                                                        f.x.z, f.y.z, f.z.z, 0.0f,
                                                        0.0f, 0.0f, 0.0f, 1.0f) {}

        ~Mat4() = default;

        JTX_HOSTDEV static Mat4 identity() {
            return Mat4(1.0f);
        }

        JTX_HOSTDEV static Mat4 diagonal(float d0, float d1, float d2, float d3) {
            Mat4 m;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i != j) m[i][j] = 0;
                }
            }
            m[0][0] = d0;
            m[1][1] = d1;
            m[2][2] = d2;
            m[3][3] = d3;
            return m;
        }
        //endregion

        //region Binary operators
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        JTX_HOSTDEV cuda::std::span<const float> operator[](int i) const {
            return {data[i]};
        }

        JTX_HOSTDEV cuda::std::span<float> operator[](int i) {
            return {data[i]};
        }
#else
        JTX_HOST std::span<const float> operator[](int i) const {
            return {data[i]};
        }

        JTX_HOST std::span<float> operator[](int i) {
            return {data[i]};
        }

#endif

        JTX_HOSTDEV Mat4 operator+(const Mat4 &other) const {
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] += other[i][j];
                }
            }
            return res;
        }

        JTX_HOSTDEV Mat4 operator+(const float scalar) const {
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] += scalar;
                }
            }
            return res;
        }

        JTX_HOSTDEV friend Mat4 operator+(const float scalar, const Mat4 &mat) {
            return mat + scalar;
        }

        JTX_HOSTDEV Mat4 operator-(const Mat4 &other) const {
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] -= other[i][j];
                }
            }
            return res;
        }

        JTX_HOSTDEV Mat4 operator-(const float scalar) const {
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] -= scalar;
                }
            }
            return res;
        }

        JTX_HOSTDEV Mat4 operator*(const float scalar) const {
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] *= scalar;
                }
            }
            return res;
        }

        JTX_HOSTDEV friend Mat4 operator*(const float scalar, const Mat4 &mat) {
            return mat * scalar;
        }

        JTX_HOSTDEV Mat4 operator/(const float scalar) const {
            ASSERT(scalar != 0.0f);
            Mat4 res = *this;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] /= scalar;
                }
            }
            return res;
        }

        JTX_HOSTDEV bool operator==(const Mat4 &other) const {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (data[i][j] != other[i][j]) {
                        return false;
                    }
                }
            }
            return true;
        }

        JTX_HOSTDEV bool operator!=(const Mat4 &other) const {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (data[i][j] != other[i][j]) {
                        return true;
                    }
                }
            }
            return false;
        }
        //endregion

        //region In-line Methods
        JTX_HOSTDEV bool equals(const Mat4 &other, float epsilon = EPSILON) const {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (jtx::abs(data[i][j] - other[i][j]) > epsilon) {
                        return false;
                    }
                }
            }
            return true;
        }

        [[nodiscard]] JTX_HOSTDEV bool isIdentity() const {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i == j) {
                        if (data[i][j] != 1) return false;
                    } else {
                        if (data[i][j] != 0) return false;
                    }
                }
            }
            return true;
        }

        [[nodiscard]] JTX_HOSTDEV bool hasNaN() const {
            for (auto i : data) {
                for (int j = 0; j < 4; ++j) {
                    if (jtx::isNaN(i[j])) {
                        return true;
                    }
                }
            }
            return false;
        }

        // Use .mul() for higher precision
        // Use .apply() when not required (they're faster)
        [[nodiscard]] JTX_HOSTDEV Vec4f mul(const Vec4f &vec) const {
            Vec4f res;
            for (int i = 0; i < 4; ++i) {
                res[i] = jtx::innerProdf(data[i][0], vec[0], data[i][1], vec[1], data[i][2], vec[2], data[i][3],
                                         vec[3]);
            }
            return res;
        }

        [[nodiscard]] JTX_HOSTDEV Mat4 mul(const Mat4 &mat) const {
            Mat4 res;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] = jtx::innerProdf(data[i][0], mat[0][j], data[i][1], mat[1][j], data[i][2], mat[2][j],
                                                data[i][3],
                                                mat[3][j]);
                }
            }
            return res;
        }

        JTX_HOSTDEV Mat4 operator*(const Mat4 &other) const {
            return this->mul(other);
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_HOSTDEV JTX_INLINE Point3<T> applyToPoint(const Point3<T> &p) const {
            T xp = data[0][0] * p.x + data[0][1] * p.y + data[0][2] * p.z + data[0][3];
            T yp = data[1][0] * p.x + data[1][1] * p.y + data[1][2] * p.z + data[1][3];
            T zp = data[2][0] * p.x + data[2][1] * p.y + data[2][2] * p.z + data[2][3];
            T wp = data[3][0] * p.x + data[3][1] * p.y + data[3][2] * p.z + data[3][3];

            ASSERT(wp != 0.0f);
            if (wp == 1) {
                return {xp, yp, zp};
            } else {
                return Point3f(xp, yp, zp) / wp;
            }
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_HOSTDEV JTX_INLINE Vec3<T> applyToVec(const Vec3<T> &v) const {
            return {data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z,
                    data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z,
                    data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z};
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_HOSTDEV JTX_INLINE Vec4<T> applyToVec(const Vec4<T> &v) const {
            return {data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z + data[0][3] * v.w,
                    data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z + data[1][3] * v.w,
                    data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z + data[2][3] * v.w,
                    data[3][0] * v.x + data[3][1] * v.y + data[3][2] * v.z + data[3][3] * v.w};
        }

        JTX_NUM_ONLY_T
        [[nodiscard]] JTX_HOSTDEV JTX_INLINE Normal3<T> applyToNormal(const Normal3<T> &n) const {
            return {data[0][0] * n.x + data[1][0] * n.y + data[2][0] * n.z,
                    data[0][1] * n.x + data[1][1] * n.y + data[2][1] * n.z,
                    data[0][2] * n.x + data[1][2] * n.y + data[2][2] * n.z};
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE Rayf applyToRay(const Rayf &ray) const {
            auto o = (*this).applyToPoint(ray.origin);
            auto d = (*this).applyToVec(ray.dir);
            return {o, d, ray.time};
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE RayfDifferential applyToRayDiff(const RayfDifferential &ray) const {
            Rayf r = applyToRay(ray);
            RayfDifferential ret(r);
            if (ray.hasDiffs) {
                ret.hasDiffs = true;
                ret.rxOrigin = applyToPoint(ray.rxOrigin);
                ret.ryOrigin = applyToPoint(ray.ryOrigin);
                ret.rxDirection = applyToVec(ray.rxDirection);
                ret.ryDirection = applyToVec(ray.ryDirection);
            }
            return ret;
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE BBox3f applyToBBox(const BBox3f &bbox) const {
            BBox3f ret;
            for (int i = 0; i < 8; i++) {
                ret = ret.merge(applyToPoint(bbox.corner(i)));
            }
            return ret;
        }

        [[nodiscard]] JTX_HOSTDEV Mat4 transpose() const {
            Mat4 res;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    res[i][j] = data[j][i];
                }
            }
            return res;
        }

        [[nodiscard]] JTX_HOSTDEV float determinant() const;

        [[nodiscard]] JTX_HOSTDEV float determinant3x3() const;

#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)

        [[nodiscard]] JTX_HOSTDEV cuda::std::optional<Mat4> inverse() const;

#else

        [[nodiscard]] JTX_HOST std::optional<Mat4> inverse() const;

#endif
        //endregion
    };

    JTX_HOSTDEV JTX_INLINE Vec4f mul(const Mat4 &mat, const Vec4f &vec) { return mat.mul(vec); }

    JTX_HOSTDEV JTX_INLINE Mat4 mul(const Mat4 &a, const Mat4 &b) { return a.mul(b); }

    JTX_HOSTDEV JTX_INLINE Mat4 transpose(const Mat4 &mat) { return mat.transpose(); }

#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
    JTX_HOSTDEV JTX_INLINE cuda::std::optional<Mat4> inverse(const Mat4 &mat) { return mat.inverse(); }

    JTX_HOSTDEV JTX_INLINE cuda::std::optional<Mat4> linearLS(const Mat4 &A, const Mat4 &B) {
        Mat4 AtA = Mat4{};
        Mat4 AtB = Mat4{};

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    AtA[i][j] += A[k][i] * A[k][j];
                    AtB[i][j] += A[k][i] * B[k][j];
                }
            }
        }

        auto AtA_inv = inverse(AtA);
        if (!AtA_inv.has_value()) { return {}; }
        return jtx::transpose((*AtA_inv).mul(AtB));
    }
#else

    JTX_HOSTDEV JTX_INLINE std::optional<Mat4> inverse(const Mat4 &mat) { return mat.inverse(); }

    JTX_HOSTDEV JTX_INLINE std::optional<Mat4> linearLS(const Mat4 &A, const Mat4 &B) {
        Mat4 AtA = Mat4{};
        Mat4 AtB = Mat4{};

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    AtA[i][j] += A[k][i] * A[k][j];
                    AtB[i][j] += A[k][i] * B[k][j];
                }
            }
        }

        auto AtA_inv = inverse(AtA);
        if (!AtA_inv.has_value()) { return {}; }
        return jtx::transpose((*AtA_inv).mul(AtB));
    }

#endif

    JTX_HOSTDEV JTX_INLINE Mat4 invert(const Mat4 &mat) {
        auto inv = inverse(mat);
        ASSERT(inv.has_value());
        return inv.value();
    }

    //region Matrix Transformations
    JTX_HOSTDEV JTX_INLINE Mat4 translate(const float delta) {
        return {
                1.0f, 0.0f, 0.0f, delta,
                0.0f, 1.0f, 0.0f, delta,
                0.0f, 0.0f, 1.0f, delta,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 translate(float x, float y, float z) {
        return {
                1.0f, 0.0f, 0.0f, x,
                0.0f, 1.0f, 0.0f, y,
                0.0f, 0.0f, 1.0f, z,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 translate(const jtx::Vec3f &v) {
        return translate(v.x, v.y, v.z);
    }

    JTX_HOSTDEV JTX_INLINE Mat4 scale(float s) {
        return Mat4::diagonal(s, s, s, 1.0f);
    }

    JTX_HOSTDEV JTX_INLINE Mat4 scale(float x, float y, float z) {
        return Mat4::diagonal(x, y, z, 1.0f);
    }

    JTX_HOSTDEV JTX_INLINE Mat4 scale(const jtx::Vec3f &v) {
        return Mat4::diagonal(v.x, v.y, v.z, 1.0f);
    }

    JTX_HOSTDEV JTX_INLINE Mat4 rotateX(float theta) {
        float sinTheta = jtx::sin(jtx::radians(theta));
        float cosTheta = jtx::cos(jtx::radians(theta));
        return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cosTheta, -sinTheta, 0.0f,
                0.0f, sinTheta, cosTheta, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 rotateY(float theta) {
        float sinTheta = jtx::sin(jtx::radians(theta));
        float cosTheta = jtx::cos(jtx::radians(theta));
        return {
                cosTheta, 0.0f, sinTheta, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -sinTheta, 0.0f, cosTheta, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 rotateZ(float theta) {
        float sinTheta = jtx::sin(jtx::radians(theta));
        float cosTheta = jtx::cos(jtx::radians(theta));
        return {
                cosTheta, -sinTheta, 0.0f, 0.0f,
                sinTheta, cosTheta, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV Mat4 rotate(float sinTheta, float cosTheta, const Vec3f &axis);

    JTX_HOSTDEV Mat4 rotate(float theta, const jtx::Vec3f &axis);

    JTX_HOSTDEV Mat4 rotateFromTo(const Vec3f &from, const Vec3f &to);

    JTX_HOSTDEV JTX_INLINE Mat4
    lookAt(const Vec3f &right, const Vec3f &vup, const Vec3f &direction, const Vec3f &position) {
        return {
                right.x, right.y, right.z, -jtx::dot(right, position),
                vup.x, vup.y, vup.z, -jtx::dot(vup, position),
                direction.x, direction.y, direction.z, -jtx::dot(direction, position),
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 lookAt(const Vec3f &position, const Vec3f &target, const Vec3f &up) {
        auto direction = (target - position).normalize();
        Vec3f right = jtx::cross(up, direction).normalize();
        Vec3f vup = jtx::cross(direction, right).normalize();
        return lookAt(right, vup, direction, position);
    }

    JTX_HOSTDEV JTX_INLINE Mat4 perspective(float fov, float aspect, float near, float far) {
        float invTanAng = 1.0f / std::tan(jtx::radians(fov) / 2.0f);
        return {
                invTanAng / aspect, 0.0f, 0.0f, 0.0f,
                0.0f, invTanAng, 0.0f, 0.0f,
                0.0f, 0.0f, (far + near) / (near - far), 2.0f * far * near / (near - far),
                0.0f, 0.0f, -1.0f, 0.0f
        };
    }

    JTX_HOSTDEV JTX_INLINE Mat4 orthographic(float left, float right, float top, float bottom, float near, float far) {
        return {
                2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
                0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
                0.0f, 0.0f, -2.0f / (far - near), -(far + near) / (far - near),
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    //endregion

    JTX_HOST std::string toString(const Mat4 &mat);

    JTX_HOSTDEV JTX_INLINE bool equals(const Mat4 &a, const Mat4 &b, float epsilon = EPSILON) {
        return a.equals(b, epsilon);
    }
}
#pragma clang diagnostic pop