#pragma once

#include "Renderable/IRenderable/IRenderable.hpp"

namespace Lexvi {

    struct SemiCircleVertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct SemiCircleMesh {
        std::vector<SemiCircleVertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO = 0, VBO = 0, EBO = 0;
    };

    // Generates a semicircle mesh in XY plane, z=0, with "vertexCount" along rim
    void GenerateSemiCircleMesh(SemiCircleMesh& mesh, uint32_t vertexCount);

    void DrawSemiCircleMesh(SemiCircleMesh& mesh, Shader& shader, uint32_t instanceCount = 1);
}
