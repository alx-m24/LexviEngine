#include "pch.h"

#include "Renderable/Model/Mesh/Mesh.hpp"

namespace Lexvi {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        : vertices(vertices), indices(indices), textures(textures)
    {
        setupMesh();
    }

    void Mesh::Draw(const Shader* shader) const
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int roughNr = 1;
        unsigned int metallicNr = 1;
        unsigned int aoNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++) {
            glBindTextureUnit(i, textures[i].id);

            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_diffuse")      number = std::to_string(diffuseNr++);
            else if (name == "texture_specular") number = std::to_string(specularNr++);
            else if (name == "texture_normal")   number = std::to_string(normalNr++);
            else if (name == "texture_roughness") number = std::to_string(roughNr++);
            else if (name == "texture_metallic")  number = std::to_string(metallicNr++);
            else if (name == "texture_ao")        number = std::to_string(aoNr++);

            shader->setInt("material." + (name + number), i);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }

    void Mesh::setupMesh() {
        GLuint VAO, VBO, EBO;
        glCreateVertexArrays(1, &VAO);
        glCreateBuffers(1, &VBO);
        glCreateBuffers(1, &EBO);

        // Upload vertex data
        glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Upload index data
        glNamedBufferData(EBO, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Bind buffers to VAO
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(VAO, EBO);

        // Enable and describe vertex attributes
        // Position
        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        glVertexArrayAttribBinding(VAO, 0, 0);

        // Normal
        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
        glVertexArrayAttribBinding(VAO, 1, 0);

        // TexCoords
        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
        glVertexArrayAttribBinding(VAO, 2, 0);

        // Tangent
        glEnableVertexArrayAttrib(VAO, 3);
        glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Tangent));
        glVertexArrayAttribBinding(VAO, 3, 0);

        // Bitangent
        glEnableVertexArrayAttrib(VAO, 4);
        glVertexArrayAttribFormat(VAO, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Bitangent));
        glVertexArrayAttribBinding(VAO, 4, 0);

    }
}