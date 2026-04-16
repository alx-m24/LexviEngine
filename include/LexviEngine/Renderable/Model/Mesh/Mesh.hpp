#pragma once

#include <Shader/Shader.hpp>
#include <Textures/Textures.hpp>
#include <Renderable/IRenderable/IRenderable.hpp>

namespace Lexvi {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(const Shader* shader) const;

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
    };

}