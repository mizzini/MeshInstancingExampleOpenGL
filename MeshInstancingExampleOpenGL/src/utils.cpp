#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Returns the content of glsl file
std::string readShaderSource(const char *filePath)
{
	std::ifstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open())
	{
		std::cerr << "Failed to open shader file: " << filePath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	fileStream.close();
	return buffer.str();
}

void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0; // characters written?
	char *log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		std::cout << "Shader Info Log: " << log << std::endl;

		// Deallocate
		free(log);
	}
}

void printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char *log;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);

	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		std::cout << "Program Info Log:" << log << std::endl;
		free(log);
	}
}

bool checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError:" << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

GLuint createShaderProgram(const char *vsStr, const char *fsStr)
{
	std::string vertShaderStr = readShaderSource(vsStr);
	std::string fragShaderStr = readShaderSource(fsStr);

	const char *vShaderSource = vertShaderStr.c_str();
	const char *fShaderSource = fragShaderStr.c_str();

	// In OpenGL, GLuint is used to store IDs for OpenGL objects like: VAOs, VBOs, textures, etc.
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Catch errors...
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	// glShaderSource is used to set the source code for a shader before compiling it with glCompileShader.
	glShaderSource(vShader, 1, &vShaderSource, NULL);
	glShaderSource(fShader, 1, &fShaderSource, NULL);

	// Compile the shader source codes and check for errors.
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		std::cout << "Vertex compilation failed." << std::endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		std::cout << "fragment compilation failed" << std::endl;
		printShaderLog(fShader);
	}

	// Need to create this program object and attach multiple shaders to it,  
	// in order to compile them into a single executable.
	GLuint vfProgram = glCreateProgram();

	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	// Link shaders.
	glLinkProgram(vfProgram);

	// Check for errors.
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "Linking failed." << std::endl;
		printProgramLog(vfProgram);
	}

	// Delete shaders after linking.
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}