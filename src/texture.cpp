#include "texture.hpp"
#include <iostream>
#include <unordered_map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

GLuint loadTexture(const std::string& filePath) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        std::cout << "Texture loaded successfully: " << filePath << std::endl;
        return textureID;
    } else {
        std::cerr << "Failed to load texture: " << filePath << " - " << stbi_failure_reason() << std::endl;
        return createDefaultTexture();
    }
}

void loadTextures(const std::vector<std::string>& filePaths) {
    for (const auto& filePath : filePaths) {
        GLuint textureID = loadTexture(filePath);
        if (textureID != 0) {
            textures[filePath] = textureID;
        }
    }
}