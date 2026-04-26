#pragma once

#include "Renderable/IRenderable/IRenderable.hpp"

namespace Lexvi {

    struct CircleVertex {
        glm::vec2 position;
    };

    struct CircleMesh {
        std::vector<CircleVertex> vertices;
        std::vector<uint16_t> indices;
        unsigned int VAO = 0, VBO = 0, EBO = 0;

        void Bind() const;
    };

    void GenerateUnitCircle(CircleMesh& mesh, uint32_t vertexCount);

    class Circle : public IRenderable {
    private:
        CircleMesh circleMesh;

    protected:
        float radius = 1.0f;
        glm::vec2 position = glm::vec3(0.0f);

    public:
        Circle() = default;
        Circle(float radius);

        void Draw(const Shader* shader) override;

        void setTransforms(const glm::mat4& mat) override;
        glm::mat4 getTransforms() const override;
        AABB getBoundBox() const override;
        bool isVisible(const Camera& camera) const override;

        void setPosition(const glm::vec3& pos);

        void Bind() const;

    public:
        bool Collides(const glm::vec2& point) const;
        bool Collides(const Circle& other) const;
    };
}
