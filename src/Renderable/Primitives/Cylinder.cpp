#include "pch.h"

#include "Renderable/Primitives/Cylinder.hpp"

namespace Lexvi {
    glm::vec3 EvalCurve(float theta, float y, float r) {
        return glm::vec3(r * cos(theta), y, r * sin(theta));
    }

    glm::vec3 ComputeNormal(float theta, float y, float r, float dTheta, float dY) {
        glm::vec3 p = EvalCurve(theta, y, r);
        glm::vec3 pt = EvalCurve(theta + dTheta, y, r);
        glm::vec3 py = EvalCurve(theta, y + dY, r);

        glm::vec3 t1 = pt - p;  // tangent in theta direction
        glm::vec3 t2 = py - p;  // tangent in y direction

        return glm::normalize(glm::cross(t1, t2));
    }

    void GenerateCylinderMesh(CylinderMesh& mesh, float radius, float height, int segments) {
        mesh.vertices.clear();
        mesh.indices.clear();

        // Small steps for finite differences
        float dTheta = 0.001f;
        float dY = 0.001f;

        for (int i = 0; i <= segments; i++) {
            float theta = (2.0f * std::numbers::pi_v<float> *i) / segments;
            float x = cos(theta) * radius;
            float z = sin(theta) * radius;

            glm::vec3 normal = ComputeNormal(theta, 0.0f, radius, dTheta, dY);

            // bottom
            mesh.vertices.push_back({ glm::vec3(x, 0.0f, z), normal });

            normal = ComputeNormal(theta, height, radius, dTheta, dY);
            // top
            mesh.vertices.push_back({ glm::vec3(x, height, z), normal });
        }

        for (int i = 0; i < segments; i++) {
            int base = i * 2;
            mesh.indices.push_back(base);
            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 2);

            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 3);
            mesh.indices.push_back(base + 2);
        }

        if (mesh.VAO != 0) {
            glDeleteVertexArrays(1, &mesh.VAO);
            glDeleteBuffers(1, &mesh.VBO);
            glDeleteBuffers(1, &mesh.EBO);
        }

        glCreateVertexArrays(1, &mesh.VAO);
        glCreateBuffers(1, &mesh.VBO);
        glCreateBuffers(1, &mesh.EBO);

        glNamedBufferData(mesh.VBO, mesh.vertices.size() * sizeof(CylinderVertex), mesh.vertices.data(), GL_STATIC_DRAW);

        glNamedBufferData(mesh.EBO, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(mesh.VAO, 0, mesh.VBO, 0, sizeof(CylinderVertex));
        glVertexArrayElementBuffer(mesh.VAO, mesh.EBO);

        glEnableVertexArrayAttrib(mesh.VAO, 0);
        glEnableVertexArrayAttrib(mesh.VAO, 1);

        glVertexArrayAttribFormat(mesh.VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(CylinderVertex, position));
        glVertexArrayAttribFormat(mesh.VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(CylinderVertex, normal));

        glVertexArrayAttribBinding(mesh.VAO, 0, 0);
        glVertexArrayAttribBinding(mesh.VAO, 1, 0);
    }

    Cylinder::Cylinder(float radius, float height, int segments) : radius(radius), height(height), segments(segments)
    {
        GenerateCylinderMesh(cylinderMesh, radius, height, segments);
    }

    void Cylinder::Draw(const Shader* shader)
    {
        shader->use();
        glBindVertexArray(cylinderMesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cylinderMesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    void Cylinder::updateBoundingBox()
    {
    }

    void Cylinder::setTransforms(const glm::mat4& mat)
    {
    }

    glm::mat4 Cylinder::getTransforms() const
    {
        return glm::mat4();
    }

    CameraAABB Cylinder::getBoundBox() const
    {
        return CameraAABB();
    }

    bool Cylinder::isVisible(const Camera& camera) const
    {
        std::cout << "implement culling for cylinder";
        return true;
    }
}