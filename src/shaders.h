#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

std::string getFile(const char* fileLoc);

void compileShader(GLuint shader, const GLchar** source);

void debugShader(GLuint shader, const char* fileLoc);

void debugShaderProgram(GLuint shaderProgram);

inline GLuint shaderWithSource(const char* vsSrc, const char* fsSrc){
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertexShader, &vsSrc);
	debugShader(vertexShader, "Vertex Shader");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragmentShader, &fsSrc);
	debugShader(fragmentShader, "Fragment Shader");

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	debugShaderProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint shaderWithFile(const char* vsLoc, const char* fsLoc);
