#pragma once
#include <OpenGL/OpenGL.h> // Include for CGLContextObj and related functions

GLuint loadShader(const char* filePath, GLenum shaderType);
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);