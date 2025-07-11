#pragma once

#include <jtxlib.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vecmath.hpp>

namespace jtx {

template<typename T>
class Ray {
    static_assert(std::is_arithmetic_v<T>, "Ray can only be instantiated with arithmetic types");

public:
    Point3<T> origin;
    Vec3<T> dir;
    T time;

    [[nodiscard]] bool valid() const {
        return origin.valid() && dir.valid() && !jtx::isNaN(time);
    }

    //region Constructors
    JTX_DEV Ray() : origin(), dir(), time(JTX_ZERO) {}

    JTX_DEV Ray(const Point3<T> &origin, const Vec3<T> &direction, T time = T(0))
        : origin(origin), dir(direction), time(time) {
        ASSERT(valid());
    }

    JTX_DEV Ray(const Ray &other) : origin(other.origin), dir(other.dir), time(other.time) {
        ASSERT(valid());
    }

    ~Ray() = default;
    //endregion

    JTX_DEV bool operator==(const Ray &other) const {
        return origin == other.origin && dir == other.dir && time == other.time;
    }

    JTX_DEV bool operator!=(const Ray &other) const {
        return !(*this == other);
    }

    JTX_DEV bool equals(const Ray &other, T epsilon) const {
        return origin.Equals(other.origin, epsilon) && dir.Equals(other.dir, epsilon) &&
               jtx::Equals(time, other.time, epsilon);
    }

    [[nodiscard]] JTX_DEV Point3<T> At(T t) const {
        return origin + t * dir;
    }
};

template<typename T>
class RayDifferential : public Ray<T> {
    static_assert(std::is_arithmetic_v<T>, "RayDifferential can only be instantiated with arithmetic types");

public:
    Point3<T> rxOrigin, ryOrigin;
    Vec3<T> rxDirection, ryDirection;
    bool hasDiffs;

    [[nodiscard]] bool valid() const {
        return Ray<T>::valid() &&
               (!hasDiffs || (rxOrigin.valid() && ryOrigin.valid() && rxDirection.valid() && ryDirection.valid()));
    }

    //region Constructors
    JTX_HOSTDEV RayDifferential() : Ray<T>(), rxOrigin(), ryOrigin(), rxDirection(), ryDirection(), hasDiffs(false) {}

    JTX_HOSTDEV RayDifferential(const Point3<T> &origin, const Vec3<T> &direction, T time = T(0))
        : Ray<T>(origin, direction, time), rxOrigin(), ryOrigin(), rxDirection(), ryDirection(),
          hasDiffs(false) {
        ASSERT(valid());
    }

    JTX_HOSTDEV explicit RayDifferential(const Ray<T> &ray) : Ray<T>(ray), rxOrigin(), ryOrigin(), rxDirection(),
                                                              ryDirection(),
                                                              hasDiffs(false) {
        ASSERT(valid());
    }

    JTX_HOSTDEV RayDifferential(const RayDifferential &other) : Ray<T>(other), rxOrigin(other.rxOrigin),
                                                                ryOrigin(other.ryOrigin),
                                                                rxDirection(other.rxDirection),
                                                                ryDirection(other.ryDirection),
                                                                hasDiffs(other.hasDiffs) {
        ASSERT(valid());
    }
    //endregion

    JTX_HOSTDEV bool operator==(const RayDifferential &other) const {
        return Ray<T>::operator==(other) && rxOrigin == other.rxOrigin && ryOrigin == other.ryOrigin &&
               rxDirection == other.rxDirection && ryDirection == other.ryDirection && hasDiffs == other.hasDiffs;
    }

    JTX_HOSTDEV bool operator!=(const RayDifferential &other) const {
        return !(*this == other);
    }

    JTX_HOSTDEV bool equals(const RayDifferential &other, T epsilon) const {
        return Ray<T>::Equals(other, epsilon) && rxOrigin.Equals(other.rxOrigin, epsilon) &&
               ryOrigin.Equals(other.ryOrigin, epsilon) && rxDirection.Equals(other.rxDirection, epsilon) &&
               ryDirection.Equals(other.ryDirection, epsilon) && hasDiffs == other.hasDiffs;
    }

    JTX_HOSTDEV void scale(T s) {
        rxOrigin = this->origin + (rxOrigin - this->origin) * s;
        ryOrigin = this->origin + (ryOrigin - this->origin) * s;
        rxDirection = this->dir + (rxDirection - this->dir) * s;
        ryDirection = this->dir + (ryDirection - this->dir) * s;
    }
};

[[maybe_unused]] typedef Ray<float> Rayf;
[[maybe_unused]] typedef Ray<double> Rayd;

typedef RayDifferential<float> RayfDifferential;
typedef RayDifferential<double> RaydDifferential;

JTX_NUM_ONLY_T
JTX_HOST std::string toString(const Ray<T> &ray) {
    return jtx::toString(ray.origin) + " + t * " + jtx::toString(ray.dir) + " (t = " + std::to_string(ray.time) +
           ")";
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool Equals(const Ray<T> &a, const Ray<T> &b, T epsilon = JTX_EPSILON) {
    return a.Equals(b, epsilon);
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool Equals(const RayDifferential<T> &a, const RayDifferential<T> &b, T epsilon = JTX_EPSILON) {
    return a.Equals(b, epsilon);
}

}// namespace jtx
