#pragma once

#include <random>

namespace Lexvi {
    class RandomUtils {
    public:
        RandomUtils(unsigned int seed = std::random_device{}())
            : rng(seed) {
        }

        // Random float in [min, max)
        float Float(float min, float max);

        uint32_t UInt_32(uint32_t min, uint32_t max);

        // Random angle in degrees [0, 360)
        float Angle();

        // Random angle in a specified range
        float Angle(float minDeg, float maxDeg);

        // Random unit vector in 3D (on the unit sphere)
        glm::vec3 UnitVector3D();

    private:
        std::mt19937 rng;
    };
}
