#pragma once

#include <cmath>
#include <jtxlib/math/constants.hpp>
#include <jtxlib/math/math.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/util/assert.hpp>
#include <stdexcept>
#include <string>

namespace jtx {
template<typename T>
class Vec3;
template<typename T>
class Vec4;

template<typename T>
class Vec2 {
    static_assert(std::is_arithmetic_v<T>, "Vec2 can only be instantiated with arithmetic types");

public:
    T x, y;

    // Check for NaN
    [[nodiscard]] JTX_HOST bool valid() const {
        return !(jtx::isNaN(x) || jtx::isNaN(y));
    }

    //region Constructors
    JTX_HOSTDEV Vec2() : x(JTX_ZERO), y(JTX_ZERO) {};

    JTX_HOSTDEV Vec2(T x, T y) : x(x), y(y) { ASSERT(valid()); };

    JTX_HOSTDEV Vec2(const Vec2 &other) : x(other.x), y(other.y) { ASSERT(valid()); };

    template<typename U>
    JTX_HOSTDEV explicit Vec2(const Vec2<U> &other) : x(T(other.x)), y(T(other.y)) { ASSERT(valid()); };

    JTX_HOSTDEV Vec2(const Vec3<T> &other, T z) : x(other.x), y(other.y) { ASSERT(valid()); };

    JTX_HOSTDEV Vec2(const Vec4<T> &other) : x(other.x), y(other.y) { ASSERT(valid()); };

    JTX_HOSTDEV explicit Vec2(const float *data) : x(data[0]), y(data[1]) { ASSERT(valid()); };

    ~Vec2() = default;
    //endregion

    //region Unary operators
    JTX_HOSTDEV Vec2 operator-() const {
        return {-x, -y};
    }

    JTX_HOSTDEV Vec2 operator+() const {
        return {+x, +y};
    }

    JTX_HOSTDEV Vec2 &operator++() {
        ++x;
        ++y;
        return *this;
    }

    JTX_HOSTDEV Vec2 operator++(int) {
        Vec2 temp = *this;
        ++x;
        ++y;
        return temp;
    }

    JTX_HOSTDEV Vec2 &operator--() {
        --x;
        --y;
        return *this;
    }

    JTX_HOSTDEV Vec2 operator--(int) {
        Vec2 temp = *this;
        --x;
        --y;
        return temp;
    }
    //endregion

    //region Binary operators
    JTX_HOSTDEV Vec2 &operator=(const Vec2 &other) {
        ASSERT(other.valid());
        x = other.x;
        y = other.y;
        return *this;
    }

    JTX_HOSTDEV bool operator==(const Vec2 &other) const {
        return (x == other.x) && (y == other.y);
    }

    JTX_HOSTDEV bool operator!=(const Vec2 &other) const {
        return (x != other.x) || (y != other.y);
    }

    JTX_HOSTDEV Vec2 operator+(const Vec2 &other) const {
        return {x + other.x, y + other.y};
    }

    JTX_HOSTDEV Vec2 operator+(const T scalar) const {
        return {x + scalar, y + scalar};
    }

    JTX_HOSTDEV Vec2 operator-(const Vec2 &other) const {
        return {x - other.x, y - other.y};
    }

    JTX_HOSTDEV Vec2 operator-(const T scalar) const {
        return {x - scalar, y - scalar};
    }

    JTX_HOSTDEV Vec2 operator*(const Vec2 &other) const {
        return {x * other.x, y * other.y};
    }

    JTX_HOSTDEV Vec2 operator*(T scalar) const {
        return {x * scalar, y * scalar};
    }

    friend JTX_HOSTDEV Vec2 operator*(T scalar, const Vec2 &v) {
        return v * scalar;
    }

    JTX_HOSTDEV Vec2 operator/(const Vec2 &other) const {
        ASSERT(JTX_ZERO != other.x && JTX_ZERO != other.y);
        return {x / other.x, y / other.y};
    }

    JTX_HOSTDEV Vec2 operator/(T scalar) const {
        ASSERT(JTX_ZERO != scalar);
        return {x / scalar, y / scalar};
    }

    friend JTX_HOSTDEV Vec2 operator/(T scalar, const Vec2 &v) {
        ASSERT(JTX_ZERO != scalar);
        return {scalar / v.x, scalar / v.y};
    }
    //endregion

    //region In-place Assignment Operators
    JTX_HOSTDEV Vec2 &operator+=(const Vec2 &other) {
        x += other.x;
        y += other.y;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator+=(const T scalar) {
        x += scalar;
        y += scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator-=(const Vec2 &other) {
        x -= other.x;
        y -= other.y;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator-=(const T scalar) {
        x -= scalar;
        y -= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator*=(const Vec2 &other) {
        x *= other.x;
        y *= other.y;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator/=(const Vec2 &other) {
        x /= other.x;
        y /= other.y;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV Vec2 &operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        ASSERT(valid());
        return *this;
    }

    JTX_HOSTDEV const T &operator[](int index) const {
        ASSERT(index >= 0 && index < 2);
        return (&x)[index];
    }

    JTX_HOSTDEV T &operator[](int index) {
        ASSERT(index >= 0 && index < 2);
        return (&x)[index];
    }
    //endregion

    //region Member functions
    [[nodiscard]] JTX_HOSTDEV bool equals(const Vec2 &other, float epsilon = EPSILON) const {
        return jtx::equals(x, other.x, epsilon) && jtx::equals(y, other.y, epsilon);
    }

    [[nodiscard]] JTX_HOSTDEV T dot(const Vec2 &other) const {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] JTX_HOSTDEV T dot(const T _x, const T _y) const {
        return this->x * _x + this->y * _y;
    }

    JTX_HOSTDEV Vec2 &abs() {
        x = jtx::abs(x);
        y = jtx::abs(y);
        return *this;
    }

    JTX_HOSTDEV T absdot(const Vec2 &other) {
        return jtx::abs(dot(other));
    }

    JTX_HOSTDEV float len() const { return jtx::sqrt(x * x + y * y); }

    JTX_HOSTDEV float lenSqr() const {
        return x * x + y * y;
    }

    JTX_HOSTDEV Vec2 &normalize() {
        float l = len();
        if (l != 0) {
            *this /= l;
        }
        return *this;
    }

    JTX_HOSTDEV Vec2 &ceil() {
        x = jtx::ceil(x);
        y = jtx::ceil(y);
        return *this;
    }

    JTX_HOSTDEV Vec2 &floor() {
        x = jtx::floor(x);
        y = jtx::floor(y);
        return *this;
    }

    JTX_HOSTDEV auto min() const {
        return jtx::min(x, y);
    }

    JTX_HOSTDEV auto max() const {
        return jtx::max(x, y);
    }

    JTX_HOSTDEV T hprod() const {
        return x * y;
    }
    //endregion
};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<uint32_t>  Vec2u;

JTX_NUM_ONLY_T
using Point2 = Vec2<T>;

typedef Point2<int> Point2i;
typedef Point2<float> Point2f;

JTX_HOST JTX_INLINE std::string toString(const Vec2f &vec) {
    return "Vec2f(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}

JTX_HOST JTX_INLINE std::string toString(const Vec2i &vec) {
    return "Vec2i(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool equals(const Vec2<T> &a, const Vec2<T> &b, T epsilon = EPSILON) {
    return a.equals(b, epsilon);
}
}// namespace jtx
