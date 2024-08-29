#pragma once

#include <GLUT/glut.h>
#include <unordered_map>
#include <string>
#include <vector>

extern std::unordered_map<std::string, GLuint> textures;

GLuint loadTexture(const std::string& filePath);
void loadTextures(const std::vector<std::string>& filePaths);
GLuint createDefaultTexture();