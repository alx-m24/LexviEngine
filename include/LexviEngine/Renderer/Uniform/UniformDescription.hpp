#pragma once

#include "LexviEngine/Renderer/Shader/Shader.hpp"

struct UniformDescription {
    uint32_t binding = 0;
    uint32_t DescriptorCount = 1;
    ShaderStageType shaderStage = ShaderStageType::VERTEX;
};
