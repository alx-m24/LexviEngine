#pragma once

#include <string>
#include <vector>

struct aiTexel;

namespace Lexvi {
    struct Texture {
        unsigned int id = 0;
        std::string type;
        std::string path;
        unsigned int format = 0;
        unsigned int internalFormat = 0;
    };

    Texture loadTextureArray(const std::vector<std::string>& paths);
    Texture loadTexture(const std::string& path);
    Texture TextureFromFile(const char* path, const std::string& directory);
    Texture TextureFromMemory(const unsigned char* data, size_t size);
    Texture TextureFromRawPixels(aiTexel* pixels, int width, int height);
    Texture GenerateDepthTexture(int width, int height);

    Texture Create3DComputeTexture(unsigned int layers, unsigned int width, unsigned int height, unsigned int channelNum);
    Texture CreateComputeTexture(unsigned int width, unsigned int height, unsigned int channelNum);

    std::vector<unsigned char> getRawTexturePixels(std::string path);
    std::vector<unsigned char> getGreyScaleTexturePixels(std::string path);

    void GenerateMipMaps(Texture& tex);
    void BindComputeTexture(Texture& tex, unsigned int id);
    void SaveComputeTexture(Texture& tex, std::string name, unsigned int width, unsigned int height);
    void BindTexture(unsigned int unit, unsigned int id);
    void BindTexture(unsigned int unit, const Texture& texture);
}
