#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include <array>

inline void glfwGLsetup(int versionmajor, int versionminor){	
	glfwSetErrorCallback([](int error, const char* description){
		fprintf(stderr, "GLFW Error (%d):\n%s\n", error, description);
		exit(EXIT_FAILURE);
	});
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionmajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionminor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
	
}


inline void configureglfwWindow(GLFWwindow* window, const std::initializer_list<GLenum>& caplist){
	if(!window){
		glfwTerminate();		
		fprintf(stderr, "Failed to create a window with GLFW\n");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	
	if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){
		fprintf(stderr, "Failed to initialize GLAD\n");
		exit(EXIT_FAILURE);	
	}

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);
	
	if(glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	for(GLenum capability : caplist)
		glEnable(capability);
}
