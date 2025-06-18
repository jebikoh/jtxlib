#pragma once

#include <cmath>
#include <jtxlib.hpp>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/math/math.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/util/assert.hpp>
#include <jtxlib/util/rand.hpp>
#include <stdexcept>

namespace jtx {
template<typename T>
class Vec2;
template<typename T>
class Vec4;

template<typename T>
class Vec3 {
    static_assert(std::is_arithmetic_v<T>, "Vec3 can only be instantiated with arithmetic types");

public:
    union {
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
    };

    // Check for NaN
    [[nodiscard]] JTX_HOST bool valid() const {
        return !(jtx::isNaN(x) || jtx::isNaN(y) || jtx::isNaN(z));
    }

#pragma region Constructors
    JTX_HOSTDEV Vec3() : x(JTX_ZERO), y(JTX_ZERO), z(JTX_ZERO) {}

    JTX_HOSTDEV explicit Vec3(T v) : x(v), y(v), z(v){};

    JTX_HOSTDEV Vec3(T x, T y, T z) : x(x), y(y), z(z) {};

    JTX_HOSTDEV Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) {};

    template<typename U>
    JTX_HOSTDEV explicit Vec3(const Vec3<U> &other) : x(T(other.x)), y(T(other.y)), z(T(other.z)){};

    JTX_HOSTDEV Vec3(const Vec2<T> &other, T z) : x(other.x), y(other.y), z(z) {};
    JTX_HOSTDEV explicit Vec3(const Vec4<T> &other) : x(other.x), y(other.y), z(other.z){};

    JTX_HOSTDEV explicit Vec3(const T *data) : x(data[0]), y(data[1]), z(data[2]){};

    JTX_HOSTDEV
    static Vec3 fromXY(Vec2<T> xy, float Y) {
        if (xy.y == 0) return {0, 0, 0};
        return {xy.x * Y / xy.y, Y, (1 - xy.x - xy.y) * Y / xy.y};
    }

    ~Vec3() = default;
#pragma endregion

#pragma region Unary operators
    JTX_HOSTDEV Vec3 operator-() const {
        return {-x, -y, -z};
    }

    JTX_HOSTDEV Vec3 operator+() const {
        return {+x, +y, +z};
    }

    JTX_HOSTDEV Vec3 &operator++() {
        ++x;
        ++y;
        ++z;
        return *this;
    }

    JTX_HOSTDEV Vec3 operator++(int) {
        Vec3 temp = *this;
        ++x;
        ++y;
        ++z;
        return temp;
    }

    JTX_HOSTDEV Vec3 &operator--() {
        --x;
        --y;
        --z;
        return *this;
    }

    JTX_HOSTDEV Vec3 operator--(int) {
        Vec3 temp = *this;
        --x;
        --y;
        --z;
        return temp;
    }

    template<typename U>
    JTX_HOSTDEV explicit operator Vec3<U>() const {
        return {U(x), U(y), U(z)};
    }

