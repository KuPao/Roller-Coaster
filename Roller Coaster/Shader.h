#ifndef SHADER_H
#define SHADER_H
#define GLEW_STATIC
#ifdef GLEW_STATIC
#  define GLEWAPI extern
#else
#  ifdef GLEW_BUILD
#    define GLEWAPI extern __declspec(dllexport)
#  else
#    define GLEWAPI extern __declspec(dllimport)
#  endif
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

class Shader {
public:
	Shader(const char *vertex_path, const char *geometry_path, const char *fragment_path);
	void UseProgram() { glUseProgram(program); }
	GLuint GetProgram() { return program; }

private:
	std::string readFile(const char *filePath);
	GLuint loadShader(const char *vertex_path, const char *geometry_path, const char *fragment_path);
	GLuint createBasicShader(int shaderCmd, const char *path);
	GLuint compileShader(int shaderCmd, GLuint shader, const char *Source);
	void debugShader(GLuint shader);
	GLuint program = 0;
};

#endif