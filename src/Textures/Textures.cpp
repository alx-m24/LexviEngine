#include "pch.h"

#include "Textures/Textures.hpp"

namespace fs = std::filesystem;

namespace Lexvi {

    static void setTexParams(GLuint id) {
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    static void chooseFormat(int nrComponents, GLenum& format, GLenum& internalFormat) {
        if (nrComponents == 1) { format = GL_RED; internalFormat = GL_R8; }
        else if (nrComponents == 3) { format = GL_RGB; internalFormat = GL_SRGB8; }
        else if (nrComponents == 4) { format = GL_RGBA; internalFormat = GL_SRGB8_ALPHA8; }
        else { format = GL_RGB; internalFormat = GL_RGB8; }
    }

    static void chooseComputeFormat(int nrComponents, GLenum& format, GLenum& internalFormat) {
        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_R8;  // single channel linear
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = GL_RGBA8; // linear RGB for compute, not sRGB
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = GL_RGBA8; // linear RGBA for compute
        }
        else {
            format = GL_RGB;
            internalFormat = GL_RGBA8;
        }
    }

    Texture loadTextureArray(const std::vector<std::string>& paths) {
        Texture tex;

        if (paths.empty()) {
            std::cerr << "No texture paths provided for texture array.\n";
            return tex;
        }

        int width = 0, height = 0, nrComponents = 0;
        unsigned char* data = stbi_load(paths[0].c_str(), &width, &height, &nrComponents, 0);
        if (!data) {
            std::cerr << "Failed to load first texture: " << paths[0] << "\n";
            return tex;
        }

        GLenum format, internalFormat;
        chooseFormat(nrComponents, format, internalFormat);
        tex.format = format;
        tex.internalFormat = internalFormat;

        GLuint id;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage3D(id, mipLevels, internalFormat, width, height, static_cast<int>(paths.size()));
        glTextureSubImage3D(id, 0, 0, 0, 0, width, height, 1, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        for (size_t i = 1; i < paths.size(); ++i) {
            int w, h, comps;
            unsigned char* layerData = stbi_load(paths[i].c_str(), &w, &h, &comps, 0);
            if (!layerData) continue;
            if (w == width && h == height)
                glTextureSubImage3D(id, 0, 0, 0, static_cast<int>(i), width, height, 1, format, GL_UNSIGNED_BYTE, layerData);
            stbi_image_free(layerData);
        }

        setTexParams(id);
        glGenerateTextureMipmap(id);

        tex.id = id;
        tex.path = paths[0];
        tex.type = "array";
        return tex;
    }

    Texture loadTexture(const std::string& path) {
        Texture tex;
        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (!data) {
            std::cerr << "Texture failed to load at path: " << path << "\n";
            return tex;
        }

        GLenum format, internalFormat;
        chooseFormat(nrComponents, format, internalFormat);

        GLuint id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage2D(id, mipLevels, internalFormat, width, height);
        glTextureSubImage2D(id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(id);
        setTexParams(id);
        stbi_image_free(data);

        tex.id = id;
        tex.path = path;
        tex.format = format;
        tex.internalFormat = internalFormat;
        tex.type = "2D";
        return tex;
    }

    Texture TextureFromFile(const char* path, const std::string& directory) {
        fs::path texPath = fs::path(directory) / fs::path(path);
        return loadTexture(texPath.string());
    }

    Texture TextureFromMemory(const unsigned char* data, size_t size) {
        Texture tex;
        int width, height, nrComponents;
        unsigned char* img = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &nrComponents, 0);
        if (!img) {
            std::cerr << "Failed to load texture from memory\n";
            return tex;
        }

        GLenum format, internalFormat;
        chooseFormat(nrComponents, format, internalFormat);

        GLuint id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage2D(id, mipLevels, internalFormat, width, height);
        glTextureSubImage2D(id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, img);
        glGenerateTextureMipmap(id);
        setTexParams(id);
        stbi_image_free(img);

        tex.id = id;
        tex.format = format;
        tex.internalFormat = internalFormat;
        tex.type = "memory";
        return tex;
    }

    Texture TextureFromRawPixels(aiTexel* pixels, int width, int height) {
        Texture tex;
        std::vector<unsigned char> data(width * height * 4);
        for (int i = 0; i < width * height; ++i) {
            data[4 * i + 0] = pixels[i].r;
            data[4 * i + 1] = pixels[i].g;
            data[4 * i + 2] = pixels[i].b;
            data[4 * i + 3] = pixels[i].a;
        }

        GLenum format = GL_RGBA, internalFormat = GL_RGBA8;
        GLuint id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage2D(id, mipLevels, internalFormat, width, height);
        glTextureSubImage2D(id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data.data());
        glGenerateTextureMipmap(id);
        setTexParams(id);

        tex.id = id;
        tex.format = format;
        tex.internalFormat = internalFormat;
        tex.type = "raw";
        return tex;
    }

    Texture GenerateDepthTexture(int width, int height) {
        Texture tex;
        GLuint id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        glTextureStorage2D(id, 1, GL_DEPTH_COMPONENT32F, width, height);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float border[] = { 1, 1, 1, 1 };
        glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, border);

        tex.id = id;
        tex.format = GL_DEPTH_COMPONENT;
        tex.internalFormat = GL_DEPTH_COMPONENT32F;
        tex.type = "depth";
        return tex;
    }

    std::vector<unsigned char> getRawTexturePixels(std::string path)
    {
        std::vector<unsigned char> pixels;

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            const size_t pixelNum = static_cast<size_t>(width) * height;
            pixels.resize(pixelNum * nrComponents);

            std::memcpy(pixels.data(), data, pixels.size());


            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return pixels;
    }

    std::vector<unsigned char> getGreyScaleTexturePixels(std::string path)
    {
        std::vector<unsigned char> pixels;

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 1);
        if (data)
        {
            const size_t pixelNum = static_cast<size_t>(width) * height;
            pixels.resize(pixelNum);

            std::memcpy(pixels.data(), data, pixels.size());


            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return pixels;
    }

    Texture Create3DComputeTexture(unsigned int layers, unsigned int width, unsigned int height, unsigned int channelNum) {
        Texture tex;
        GLenum format, internalFormat;
        chooseComputeFormat(channelNum, format, internalFormat);

        GLuint id;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage2D(id, mipLevels, internalFormat, width, height);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        tex.id = id;
        tex.format = format;
        tex.internalFormat = internalFormat;
        tex.type = "compute3D";
        return tex;
    }

    Texture CreateComputeTexture(unsigned int width, unsigned int height, unsigned int channelNum) {
        Texture tex;
        GLenum format, internalFormat;
        chooseComputeFormat(channelNum, format, internalFormat);

        GLuint id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        int mipLevels = 1 + (int)std::floor(std::log2(std::max(width, height)));
        glTextureStorage2D(id, mipLevels, internalFormat, width, height);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        tex.id = id;
        tex.format = format;
        tex.internalFormat = internalFormat;
        tex.type = "compute";
        return tex;
    }

    void GenerateMipMaps(Texture& tex)
    {
        glGenerateTextureMipmap(tex.id);
    }

    void BindComputeTexture(Texture& tex, unsigned int id)
    {
        bool layered = (tex.type == "compute3D");
        glBindImageTexture(id, tex.id, 0, layered, 0, GL_WRITE_ONLY, tex.internalFormat);
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