    JTX_HOSTDEV explicit operator bool() const {
        return x || y || z;
    }

#pragma endregion

#pragma region Binary operators
    JTX_HOSTDEV Vec3 &operator=(const Vec3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    JTX_HOSTDEV bool operator==(const Vec3 &other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    JTX_HOSTDEV bool operator!=(const Vec3 &other) const {
        return (x != other.x) || (y != other.y) || (z != other.z);
    }

    JTX_HOSTDEV Vec3 operator+(const Vec3 &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    JTX_HOSTDEV Vec3 operator+(const T scalar) const {
        return {x + scalar, y + scalar, z + scalar};
    }

    JTX_HOSTDEV friend Vec3 operator+(T scalar, const Vec3 &v) {
        return v + scalar;
    }

    JTX_HOSTDEV Vec3 operator-(const Vec3 &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    JTX_HOSTDEV Vec3 operator-(const T scalar) const {
        return {x - scalar, y - scalar, z - scalar};
    }

    JTX_HOSTDEV friend Vec3 operator-(T scalar, const Vec3 &v) {
        return {scalar - v.x, scalar - v.y, scalar - v.z};
    }

    JTX_HOSTDEV Vec3 operator*(const Vec3 &other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    JTX_HOSTDEV Vec3 operator*(T scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    JTX_HOSTDEV friend Vec3 operator*(T scalar, const Vec3 &v) {
        return v * scalar;
    }

    JTX_HOSTDEV Vec3 operator/(const Vec3 &other) const {
        ASSERT(JTX_ZERO != other.x && JTX_ZERO != other.y && JTX_ZERO != other.z);
        return {x / other.x, y / other.y, z / other.z};
    }

    JTX_HOSTDEV Vec3 operator/(T scalar) const {
        ASSERT(JTX_ZERO != scalar);
        return {x / scalar, y / scalar, z / scalar};
    }

    JTX_HOSTDEV friend Vec3 operator/(T scalar, const Vec3 &v) {
        ASSERT(JTX_ZERO != scalar);
        return {scalar / v.x, scalar / v.y, scalar / v.z};
    }

    JTX_HOSTDEV Vec3 operator<<(uint32_t shift) {
        return {x << shift, y << shift, z << shift};
    }

    JTX_HOSTDEV Vec3 operator>>(uint32_t shift) const {
        return {x >> shift, y >> shift, z >> shift};
    }

    JTX_HOSTDEV Vec3 operator^(uint32_t scalar) const {
        return {x ^ scalar, y ^ scalar, z ^ scalar};
    }

    JTX_HOSTDEV Vec3 operator^(const Vec3 &p) const {
        return {x ^ p.x, y ^ p.y, z ^ p.z};
    }
#pragma endregion

#pragma region In-place Assignment Operators
    JTX_HOSTDEV Vec3 &operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator+=(const T scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator-=(const Vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator-=(const T scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator*=(const Vec3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator/=(const Vec3 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator^=(const Vec3 &other) {
        x ^= other.x;
        y ^= other.y;
        z ^= other.z;
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator^=(const uint32_t scalar) {
        x ^= scalar;
        y ^= scalar;
        z ^= scalar;
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator>>=(uint32_t shift) {
        x >>= shift;
        y >>= shift;
        z >>= shift;
        return *this;
    }

    JTX_HOSTDEV Vec3 &operator<<=(uint32_t shift) {
        x <<= shift;
        y <<= shift;
        z <<= shift;
        return *this;
    }

    JTX_HOSTDEV const T &operator[](int index) const {
        ASSERT(index >= 0 && index < 3);
        return (&x)[index];
    }

    JTX_HOSTDEV T &operator[](int index) {
        ASSERT(index >= 0 && index < 3);
        return (&x)[index];
    }
#pragma endregion

#pragma region Member functions
    JTX_HOSTDEV bool equals(const Vec3 &other, float epsilon = JTX_EPSILON) const {
        return jtx::equals(x, other.x, epsilon) && jtx::equals(y, other.y, epsilon) &&
               jtx::equals(z, other.z, epsilon);
    }

    [[nodiscard]] JTX_HOSTDEV T Dot(const Vec3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] JTX_HOSTDEV T Dot(const T _x, const T _y, const T _z) const {
        return this->x * _x + this->y * _y + this->z * _z;
    }

    [[nodiscard]] JTX_HOSTDEV Vec3 cross(const Vec3 &other) const {
#ifdef JTXLIB_MINIMIZE_FP_ERROR
        return {jtx::dop(y, other.z, z, other.y),
                jtx::dop(z, other.x, x, other.z),
                jtx::dop(x, other.y, y, other.x)};
#else
         return {y * other.z - z * other.y,
                 z * other.x - x * other.z,
                 x * other.y - y * other.x};
#endif
    }


    JTX_HOSTDEV Vec3 &abs() {
        x = jtx::abs(x);
        y = jtx::abs(y);
        z = jtx::abs(z);
        return *this;
    }

    JTX_HOSTDEV T absdot(const Vec3 &other) {
        return jtx::abs(Dot(other));
    }

    [[nodiscard]] JTX_HOSTDEV float LenSqr() const {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] JTX_HOSTDEV float Length() const { return jtx::Sqrt(LenSqr()); }

    JTX_HOSTDEV Vec3 &Normalize() {
        float l = Length();
        if (l != 0) {
            (*this) /= l;
        }
        return *this;
    }

    [[nodiscard]] JTX_HOSTDEV T l1norm() const {
        return jtx::abs(x) + jtx::abs(y) + jtx::abs(z);
    }

    JTX_HOSTDEV Vec3 &ceil() {
        x = jtx::ceil(x);
        y = jtx::ceil(y);
        z = jtx::ceil(z);
        return *this;
    }

    JTX_HOSTDEV Vec3 &floor() {
        x = jtx::floor(x);
        y = jtx::floor(y);
        z = jtx::floor(z);
        return *this;
    }

    [[nodiscard]] JTX_HOSTDEV auto min() const {
        return jtx::min(z, jtx::min(x, y));
    }

    [[nodiscard]] JTX_HOSTDEV auto MaxComponent() const {
        return jtx::max(z, jtx::max(x, y));
    }

    [[nodiscard]] JTX_HOSTDEV T hprod() const {
        return x * y * z;
    }

    JTX_HOSTDEV Vec3 &align(const Vec3 &other) {
        if ((*this).Dot(other) < 0.0f) {
            (*this) = -(*this);
        }
        return *this;
    }

    JTX_HOSTDEV
    float average() const { return (x + y + z) / 3; }

    JTX_HOSTDEV
    Vec2<T> xy() const {
        return {x / (x + y + z), y / (x + y + z)};
    }
#pragma endregion

#pragma region Random
    JTX_HOST static Vec3 random() {
        return {jtx::random<T>(), jtx::random<T>(), jtx::random<T>()};
    }

    JTX_HOST static Vec3 random(T min, T max) {
        return {jtx::random<T>(min, max), jtx::random<T>(min, max), jtx::random<T>(min, max)};
    }
#pragma endregion
};

template<typename T>
JTX_HOSTDEV Vec3<T> faceForward(Vec3<T> n, Vec3<T> v) {
    return (n.Dot(v) < 0.0f) ? -n : n;
}

#pragma region Type aliases
typedef Vec3<int32_t> Vec3i;
typedef Vec3<uint32_t> Vec3u;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

JTX_NUM_ONLY_T
using Point3 = Vec3<T>;
typedef Point3<int32_t> Point3i;
typedef Point3<uint32_t> Point3u;
typedef Point3<float> Point3f;
typedef Point3<double> Point3d;

JTX_NUM_ONLY_T
using Normal3 = Vec3<T>;
typedef Normal3<int32_t> Normal3i;
typedef Normal3<uint32_t> Normal3u;
typedef Normal3<float> Normal3f;
typedef Normal3<double> Normal3d;
#pragma endregion

JTX_NUM_ONLY_T
JTX_HOST std::string toString(const Vec3<T> &vec) {
    return "Vec3(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool equals(const Vec3<T> &a, const Vec3<T> &b, float epsilon = JTX_EPSILON) {
    return a.equals(b, epsilon);
}

#pragma region Vector Frame
JTX_NUM_ONLY_T
JTX_HOSTDEV void coordinateSystem(const Vec3<T> v1, Vec3<T> *v2, Vec3<T> *v3) {
    float sign = jtx::copysign(1.0f, v1.z);
    float a = -1.0f / (sign + v1.z);
    float b = v1.x * v1.y * a;
    *v2 = Vec3{1.0f + sign * v1.x * v1.x * a, sign * b, -sign * v1.x};
    *v3 = Vec3{b, sign + v1.y * v1.y * a, -v1.y};
}

class Frame {
public:
    Vec3f x, y, z;

    JTX_HOSTDEV Frame() : x{1.0f, 0.0f, 0.0f}, y{0.0f, 1.0f, 0.0f}, z{0.0f, 0.0f, 1.0f} {}
    JTX_HOSTDEV Frame(const Vec3f &x, const Vec3f &y, const Vec3f &z) : x(x), y(y), z(z) {
        ASSERT(jtx::equals(x.lenSqr(), 1.0f, 1e-4f));
        ASSERT(jtx::equals(y.lenSqr(), 1.0f, 1e-4f));
        ASSERT(jtx::equals(z.lenSqr(), 1.0f, 1e-4f));
        ASSERT(jtx::equals(x.dot(y), 0.0f, 1e-4f));
        ASSERT(jtx::equals(y.dot(z), 0.0f, 1e-4f));
        ASSERT(jtx::equals(z.dot(x), 0.0f, 1e-4f));
    }

    JTX_HOSTDEV static Frame fromXZ(const Vec3f &x, const Vec3f &z) {
        return {x, z.cross(x), z};
    }

    JTX_HOSTDEV static Frame fromXY(const Vec3f &x, const Vec3f &y) {
        return {x, y, x.cross(y)};
    }

    JTX_HOSTDEV static Frame fromYZ(const Vec3f &y, const Vec3f &z) {
        return {y.cross(z), y, z};
    }

    // WARNING: THESE FUNCTIONS SET THE 2 CALCULATED VECTORS ARBITRARILY

    JTX_HOSTDEV static Frame FromZ(const Vec3f &z) {
        Vec3f x, y;
        coordinateSystem(z, &x, &y);
        return {x, y, z};
    }

    JTX_HOSTDEV static Frame fromX(const Vec3f &x) {
        Vec3f y, z;
        coordinateSystem(x, &y, &z);
        return {x, y, z};
    }

    JTX_HOSTDEV static Frame fromY(const Vec3f &y) {
        Vec3f x, z;
        coordinateSystem(y, &z, &x);
        return {x, y, z};
    }

    [[nodiscard]] JTX_HOSTDEV Vec3f ToLocal(const Vec3f &v) const {
        return {v.Dot(x), v.Dot(y), v.Dot(z)};
    }

    [[nodiscard]] JTX_HOSTDEV Vec3f ToWorld(const Vec3f &v) const {
        return x * v.x + y * v.y + z * v.z;
    }
};

JTX_HOST JTX_INLINE std::string toString(const Frame &vec) {
    return "Frame(" + jtx::toString(vec.x) + ", " + jtx::toString(vec.y) + ", " + jtx::toString(vec.z) + ")";
}
#pragma endregion
}// namespace jtx
