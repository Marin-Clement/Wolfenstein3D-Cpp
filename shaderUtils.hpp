#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string readShaderSource(const std::string& filePath);
GLuint compileShader(const std::string& source, GLenum shaderType);
GLuint createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

#endif // SHADER_UTILS_H