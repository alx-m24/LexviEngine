#pragma once

#include "Renderable/IRenderable/IRenderable.hpp"

namespace Lexvi {
    struct CylinderVertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct CylinderMesh {
        std::vector<CylinderVertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO = 0, VBO = 0, EBO = 0;
    };

    glm::vec3 EvalCurve(float theta, float y, float r);
    glm::vec3 ComputeNormal(float theta, float y, float r, float dTheta, float dY);
    void GenerateCylinderMesh(CylinderMesh& mesh, float radius, float height, int segments);

    class Cylinder : public IRenderable {
    private:
        CylinderMesh cylinderMesh;

    protected:
        float radius = 1.0f;
        float height = 1.0f;
        int segments = 1;

    public:
        Cylinder(float radius, float height, int segments);

        void Draw(const Shader* shader) override;

        void updateBoundingBox() override;

        void setTransforms(const glm::mat4& mat) override;
        glm::mat4 getTransforms() const override;
        CameraAABB getBoundBox() const override;
        bool isVisible(const Camera& camera) const override;
    };
}