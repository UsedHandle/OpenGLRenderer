#include "shaders.h"

#include <fstream>
#include <sstream>

std::string getFile(const char* fileLoc){
	std::ifstream fileStream(fileLoc);
	if(!fileStream){ 
		fprintf(stderr, "%s not found\n", fileLoc); 
		exit(EXIT_FAILURE); 
	}

	std::stringstream fileSStream;
	fileSStream << fileStream.rdbuf();
	fileStream.close();

	return fileSStream.str();
}

void compileShader(GLuint shader, const GLchar** source){
	glShaderSource(shader, 1, source, nullptr);
	glCompileShader(shader);
}

void debugShader(GLuint shader, const char* fileLoc){	
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(!success){
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		fprintf(stderr, "%s\n%s\n\n", fileLoc, infoLog);
	}
}

void debugShaderProgram(GLuint shaderProgram){
	int  success;
	char infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
			glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
			fprintf(stderr, "Shader Program Failed to link/create:\n%s\n", infoLog);
			exit(EXIT_FAILURE);
	}
}


GLuint shaderWithFile(const char* vsLoc, const char* fsLoc){
	const std::string vshaderInput = getFile(vsLoc);
	const char* vertexShaderSource = vshaderInput.c_str();
	const std::string fshaderInput = getFile(fsLoc);
	const char* fragmentShaderSource = fshaderInput.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertexShader, &vertexShaderSource);
	debugShader(vertexShader, vsLoc);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragmentShader, &fragmentShaderSource);
	debugShader(fragmentShader, fsLoc);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	debugShaderProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
