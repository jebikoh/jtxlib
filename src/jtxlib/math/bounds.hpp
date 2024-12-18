#pragma once

#include <jtxlib/math/math.hpp>
#include <jtxlib/math/numerical.hpp>
#include <jtxlib/math/vec2.hpp>
#include <jtxlib/math/vec3.hpp>
#include <jtxlib/math/vecmath.hpp>
#include <jtxlib/std/std.hpp>
#include <jtxlib/util/assert.hpp>

#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
#include <cuda/std/limits>
#endif

namespace jtx {
template<typename T>
class AABB3 {
    static_assert(std::is_arithmetic_v<T>, "AABB3 can only be instantiated with arithmetic types");

public:
    Point3<T> pmin, pmax;

    // "empty" = "no volume"
    [[nodiscard]] JTX_DEV bool isEmpty() const {
        return pmin.x >= pmax.x || pmin.y >= pmax.y || pmin.z >= pmax.z;
    }

    [[nodiscard]] JTX_DEV bool isDegenerate() const {
        return pmin.x > pmax.x || pmin.y > pmax.y || pmin.z > pmax.z;
    }

    //region Constructors
    JTX_DEV AABB3() {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        T minval = cuda::std::numeric_limits<T>::lowest();
        T maxval = cuda::std::numeric_limits<T>::max();
#else
        T minval = std::numeric_limits<T>::lowest();
        T maxval = std::numeric_limits<T>::max();
#endif

        pmin = {maxval, maxval, maxval};
        pmax = {minval, minval, minval};
    }

    JTX_DEV AABB3(const Point3<T> &a, const Point3<T> &b) {
        pmin = jtx::min(a, b);
        pmax = jtx::max(a, b);
    }

    JTX_DEV explicit AABB3(const Point3<T> &p) : pmin(p), pmax(p) {
    }

    JTX_DEV AABB3(const AABB3 &other) {
        pmin = Point3<T>(other.pmin);
        pmax = Point3<T>(other.pmax);
    }

    JTX_NUM_ONLY(U)
    JTX_DEV explicit AABB3(const AABB3<U> &other) {
        if (other.isEmpty()) {
            *this = AABB3();
        } else {
            pmin = Point3<T>(other.pmin);
            pmax = Point3<T>(other.pmax);
        }
    }

    JTX_DEV ~AABB3() = default;
    //endregion

    //region Operators
    JTX_DEV bool operator==(const AABB3 &other) const {
        return pmin == other.pmin && pmax == other.pmax;
    }

    JTX_DEV bool operator!=(const AABB3 &other) const {
        return pmin != other.pmin || pmax != other.pmax;
    }
    //endregion

    //region Member functions
    JTX_DEV bool equals(const AABB3 &other, T epsilon) const {
        return pmin.equals(other.pmin, epsilon) && pmax.equals(other.pmax, epsilon);
    }

    // Modified from https://pbr-book.org/4ed/Geometry_and_Transformations/Bounding_Boxes
    [[nodiscard]] JTX_DEV Point3<T> corner(const int i) const {
        ASSERT(i >= 0 && i < 8);
        return Point3<T>(i & 1 ? pmax.x : pmin.x,
                         i & 2 ? pmax.y : pmin.y,
                         i & 4 ? pmax.z : pmin.z);
    }

    JTX_DEV AABB3 &merge(const Point3<T> &p) {
        pmin = jtx::min(pmin, p);
        pmax = jtx::max(pmax, p);
        return *this;
    }

    JTX_DEV AABB3 &merge(const AABB3 &b) {
        pmin = jtx::min(pmin, b.pmin);
        pmax = jtx::max(pmax, b.pmax);
        return *this;
    }

    JTX_DEV bool overlaps(const AABB3 &b) const {
        return pmin.x <= b.pmax.x && pmax.x >= b.pmin.x &&
               pmin.y <= b.pmax.y && pmax.y >= b.pmin.y &&
               pmin.z <= b.pmax.z && pmax.z >= b.pmin.z;
    }

    JTX_DEV bool inside(const Point3<T> &p) const {
        return p.x >= pmin.x && p.x <= pmax.x &&
               p.y >= pmin.y && p.y <= pmax.y &&
               p.z >= pmin.z && p.z <= pmax.z;
    }

