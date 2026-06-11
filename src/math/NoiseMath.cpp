#include "../../include/math/NoiseMath.hpp"
#include <cmath>
#include <algorithm>
#include <cstdint> // Required for clean uint32_t definitions

// === FIXED: Uses uint32_t to eliminate Undefined Behavior from Signed Overflow ===
static float Hash2D(int x, int z) noexcept {
    // Cast to unsigned immediately to enforce deterministic modulo wrapping
    uint32_t ux = static_cast<uint32_t>(x);
    uint32_t uz = static_cast<uint32_t>(z);

    uint32_t n = ux + uz * 57u;
    n = (n << 13) ^ n;
    
    // Perform the chaotic mixing math safely using unsigned attributes
    uint32_t hashed = (n * (n * n * 15731u + 789221u) + 1376312589u) & 0x7fffffffu;
    
    // Scale cleanly back into a symmetric floating point range of [-1.0f, 1.0f]
    return 1.0f - (static_cast<float>(hashed) / 1073741824.0f);
}

static float SmoothNoise2D(float x, float z) noexcept {
    int intX = static_cast<int>(std::floor(x));
    int intZ = static_cast<int>(std::floor(z));

    float fracX = x - static_cast<float>(intX);
    float fracZ = z - static_cast<float>(intZ);

    float u = fracX * fracX * fracX * (fracX * (fracX * 6.0f - 15.0f) + 10.0f);
    float v = fracZ * fracZ * fracZ * (fracZ * (fracZ * 6.0f - 15.0f) + 10.0f);

    float v1 = Hash2D(intX, intZ);
    float v2 = Hash2D(intX + 1, intZ);
    float v3 = Hash2D(intX, intZ + 1);
    float v4 = Hash2D(intX + 1, intZ + 1);

    float i1 = std::lerp(v1, v2, u);
    float i2 = std::lerp(v3, v4, u);

    return std::lerp(i1, i2, v);
}

static float FractalNoise2D(float x, float z, int octaves, float baseFrequency, float persistence) noexcept {
    float total = 0.0f;
    float frequency = baseFrequency; 
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += SmoothNoise2D(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;  
        frequency *= 2.0f;
    }
    return total / maxValue;
}

namespace Math {

float NoiseMath::CalculateBoundaryNoise(float x, float z, float centerX, float centerZ, float maxRadius, uint32_t seed) noexcept {
    float seedOffset = static_cast<float>(seed * 17);
    float rawNoise = FractalNoise2D(x + 1200.0f + seedOffset, z + 1200.0f + seedOffset, 3, 0.014f, 0.42f);

    float dx = x - centerX;
    float dz = z - centerZ;
    float distance = std::sqrt(dx * dx + dz * dz);
    float normalizedDist = distance / maxRadius;

    float islandShape = 1.0f - (normalizedDist * normalizedDist * 1.1f);
    float boundaryMask = islandShape + (rawNoise * 0.20f);

    return boundaryMask;
}

float NoiseMath::CalculateHeightNoise(float x, float z, float centerX, float centerZ, float maxRadius, uint32_t seed) noexcept {
    float seedOffset = static_cast<float>(seed * 17);
    float rawNoise = FractalNoise2D(x + seedOffset, z + seedOffset, 4, 0.012f, 0.35f);    

    float warpedX = x + (rawNoise * 6.0f);
    float warpedZ = z + (rawNoise * 6.0f);

    float dx = warpedX - centerX;
    float dz = warpedZ - centerZ;

    float distance = std::sqrt(dx * dx + dz * dz);
    float normalizedDist = distance / maxRadius;

    float boundaryNoise= 1.0f - (normalizedDist * normalizedDist) + (rawNoise * 0.35f);
    
    float heightScale = (boundaryNoise - 0.2f) / 0.8f;
    heightScale = std::clamp(heightScale, 0.0f, 1.0f);

    float curvedBias = std::pow(heightScale, 5.0f);
    return curvedBias * (0.65f + (rawNoise * 1.0f) * 0.5f * 0.35f);
}

float NoiseMath::CalculateRockDensity(float x, float z, uint32_t seed) noexcept {
    float seedOffset = static_cast<float>(seed * 73);
    return FractalNoise2D(x - 2400.0f + seedOffset, z - 2400.0f + seedOffset, 2, 0.012f, 0.5f);
}

int NoiseMath::QuantizeHeight(float rawNoise, int stepInterval, int maxHeight) noexcept {
    float positiveSignal = rawNoise * static_cast<float>(maxHeight);

    int continuousVoxelY = std::clamp(static_cast<int>(positiveSignal), 0, maxHeight - 1);

    return (continuousVoxelY / stepInterval) * stepInterval;
}

} // namespace Math