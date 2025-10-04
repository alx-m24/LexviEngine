#pragma once

#include <string>
struct aiTexel;

namespace Lexvi {
    struct Texture
    {
        unsigned int id = 0;
        std::string type;
        std::string path;
    };

    unsigned int loadTexture(std::string path);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
    unsigned int TextureFromMemory(const unsigned char* data, size_t size);
    unsigned int TextureFromRawPixels(aiTexel* pixels, int width, int height);
    unsigned int GenerateDepthTexture(int width, int height);

    void BindTexture(unsigned int unit, unsigned int id);
}