    JTX_DEV bool insideExclusive(const Point3<T> &pt) const {
        return pt.x > pmin.x && pt.x < pmax.x &&
               pt.y > pmin.y && pt.y < pmax.y &&
               pt.z > pmin.z && pt.z < pmax.z;
    }

    JTX_DEV AABB3 &expand(T delta) {
        ASSERT(delta >= 0);
        pmin -= delta;
        pmax += delta;
        return *this;
    }

    JTX_DEV AABB3 &shrink(T delta) {
        ASSERT(delta >= 0);
        pmin += delta;
        pmax -= delta;
        return *this;
    }

    JTX_DEV Vec3<T> diagonal() const {
        return pmax - pmin;
    }

    JTX_DEV T surfaceArea() const {
        Vec3<T> d = diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    JTX_DEV T volume() const {
        Vec3<T> d = diagonal();
        return d.x * d.y * d.z;
    }

    [[nodiscard]] JTX_DEV int maxDim() const {
        if (Vec3<T> d = diagonal(); d.x > d.y && d.x > d.z) return 0;
        else if (d.y > d.z)
            return 1;
        return 2;
    }

    [[nodiscard]] JTX_DEV Point3f lerp(const Point3f &t) const {
        return Point3f(jtx::lerp(pmin.x, pmax.x, t.x),
                       jtx::lerp(pmin.y, pmax.y, t.y),
                       jtx::lerp(pmin.z, pmax.z, t.z));
    }

    JTX_DEV Vec3f offset(Point3f &p) const {
        Vec3f o = p - pmin;
        if (pmax.x > pmin.x) o.x /= pmax.x - pmin.x;
        if (pmax.y > pmin.y) o.y /= pmax.y - pmin.y;
        if (pmax.z > pmin.z) o.z /= pmax.z - pmin.z;
        return o;
    }

    JTX_DEV void boundingSphere(Point3<T> *center, T *radius) const {
        *center = (pmin + pmax) / 2;
        // PBRT: when would the center be outside the box?
        *radius = inside(*center) ? distance(*center, pmax) : 0;
    }

    [[nodiscard]] JTX_DEV bool intersectP(const Point3f &o, const Vec3f &d, const float tMax, float *hitT0, float *hitT1) const {
        float t0 = 0;
        float t1 = tMax;
        for (int i = 0; i < 3; ++i) {
            float invDir = 1 / d[i];
            float tNear = (pmin[i] - o[i]) * invDir;
            float tFar = (pmax[i] - o[i]) * invDir;

            if (tNear > tFar) jstd::swap(tNear, tFar);
            // Don't use min/max to avoid NaN issues
            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar < t1 ? tFar : t1;
            if (t0 > t1) return false;
        }

        if (hitT0) *hitT0 = t0;
        if (hitT1) *hitT1 = t1;

        return true;
    }
    //endregion
};

template<typename T>
class AABB2 {
    static_assert(std::is_arithmetic_v<T>, "AABB2 can only be instantiated with arithmetic types");

public:
    Point2<T> pmin, pmax;

    // "empty" = "no volume"
    [[nodiscard]] JTX_DEV bool isEmpty() const { return pmin.x >= pmax.x || pmin.y >= pmax.y; }

    [[nodiscard]] JTX_DEV bool isDegenerate() const { return pmin.x > pmax.x || pmin.y > pmax.y; }

    //region Constructors
    JTX_DEV AABB2() {
#if defined(CUDA_ENABLED) && defined(__CUDA_ARCH__)
        T minval = cuda::std::numeric_limits<T>::lowest();
        T maxval = cuda::std::numeric_limits<T>::max();
#else
        T minval = std::numeric_limits<T>::lowest();
        T maxval = std::numeric_limits<T>::max();
#endif

        pmin = {maxval, maxval};
        pmax = {minval, minval};
    }

    JTX_DEV AABB2(const Point2<T> &a, const Point2<T> &b) {
        pmin = jtx::min(a, b);
        pmax = jtx::max(a, b);
    }

    JTX_DEV explicit AABB2(const Point2<T> &p) : pmin(p), pmax(p) {}

    JTX_DEV AABB2(const AABB2 &other) {
        pmin = Point2<T>(other.pmin);
        pmax = Point2<T>(other.pmax);
    }

    JTX_NUM_ONLY(U)
    JTX_DEV explicit AABB2(const AABB2<U> &other) {
        if (other.isEmpty()) {
            *this = AABB2();
        } else {
            pmin = Point2<T>(other.pmin);
            pmax = Point2<T>(other.pmax);
        }
    }

