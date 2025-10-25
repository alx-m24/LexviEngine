#include "pch.h"

#include "Textures/Textures.hpp"

namespace fs = std::filesystem;

namespace Lexvi {

    unsigned int loadTexture(std::string path)
    {
        unsigned int textureID = 0;

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            GLenum internalFormat = GL_SRGB;
            if (nrComponents == 1) {
                format = GL_RED;
                internalFormat = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
                internalFormat = GL_SRGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
                internalFormat = GL_SRGB_ALPHA;
            }
            else {
                std::cerr << "Failed to get texture format\n";
                return 0;
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

            glTextureStorage2D(textureID, 1, internalFormat, width, height);
            glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
            glGenerateTextureMipmap(textureID);

            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    unsigned int TextureFromFile(const char* path, const std::string& directory)
    {
        namespace fs = std::filesystem;

        fs::path texPath = fs::path(directory) / fs::path(path);
        texPath = texPath.lexically_normal(); // resolves .. and mixed slashes

        std::string filename = texPath.string();

        unsigned int textureID = 0;

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            GLenum internalFormat = GL_SRGB;
            if (nrComponents == 1) {
                format = GL_RED;
                internalFormat = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
                internalFormat = GL_SRGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
                internalFormat = GL_SRGB_ALPHA;
            }
            else {
                std::cerr << "Failed to get texture format\n";
                return 0;
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

            glTextureStorage2D(textureID, 1, internalFormat, width, height);
            glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
            glGenerateTextureMipmap(textureID);

            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    unsigned int TextureFromMemory(const unsigned char* data, size_t size) {
        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true); // if needed
        unsigned char* image = stbi_load_from_memory(data, static_cast<int>(size),
            &width, &height, &nrComponents, 0);
        if (!image) {
            std::cerr << "Failed to load texture from memory\n";
            return 0;
        }

        GLenum format = GL_RGB;
        GLenum internalFormat = GL_SRGB;
        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = GL_SRGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = GL_SRGB_ALPHA;
        }
        else {
            std::cerr << "Failed to get texture format\n";
            return 0;
        }

        unsigned int textureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

        glTextureStorage2D(textureID, 1, internalFormat, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, image);
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(image);
        return textureID;
    }
    unsigned int TextureFromRawPixels(aiTexel* pixels, int width, int height) {
        // aiTexel is always 4 bytes: BGRA (0-255)
        std::vector<unsigned char> data(width * height * 4);

        for (int i = 0; i < width * height; i++) {
            data[4 * i + 0] = pixels[i].r;
            data[4 * i + 1] = pixels[i].g;
            data[4 * i + 2] = pixels[i].b;
            data[4 * i + 3] = pixels[i].a;
        }

        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;

        unsigned int textureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

        glTextureStorage2D(textureID, 1, internalFormat, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data.data());
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return textureID;
    }

    unsigned int GenerateDepthTexture(int width, int height)
    {
        unsigned int depthMap;
        glCreateTextures(GL_TEXTURE_2D, 1, &depthMap);

        glTextureStorage2D(depthMap, 1, GL_DEPTH_COMPONENT32F, width, height);

        glTextureParameteri(depthMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(depthMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(depthMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(depthMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameteri(depthMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTextureParameteri(depthMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTextureParameterfv(depthMap, GL_TEXTURE_BORDER_COLOR, borderColor);

        return depthMap;
    }

    void CreateComputeTexture(Texture& tex, unsigned int width, unsigned int height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &tex.id);
        glTextureStorage2D(tex.id, 1, GL_RGBA8, width, height);

        glTextureParameteri(tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(tex.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void BindComputeTexture(Texture& tex)
    {
        glBindImageTexture(0, tex.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    }

    void SaveComputeTexture(Texture& tex, std::string name, unsigned int width, unsigned int height)
    {
        std::vector<unsigned char> pixels(width * height * 4); // RGBA8 -> 4 bytes per pixel

        glBindTexture(GL_TEXTURE_2D, tex.id);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        stbi_flip_vertically_on_write(1); // OpenGL’s origin is bottom-left

        stbi_write_png(std::string(name + ".png").c_str(), width, height, 4, pixels.data(), width * 4);
    }


    void BindTexture(unsigned int unit, unsigned int id)
    {
        glBindTextureUnit(unit, id);
    }

}