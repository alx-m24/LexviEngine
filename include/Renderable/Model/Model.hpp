#pragma once

#include "Mesh/Mesh.hpp"
#include "Shader/Shader.hpp"
#include <Renderable/IRenderable/IRenderable.hpp>

namespace Lexvi {
    class Model : public IRenderable {
    public:
        Model(const std::string& path) { loadModel(path); }
        void Draw(const Shader* shader) override;

    private:
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}