    JTX_DEV ~AABB2() = default;
    //endregion

    //region Operators
    JTX_DEV bool operator==(const AABB2 &other) const {
        return pmin == other.pmin && pmax == other.pmax;
    }

    JTX_DEV bool operator!=(const AABB2 &other) const {
        return pmin != other.pmin || pmax != other.pmax;
    }
    //endregion

    //region Member functions
    JTX_DEV bool equals(const AABB2 &other, T epsilon) const {
        return pmin.equals(other.pmin, epsilon) && pmax.equals(other.pmax, epsilon);
    }

    // Modified from https://pbr-book.org/4ed/Geometry_and_Transformations/Bounding_Boxes
    [[nodiscard]] JTX_DEV Point2<T> corner(const int i) const {
        ASSERT(i >= 0 && i < 4);
        return Point2<T>(i & 1 ? pmax.x : pmin.x,
                         i & 2 ? pmax.y : pmin.y);
    }

    JTX_DEV AABB2 &merge(const Point2<T> &p) {
        pmin = jtx::min(pmin, p);
        pmax = jtx::max(pmax, p);
        return *this;
    }

    JTX_DEV AABB2 &merge(const AABB2 &b) {
        pmin = jtx::min(pmin, b.pmin);
        pmax = jtx::max(pmax, b.pmax);
        return *this;
    }

    JTX_DEV bool overlaps(const AABB2 &b) const {
        return pmin.x <= b.pmax.x && pmax.x >= b.pmin.x &&
               pmin.y <= b.pmax.y && pmax.y >= b.pmin.y;
    }

    JTX_DEV bool inside(const Point2<T> &p) const {
        return p.x >= pmin.x && p.x <= pmax.x &&
               p.y >= pmin.y && p.y <= pmax.y;
    }

    JTX_DEV bool insideExclusive(const Point2<T> &pt) const {
        return pt.x > pmin.x && pt.x < pmax.x &&
               pt.y > pmin.y && pt.y < pmax.y;
    }

    JTX_DEV AABB2 &expand(T delta) {
        ASSERT(delta >= 0);
        pmin -= delta;
        pmax += delta;
        return *this;
    }

    JTX_DEV AABB2 &shrink(T delta) {
        ASSERT(delta >= 0);
        pmin += delta;
        pmax -= delta;
        return *this;
    }

    JTX_DEV Vec2<T> diagonal() const {
        return pmax - pmin;
    }

    [[nodiscard]] JTX_DEV int maxDim() const {
        if (Vec2<T> d = diagonal(); d.x > d.y) return 0;
        return 1;
    }

    [[nodiscard]] JTX_DEV Point2f lerp(const Point2f &t) const {
        return Point2f(jtx::lerp(pmin.x, pmax.x, t.x),
                       jtx::lerp(pmin.y, pmax.y, t.y));
    }

