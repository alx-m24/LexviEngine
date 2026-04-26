#include "LexviEngine/Renderable/Primitives/2D/Circle.hpp"

namespace Lexvi {
    void GenerateUnitCircle(CircleMesh& mesh, uint32_t vertexCount) {
        mesh.vertices.clear();
        mesh.vertices.reserve(vertexCount + 1);

        float theta_increment = glm::pi<float>() * 2.0f / static_cast<float>(vertexCount);

        mesh.vertices.push_back(CircleVertex { glm::vec2(0.0f) });
        for (uint32_t i = 0; i < vertexCount; ++i) {
            float theta = i * theta_increment;
            glm::vec2 vertexPosition = { glm::cos(theta), glm::sin(theta) };
            mesh.vertices.push_back(CircleVertex{ .position = vertexPosition });
        }

        mesh.indices.clear();
        mesh.indices.reserve(vertexCount * 3);
        
        for (uint32_t i = 1; i <= vertexCount; ++i) {
            uint32_t next = (i % vertexCount) + 1;
        
            mesh.indices.push_back(0);     // center
            mesh.indices.push_back(i);     // current
            mesh.indices.push_back(next);  // next (wraps around)
        }
    }

    Circle::Circle(float radius) {
        GenerateUnitCircle(circleMesh, 10);
        this->transforms = glm::scale(this->transforms, glm::vec3(radius));
    }

    void Circle::Draw(const Shader* shader) {
		SmartBind shaderBind(*shader);

        glBindVertexArray(circleMesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleMesh.indices.size()), GL_UNSIGNED_SHORT, nullptr);
    }

    void Circle::setTransforms(const glm::mat4& mat) {
        transforms = mat;
    }

    glm::mat4 Circle::getTransforms() const {
        return glm::mat4(transforms);
    }

    AABB Circle::getBoundBox() const {
        AABB aabb{};
        aabb.min = glm::vec3(position.x - radius / 2.0f, position.y - 0.0f, 0.0f );
        aabb.max = glm::vec3(position.x + radius / 2.0f, position.y + 0.0f, 0.0f );
        return aabb;
    }

    bool Circle::isVisible(const Camera& camera) const {
        return isInFrustum(camera.getFrustum(), getBoundBox());
    }

    void Circle::setPosition(const glm::vec3& pos) {
        position = pos;
    }

    void CircleMesh::Bind() const {
        glBindVertexArray(VAO);
    }

    void Circle::Bind() const {
        circleMesh.Bind();
    }

    bool Circle::Collides(const glm::vec2& point) const {
        return glm::distance(point, this->position) <= radius;
    }

    bool Circle::Collides(const Circle& other) const {
        return glm::distance(other.position, this->position) <= this->radius + other.radius;
    }
}
