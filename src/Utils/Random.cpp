#include "pch.h"

#include "Utils/Random.hpp"

namespace Lexvi {
    float RandomUtils::Float(float min, float max){
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

    uint32_t RandomUtils::UInt_32(uint32_t min, uint32_t max) {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(rng);
    }

    float RandomUtils::Angle() {
        return Float(0.0f, 360.0f);
    }

    float RandomUtils::Angle(float minDeg, float maxDeg) {
        return Float(minDeg, maxDeg);
    }

    glm::vec3 RandomUtils::UnitVector3D() {
        float theta = Angle(0.0f, 2.0f * glm::pi<float>()); // azimuth
        float phi = Float(0.0f, glm::pi<float>());        // inclination
        float x = sin(phi) * cos(theta);
        float y = cos(phi);
        float z = sin(phi) * sin(theta);
        return glm::vec3(x, y, z);
    }
}