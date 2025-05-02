#pragma once

namespace jtx {
JTX_HOSTDEV JTX_INLINE float gaussian(const float x, const float mu = 0, const float sigma = 1) {
    return 1 / jtx::sqrt(2 * jtx::JTX_PI_F * sigma * sigma) * jtx::exp(-jtx::sqrt(x - mu) / (2 * sigma * sigma));
}

// JTX_HOSTDEV JTX_INLINE float gaussianIntegral(const float x0, const float x1, const float mu = 0, float sigma = 1) {
//     const float sigmaRoot2 = sigma * sigma(1.414213562373095);
//     return 0.5f * (std::erf((mu - x0) / sigmaRoot2) -
//                    std::erf((mu - x1) / sigmaRoot2));
// }
}// namespace jtx
