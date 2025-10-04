#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Renderable/IRenderable/IRenderable.hpp>

namespace Lexvi {

    struct QuadVertex {
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
    };

    struct QuadMesh {
        std::vector<QuadVertex> vertices;
        std::vector<uint32_t> indices;
        unsigned int VAO = 0, VBO = 0, EBO = 0;

        void Bind() const;
    };

    void SetupQuadBuffers(QuadMesh& quad);
    void GenerateQuad(QuadMesh& quad, float width = 1.0f, float height = 1.0f);

    class Quad : public IRenderable {
    private:
        QuadMesh quadMesh;

    protected:
        glm::vec2 size = glm::vec2(1.0f, 1.0f);
        glm::vec3 position = glm::vec3(0.0f);

    public:
        Quad() = default;
        Quad(float width, float height);

        void Draw(const Shader* shader) override;

        void setTransforms(const glm::mat4& mat) override;
        glm::mat4 getTransforms() const override;
        CameraAABB getBoundBox() const override;
        bool isVisible(const Camera& camera) const override;

        void setPosition(const glm::vec3& pos);

        void Bind() const;
    };

} // namespace Lexvi
