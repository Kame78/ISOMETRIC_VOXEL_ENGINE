#pragma once

#include <cstdint>

namespace Math {

    class NoiseMath{
    public:

        NoiseMath() = delete;
    
        [[nodiscard]] static float CalculateBoundaryNoise(float x, float z, float centerX, float centerZ, float maxRadius, uint32_t seed) noexcept;

        [[nodiscard]] static float CalculateHeightNoise(float x, float z, float centerX, float centerZ, float maxRadius, uint32_t seed) noexcept;

        [[nodiscard]] static float CalculateRockDensity(float x, float z, uint32_t seed) noexcept;

        [[nodiscard]] static int QuantizeHeight(float rawNoise, int StepInterval) noexcept;
    };
}