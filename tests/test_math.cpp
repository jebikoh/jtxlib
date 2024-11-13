#include <jtxlib/math/math.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "./tconstants.h"

//region BB3f Constructors
TEST_CASE("Pow gives same result as std::pow", "[Math]") {
    float ref = std::pow(2.5f, 9.0f);
    float result = jtx::pow(2.5f, 9.0f);

    REQUIRE_THAT(result, Catch::Matchers::WithinRel(ref, T_EPS));
}

TEST_CASE("Fast exp gives sames result as std::exp", "[Math]") {
    float ref = std::exp(2.5f);
    float result = jtx::fastExp(2.5f);

    REQUIRE_THAT(result, Catch::Matchers::WithinRel(ref, T_EPS));
}