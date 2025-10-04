#include "pch.h"

#include "Renderable/Primitives/Plane.hpp"

namespace Lexvi {
    void SetupPlaneBuffers(PlaneMesh& plane) {
        if (plane.VAO != 0) {
            glDeleteVertexArrays(1, &plane.VAO);
            glDeleteBuffers(1, &plane.VBO);
            glDeleteBuffers(1, &plane.EBO);
        }

        glCreateVertexArrays(1, &plane.VAO);
        glCreateBuffers(1, &plane.VBO);
        glCreateBuffers(1, &plane.EBO);

        glNamedBufferData(plane.VBO, sizeof(PlaneVertex) * plane.vertices.size(), plane.vertices.data(), GL_STATIC_DRAW);

        glNamedBufferData(plane.EBO, sizeof(GLuint) * plane.indices.size(), plane.indices.data(), GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(plane.VAO, 0, plane.VBO, 0, sizeof(PlaneVertex));
        glVertexArrayElementBuffer(plane.VAO, plane.EBO);

        glEnableVertexArrayAttrib(plane.VAO, 0); // Position
        glEnableVertexArrayAttrib(plane.VAO, 1); // TexCoords
        glEnableVertexArrayAttrib(plane.VAO, 2); // Normals

        glVertexArrayAttribFormat(plane.VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(PlaneVertex, position));
        glVertexArrayAttribFormat(plane.VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(PlaneVertex, texCoords));
        glVertexArrayAttribFormat(plane.VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(PlaneVertex, normal));

        glVertexArrayAttribBinding(plane.VAO, 0, 0);
        glVertexArrayAttribBinding(plane.VAO, 1, 0);
        glVertexArrayAttribBinding(plane.VAO, 2, 0);
    }

    void GeneratePlane(PlaneMesh& plane, int gridSizeX, int gridSizeZ, float spacing) {
        plane.vertices.clear();
        plane.indices.clear();

        plane.gridSizeX = gridSizeX;
        plane.gridSizeZ = gridSizeZ;

        float halfWidth = gridSizeX * spacing * 0.5f;
        float halfDepth = gridSizeZ * spacing * 0.5f;

        // Generate vertices
        for (int z = 0; z <= gridSizeZ; z++) {
            for (int x = 0; x <= gridSizeX; x++) {
                PlaneVertex v;

                // Centered on origin
                v.position = glm::vec3(x * spacing - halfWidth, 0.0f, z * spacing - halfDepth);

                // Normalized texcoords [0,1]
                v.texCoords = glm::vec2(
                    static_cast<float>(x) / gridSizeX,
                    static_cast<float>(z) / gridSizeZ
                );

                // Flat upward normal
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f);

                plane.vertices.push_back(v);
            }
        }

        int meshWidth = gridSizeX + 1;

        // Indices
        for (int z = 0; z < gridSizeZ; z++) {
            for (int x = 0; x < gridSizeX; x++) {
                int i0 = z * meshWidth + x;
                int i1 = i0 + 1;
                int i2 = i0 + meshWidth;
                int i3 = i2 + 1;

                plane.indices.push_back(i0);
                plane.indices.push_back(i2);
                plane.indices.push_back(i1);

                plane.indices.push_back(i1);
                plane.indices.push_back(i2);
                plane.indices.push_back(i3);
            }
        }

        SetupPlaneBuffers(plane);
    }

    Plane::Plane(int gridSizeX, int gridSizeZ, float spacing) : size(glm::vec3(gridSizeX, 1.0f, gridSizeZ)), position(0.0f)
    {
        GeneratePlane(planeMesh, gridSizeX, gridSizeZ, spacing);
    }

    void Plane::Draw(const Shader* shader)
    {
        shader->use();
        glBindVertexArray(planeMesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(planeMesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    void Plane::setTransforms(const glm::mat4& mat)
    {
    }

    glm::mat4 Plane::getTransforms() const
    {
        return glm::mat4(transforms);
    }

    CameraAABB Plane::getBoundBox() const
    {
        CameraAABB aabb{};

        aabb.min = position - size / 2.0f;
        aabb.max = position + size / 2.0f;

        return aabb;
    }

    bool Plane::isVisible(const Camera& camera) const
    {
        //return isInFrustum(camera.getFrustum(), getBoundBox());
        return true;
    }

    void Plane::setPosition(glm::vec3 position)
    {
        this->position = position;
    }

    void PlaneMesh::Bind() const
    {
        glBindVertexArray(VAO);
    }

    void Plane::Bind() const {
        planeMesh.Bind();
    }
}