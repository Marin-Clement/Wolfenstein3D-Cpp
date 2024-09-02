#define STB_IMAGE_IMPLEMENTATION

#include "texture.hpp"
#include <iostream>
#include <unordered_map>
#include <string>
#include <stb_image.h>
#include <__filesystem/directory_iterator.h>

std::unordered_map<std::string, GLuint> textures;

GLuint createDefaultTexture() {
    const int width = 32;
    const int height = 32;
    unsigned char data[width * height * 3];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int offset = (y * width + x) * 3;
            if ((x / 8 + y / 8) % 2 == 0) {
                data[offset] = 255;     // R
                data[offset + 1] = 0;   // G
                data[offset + 2] = 255; // B
            } else {
                data[offset] = 0;       // R
                data[offset + 1] = 0;   // G
                data[offset + 2] = 0;   // B
            }
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}

GLuint loadTexture(const std::string& filePath) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
            stbi_image_free(data);
            return createDefaultTexture();
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
        return textureID;
    }
    std::cerr << "Failed to load texture: " << filePath << " - " << stbi_failure_reason() << std::endl;
    return createDefaultTexture();
}


void printTextures() {
    for (const auto& texture : textures) {
        std::cout << "Texture: " << texture.first << " - ID: " << texture.second << std::endl;
    }
}

void loadTextures(const std::string& directoryPath) {
    for (const auto& entry : std::__fs::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            std::string baseName = entry.path().stem().string();
            GLuint textureID = loadTexture(filePath);
            if (textureID != 0) {
                textures[baseName] = textureID;
            }
        }
    }
    printTextures();
}