    JTX_DEV Vec2f offset(Point2f &p) const {
        Vec2f o = p - pmin;
        if (pmax.x > pmin.x) o.x /= pmax.x - pmin.x;
        if (pmax.y > pmin.y) o.y /= pmax.y - pmin.y;
        return o;
    }
    //endregion
};

//region AABB3 functions
JTX_NUM_ONLY_T
JTX_DEV AABB3<T> merge(const AABB3<T> &a, const AABB3<T> &b) {
    AABB3<T> res;
    res.pmin = jtx::min(a.pmin, b.pmin);
    res.pmax = jtx::max(a.pmax, b.pmax);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB3<T> merge(const AABB3<T> &a, const Point3<T> &p) {
    AABB3<T> res;
    res.pmin = jtx::min(a.pmin, p);
    res.pmax = jtx::max(a.pmax, p);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB3<T> intersect(const AABB3<T> &a, const AABB3<T> &b) {
    AABB3<T> res;
    res.pmin = jtx::max(a.pmin, b.pmin);
    res.pmax = jtx::min(a.pmax, b.pmax);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV bool overlaps(const AABB3<T> &a, const AABB3<T> &b) {
    return a.overlaps(b);
}

// Added these because the syntax a.inside(p) is semantically confusing
JTX_NUM_ONLY_T
JTX_DEV bool inside(const Point3<T> &p, const AABB3<T> &a) {
    return a.inside(p);
}

JTX_NUM_ONLY_T
JTX_DEV bool insideExclusive(const Point3<T> &p, const AABB3<T> &a) {
    return a.insideExclusive(p);
}

template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
JTX_DEV auto distanceSqr(const AABB3<T> &b, const Point3<U> &p) {
    using TmU = decltype(T{} - U{});
    TmU dx = jtx::max<TmU>(0, b.pmin.x - p.x, p.x - b.pmax.x);
    TmU dy = jtx::max<TmU>(0, b.pmin.y - p.y, p.y - b.pmax.y);
    TmU dz = jtx::max<TmU>(0, b.pmin.z - p.z, p.z - b.pmax.z);
    return dx * dx + dy * dy + dz * dz;
}

template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
JTX_DEV auto distance(const AABB3<T> &b, const Point3<U> &p) {
    return jtx::sqrt(distanceSqr(b, p));
}

JTX_NUM_ONLY_T
JTX_DEV AABB3<T> expand(const AABB3<T> &a, T delta) {
    ASSERT(delta >= 0);
    AABB3<T> res;
    res.pmin = a.pmin - delta;
    res.pmax = a.pmax + delta;
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB3<T> shrink(const AABB3<T> &a, T delta) {
    ASSERT(delta >= 0);
    AABB3<T> res;
    res.pmin = a.pmin + delta;
    res.pmax = a.pmax - delta;
    return res;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool equals(const AABB3<T> &a, const AABB3<T> &b, T epsilon = EPSILON) {
    return a.equals(b, epsilon);
}
//endregion

//region AABB2 functions
JTX_NUM_ONLY_T
JTX_DEV AABB2<T> merge(const AABB2<T> &a, const AABB2<T> &b) {
    AABB2<T> res;
    res.pmin = jtx::min(a.pmin, b.pmin);
    res.pmax = jtx::max(a.pmax, b.pmax);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB2<T> merge(const AABB2<T> &a, const Point2<T> &p) {
    AABB2<T> res;
    res.pmin = jtx::min(a.pmin, p);
    res.pmax = jtx::max(a.pmax, p);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB2<T> intersect(const AABB2<T> &a, const AABB2<T> &b) {
    AABB2<T> res;
    res.pmin = jtx::max(a.pmin, b.pmin);
    res.pmax = jtx::min(a.pmax, b.pmax);
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV bool overlaps(const AABB2<T> &a, const AABB2<T> &b) {
    return a.overlaps(b);
}

// Added these because the syntax a.inside(p) is semantically confusing
JTX_NUM_ONLY_T
JTX_DEV bool inside(const Point2<T> &p, const AABB2<T> &a) {
    return a.inside(p);
}

JTX_NUM_ONLY_T
JTX_DEV bool insideExclusive(const Point2<T> &p, const AABB2<T> &a) {
    return a.insideExclusive(p);
}

template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
JTX_DEV auto distanceSqr(const AABB2<T> &b, const Point2<U> &p) {
    using TmU = decltype(T{} - U{});
    TmU dx = jtx::max<TmU>(0, b.pmin.x - p.x, p.x - b.pmax.x);
    TmU dy = jtx::max<TmU>(0, b.pmin.y - p.y, p.y - b.pmax.y);
    return dx * dx + dy * dy;
}

template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
JTX_DEV auto distance(const AABB2<T> &b, const Point2<U> &p) {
    return jtx::sqrt(distanceSqr(b, p));
}

JTX_NUM_ONLY_T
JTX_DEV AABB2<T> expand(const AABB2<T> &a, T delta) {
    ASSERT(delta >= 0);
    AABB2<T> res;
    res.pmin = a.pmin - delta;
    res.pmax = a.pmax + delta;
    return res;
}

JTX_NUM_ONLY_T
JTX_DEV AABB2<T> shrink(const AABB2<T> &a, T delta) {
    ASSERT(delta >= 0);
    AABB2<T> res;
    res.pmin = a.pmin + delta;
    res.pmax = a.pmax - delta;
    return res;
}

JTX_NUM_ONLY_T
JTX_HOSTDEV bool equals(const AABB2<T> &a, const AABB2<T> &b, T epsilon = EPSILON) {
    return a.equals(b, epsilon);
}
//endregion

typedef AABB3<int> BBox3i;
typedef AABB3<float> BBox3f;

typedef AABB2<int> BBox2i;
typedef AABB2<float> BBox2f;
}// namespace jtx
