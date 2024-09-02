#pragma once

#include <GLUT/glut.h>
#include <unordered_map>
#include <string>

extern std::unordered_map<std::string, GLuint> textures;

GLuint loadTexture(const std::string& filePath);
void loadTextures(const std::string& directory);
GLuint createDefaultTexture();