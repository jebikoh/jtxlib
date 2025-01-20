#pragma once

#include <cmath>
#include <stdexcept>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/util/assert.hpp>
#include <jtxlib/math/math.hpp>
#include <string>

namespace jtx {
    template<typename T> class Vec2;
    template<typename T> class Vec3;

    template<typename T>
    class Vec4 {
        static_assert(std::is_arithmetic_v<T>, "Vec4 can only be instantiated with arithmetic types");
    public:
        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T r, g, b, a;
            };
        };

        // Check for NaN
        [[nodiscard]] JTX_HOST JTX_INLINE bool valid() const {
            return !(jtx::isNaN(x) || jtx::isNaN(y) || jtx::isNaN(z) || jtx::isNaN(w));
        }

        //region Constructors
        JTX_HOSTDEV Vec4() : x(JTX_ZERO), y(JTX_ZERO), z(JTX_ZERO), w(JTX_ZERO) {};

        JTX_HOSTDEV Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { ASSERT(valid()); };

        JTX_HOSTDEV Vec4(const Vec4 &other) : x(other.x), y(other.y), z(other.z), w(other.w) { ASSERT(valid()); };

        template<typename U>
        JTX_HOSTDEV explicit Vec4(const Vec4<U> &other) : x(T(other.x)), y(T(other.y)), z(T(other.z)), w(T(other.w)) {
            ASSERT(valid());
        };

        JTX_HOSTDEV Vec4(const Vec2<T> &other, T z, T w) : x(other.x), y(other.y), z(z), w(w) { ASSERT(valid()); };
        JTX_HOSTDEV Vec4(const Vec3<T> &other, T w) : x(other.x), y(other.y), z(other.z), w(w) { ASSERT(valid()); };

        ~Vec4() = default;
        //endregion

        //region Unary operators
        JTX_HOSTDEV JTX_INLINE Vec4 operator-() const {
            return {-x, -y, -z, -w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator+() const {
            return {+x, +y, +z, +w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator++() {
            x++;
            y++;
            z++;
            w++;
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator++(int) {
            Vec4 temp = *this;
            x++;
            y++;
            z++;
            w++;
            return temp;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator--() {
            x--;
            y--;
            z--;
            w--;
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator--(int) {
            Vec4 temp = *this;
            x--;
            y--;
            z--;
            w--;
            return temp;
        }
        //endregion

        //region Binary operators
        JTX_HOSTDEV JTX_INLINE Vec4 &operator=(const Vec4 &other) {
            ASSERT(other.valid());
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE bool operator==(const Vec4 &other) const {
            return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
        }

        JTX_HOSTDEV JTX_INLINE bool operator!=(const Vec4 &other) const {
            return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator+(const Vec4 &other) const {
            return {x + other.x, y + other.y, z + other.z, w + other.w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator+(const T scalar) const {
            return {x + scalar, y + scalar, z + scalar, w + scalar};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator-(const Vec4 &other) const {
            return {x - other.x, y - other.y, z - other.z, w - other.w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator-(const T scalar) const {
            return {x - scalar, y - scalar, z - scalar, w - scalar};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator*(const Vec4 &other) const {
            return {x * other.x, y * other.y, z * other.z, w * other.w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator*(T scalar) const {
            return {x * scalar, y * scalar, z * scalar, w * scalar};
        }

        friend JTX_HOSTDEV JTX_INLINE Vec4 operator*(T scalar, const Vec4 &v) {
            return v * scalar;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator/(const Vec4 &other) const {
            ASSERT(JTX_ZERO != other.x && JTX_ZERO != other.y && JTX_ZERO != other.z && JTX_ZERO != other.w);
            return {x / other.x, y / other.y, z / other.z, w / other.w};
        }

        JTX_HOSTDEV JTX_INLINE Vec4 operator/(T scalar) const {
            ASSERT(JTX_ZERO != scalar);
            return {x / scalar, y / scalar, z / scalar, w / scalar};
        }

        friend JTX_HOSTDEV JTX_INLINE Vec4 operator/(T scalar, const Vec4 &v) {
            ASSERT(JTX_ZERO != scalar);
            return {scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w};
        }

        JTX_HOSTDEV JTX_INLINE const T &operator[](int index) const {
            ASSERT(index >= 0 && index < 4);
            return (&x)[index];
        }

        JTX_HOSTDEV JTX_INLINE T &operator[](int index) {
            ASSERT(index >= 0 && index < 4);
            return (&x)[index];
        }
        //endregion

        //region In-place Assignment Operators
        JTX_HOSTDEV JTX_INLINE Vec4 &operator+=(const Vec4 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator+=(const T scalar) {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator-=(const Vec4 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator-=(const T scalar) {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator*=(const Vec4 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator/=(const Vec4 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            ASSERT(valid());
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &operator/=(T scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            ASSERT(valid());
            return *this;
        }
        //endregion

        //region Member functions
        JTX_HOSTDEV bool equals(const Vec4 &other, float epsilon = EPSILON) const {
            return jtx::equals(x, other.x, epsilon) && jtx::equals(y, other.y, epsilon) &&
                   jtx::equals(z, other.z, epsilon) && jtx::equals(w, other.w, epsilon);
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE T dot(const Vec4 &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE T dot(const T _x, const T _y, const T _z, const T _w) const {
            return this->x * _x + this->y * _y + this->z * _z + this->w * _w;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &abs() {
            x = jtx::abs(x);
            y = jtx::abs(y);
            z = jtx::abs(z);
            w = jtx::abs(w);
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE T absdot(const Vec4 &other) {
            return jtx::abs(dot(other));
        }

        [[nodiscard]] JTX_HOSTDEV JTX_INLINE float len() const { return jtx::sqrt(x * x + y * y + z * z + w * w); }

        JTX_HOSTDEV JTX_INLINE Vec4 &normalize() {
            float l = len();
            if (l != 0) {
                (*this) /= l;
            }
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &ceil() {
            x = jtx::ceil(x);
            y = jtx::ceil(y);
            z = jtx::ceil(z);
            w = jtx::ceil(w);
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE Vec4 &floor() {
            x = jtx::floor(x);
            y = jtx::floor(y);
            z = jtx::floor(z);
            w = jtx::floor(w);
            return *this;
        }

        JTX_HOSTDEV JTX_INLINE auto min() const {
            return jtx::min(jtx::min(z, w), jtx::min(x, y));
        }

        JTX_HOSTDEV JTX_INLINE auto max() const {
            return jtx::max(jtx::max(z, w), jtx::max(x, y));
        }

        JTX_HOSTDEV JTX_INLINE T hprod() const {
            return x * y * z * w;
        }
        //endregion
    };

    typedef Vec4<int> Vec4i;
    typedef Vec4<float> Vec4f;

    JTX_NUM_ONLY_T
    using Point4 = Vec4<T>;

    typedef Point4<int> Point4i;
    typedef Point4<float> Point4f;

    JTX_NUM_ONLY_T
    JTX_HOSTDEV JTX_INLINE bool equals(const Vec4<T> &a, const Vec4<T> &b, float epsilon = EPSILON) {
        return a.equals(b, epsilon);
    }

    JTX_HOST JTX_INLINE std::string toString(const Vec4f &vec) {
        return "Vec4f(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " +
               std::to_string(vec.w) + ")";
    }

    JTX_HOST JTX_INLINE std::string toString(const Vec4i &vec) {
        return "Vec4i(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " +
               std::to_string(vec.w) + ")";
    }
}// namespace jtx
