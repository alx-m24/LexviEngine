#include "pch.h"
#include "Renderable/Primitives/Quad.hpp"

namespace Lexvi {

    void SetupQuadBuffers(QuadMesh& quad) {
        if (quad.VAO != 0) {
            glDeleteVertexArrays(1, &quad.VAO);
            glDeleteBuffers(1, &quad.VBO);
            glDeleteBuffers(1, &quad.EBO);
        }

        glCreateVertexArrays(1, &quad.VAO);
        glCreateBuffers(1, &quad.VBO);
        glCreateBuffers(1, &quad.EBO);

        glNamedBufferData(quad.VBO, sizeof(QuadVertex) * quad.vertices.size(), quad.vertices.data(), GL_STATIC_DRAW);
        glNamedBufferData(quad.EBO, sizeof(GLuint) * quad.indices.size(), quad.indices.data(), GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(quad.VAO, 0, quad.VBO, 0, sizeof(QuadVertex));
        glVertexArrayElementBuffer(quad.VAO, quad.EBO);

        glEnableVertexArrayAttrib(quad.VAO, 0); // Position
        glEnableVertexArrayAttrib(quad.VAO, 1); // TexCoords
        glEnableVertexArrayAttrib(quad.VAO, 2); // Normals

        glVertexArrayAttribFormat(quad.VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, position));
        glVertexArrayAttribFormat(quad.VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, texCoords));
        glVertexArrayAttribFormat(quad.VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, normal));

        glVertexArrayAttribBinding(quad.VAO, 0, 0);
        glVertexArrayAttribBinding(quad.VAO, 1, 0);
        glVertexArrayAttribBinding(quad.VAO, 2, 0);
    }

    void GenerateQuad(QuadMesh& quad, float width, float height) {
        quad.vertices.clear();
        quad.indices.clear();

        quad.vertices = {
            {{-width, -height, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{ width, -height, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{ width,  height, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-width,  height, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
        };

        quad.indices = { 0, 1, 2, 2, 3, 0 };

        SetupQuadBuffers(quad);
    }

    Quad::Quad(float width, float height) : size(width, height), position(0.0f) {
        GenerateQuad(quadMesh, width, height);
    }

    void Quad::Draw(const Shader* shader) {
        shader->use();
        glBindVertexArray(quadMesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(quadMesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    void Quad::setTransforms(const glm::mat4& mat) {
        transforms = mat;
    }

    glm::mat4 Quad::getTransforms() const {
        return glm::mat4(transforms);
    }

    CameraAABB Quad::getBoundBox() const {
        CameraAABB aabb{};
        aabb.min = glm::vec3(position.x - size.x / 2.0f, position.y - 0.0f, position.z - size.y / 2.0f);
        aabb.max = glm::vec3(position.x + size.x / 2.0f, position.y + 0.0f, position.z + size.y / 2.0f);
        return aabb;
    }

    bool Quad::isVisible(const Camera& camera) const {
        return isInFrustum(camera.getFrustum(), getBoundBox());
    }

    void Quad::setPosition(const glm::vec3& pos) {
        position = pos;
    }

    void QuadMesh::Bind() const {
        glBindVertexArray(VAO);
    }

    void Quad::Bind() const {
        quadMesh.Bind();
    }

} // namespace Lexvi
