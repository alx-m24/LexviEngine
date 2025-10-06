#include "pch.h"

#include "Renderable/Primitives/SemiCircle.hpp"

namespace Lexvi {
    // Generates a semicircle mesh in XY plane, z=0, with "vertexCount" along rim
    void GenerateSemiCircleMesh(SemiCircleMesh& mesh, uint32_t vertexCount) {
        mesh.vertices.clear();
        mesh.indices.clear();

        SemiCircleVertex center{};
        center.position = glm::vec3(0.0f, 0.0f, 0.0f);
        center.normal = glm::vec3(0.0f, 0.0f, 1.0f);
        mesh.vertices.push_back(center);

        // Ensure vertex count is odd for symmetry
        if (vertexCount % 2 == 0) vertexCount += 1;

        // Rim vertices along semicircle
        for (uint32_t i = 0; i <= vertexCount; ++i) {
            float t = (float)i / vertexCount;                             // 0 -> 1
            float angle = -glm::half_pi<float>() + t * glm::pi<float>();  // -pi/2 -> +pi/2
            float x = cos(angle);
            float y = sin(angle);

            SemiCircleVertex v{};
            v.position = glm::vec3(x, y, 0.0f);
            v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            mesh.vertices.push_back(v);
        }

        // Convert fan to triangle list
        for (uint32_t i = 1; i <= vertexCount; ++i) {
            mesh.indices.push_back(0);   // center
            mesh.indices.push_back(i);
            mesh.indices.push_back(i + 1);
        }

        // Delete old buffers if they exist
        if (mesh.VAO != 0) {
            glDeleteVertexArrays(1, &mesh.VAO);
            glDeleteBuffers(1, &mesh.VBO);
            glDeleteBuffers(1, &mesh.EBO);
        }

        glCreateVertexArrays(1, &mesh.VAO);
        glCreateBuffers(1, &mesh.VBO);
        glCreateBuffers(1, &mesh.EBO);

        glNamedBufferData(mesh.VBO, mesh.vertices.size() * sizeof(SemiCircleVertex), mesh.vertices.data(), GL_STATIC_DRAW);
        glNamedBufferData(mesh.EBO, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(mesh.VAO, 0, mesh.VBO, 0, sizeof(SemiCircleVertex));
        glVertexArrayElementBuffer(mesh.VAO, mesh.EBO);

        glEnableVertexArrayAttrib(mesh.VAO, 0);
        glEnableVertexArrayAttrib(mesh.VAO, 1);

        glVertexArrayAttribFormat(mesh.VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(SemiCircleVertex, position));
        glVertexArrayAttribFormat(mesh.VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(SemiCircleVertex, normal));

        glVertexArrayAttribBinding(mesh.VAO, 0, 0);
        glVertexArrayAttribBinding(mesh.VAO, 1, 0);
    }

    void DrawSemiCircleMesh(SemiCircleMesh& mesh, Shader& shader, uint32_t instanceCount) {
        shader.use();
        glBindVertexArray(mesh.VAO);

        if (instanceCount > 1) {
            glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instanceCount));
        }
        else {
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
        }
    }

}
