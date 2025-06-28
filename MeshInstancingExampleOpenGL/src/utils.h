#pragma once

#include <string>
#include <glad/glad.h>

// These functions exist. Trust me.
// Linker will find it.
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();
GLuint createShaderProgram(const char* vsStr, const char* fsStr);

std::string readShaderSource(const char *filePath);