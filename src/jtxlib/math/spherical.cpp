#include <jtxlib/math/spherical.hpp>

namespace jtx{
    Vec3f EqualAreaSquareToSphere(const Point2f &p) {
        ASSERT(p.x >= 0 && p.x <= 1 && p.y >= 0 && p.y <= 1);
        float u = 2 * p.x - 1;
        float v = 2 * p.y - 1;
        float u_p = std::abs(u);
        float v_p = std::abs(v);

        float signedDist = 1 - (u_p + v_p);
        float r = 1 - std::abs(signedDist);

        float phi = (r == 0 ? 1 : (v_p - u_p) / r + 1) * JTX_PI_F / 4;
        return {jtx::CopySign(std::cos(phi), u) * r * jtx::SafeSqrt(2 - r * r),
                jtx::CopySign(std::sin(phi), v) * r * jtx::SafeSqrt(2 - r * r),
                jtx::CopySign(1 - r * r, signedDist)};
    }

    Point2f EqualAreaSphereToSquare(const Point3f &d) {
        ASSERT(d.lenSqr() < 1.001f && d.lenSqr() > 0.999f);
        float x = jtx::Abs(d.x);
        float y = jtx::Abs(d.y);
        float z = jtx::Abs(d.z);

        float r = jtx::SafeSqrt(1 - z);
        float a = jtx::Max(x, y);
        float b = jtx::min(x, y);
        b = a == 0 ? 0 : b / a;

        // https://github.com/mmp/pbrt-v4/blob/39e01e61f8de07b99859df04b271a02a53d9aeb2/src/pbrt/util/math.cpp#L292
        const float t1 = 0.406758566246788489601959989e-5;
        const float t2 = 0.636226545274016134946890922156;
        const float t3 = 0.61572017898280213493197203466e-2;
        const float t4 = -0.247333733281268944196501420480;
        const float t5 = 0.881770664775316294736387951347e-1;
        const float t6 = 0.419038818029165735901852432784e-1;
        const float t7 = -0.251390972343483509333252996350e-1;
        float phi = jtx::evalPolynomial(b, t1, t2, t3, t4, t5, t6, t7);

        if (x < y) phi = 1 - phi;
        float v = phi * r;
        float u = r - v;

        if (d.z < 0) {
            u = 1 - u;
            v = 1 - v;
        }

        u = jtx::CopySign(u, d.x);
        v = jtx::CopySign(v, d.y);

        return {0.5f * (u+1), 0.5f * (v+1)};
    }
}

