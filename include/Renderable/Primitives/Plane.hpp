#pragma once

#include <Renderable/IRenderable/IRenderable.hpp>

namespace Lexvi {
    struct PlaneVertex {
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
    };

    struct PlaneMesh {
        std::vector<PlaneVertex> vertices;
        std::vector<uint32_t> indices;
        unsigned int VAO = 0, VBO = 0, EBO = 0;
        int gridSizeX = 0;
        int gridSizeZ = 0;

        void Bind() const;
    };

    void SetupPlaneBuffers(PlaneMesh& plane);
    void GeneratePlane(PlaneMesh& plane, int gridSizeX, int gridSizeZ, float spacing = 1.0f);

    class Plane : public IRenderable {
    private:
        PlaneMesh planeMesh;

    protected:
        glm::vec3 size = glm::vec3(1.0f);
        glm::vec3 position = glm::vec3(0.0f);

    public:
        Plane() = default;
        Plane(int gridSizeX, int gridSizeZ, float spacing = 1.0f);

        void Draw(const Shader* shader) override;

        void setTransforms(const glm::mat4& mat) override;
        glm::mat4 getTransforms() const override;
        CameraAABB getBoundBox() const override;
        bool isVisible(const Camera& camera) const override;

        void setPosition(glm::vec3 position);

        void Bind() const;
    };
}