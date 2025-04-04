#pragma once

#include "setup.h"
#include <cstdint>
#include <initializer_list>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct WinData {
	double xoffset, yoffset;
	double xpos{}, ypos{};

	float aspectrat;
	int width, height;
	unsigned int framerate;
};


struct Window{
	WinData windat;	
	
	GLFWwindow* glfwwindow;

	double time{};
	double dt;

	Window(
		int widthb,
		int heightb,
		const char* titleb,
		GLFWmonitor* monitorb,
		GLFWwindow* shareb,
		unsigned int framerateb,
		std::initializer_list<GLenum> caplist = {}
	);

	~Window(){ destroy(); }

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&& b) :
		windat(b.windat),
		glfwwindow(b.glfwwindow)
	{
		b.glfwwindow = nullptr;
	}

	Window& operator=(Window&& b){
		if(this != &b){
			destroy();
			std::swap(windat, b.windat);
			std::swap(glfwwindow, b.glfwwindow);
		}

		return *this;
	}

	inline int shouldClose(){ return glfwWindowShouldClose(glfwwindow); }

	void updateAndClear(GLenum bufferbit);

	inline double adjDelta() const { return dt * static_cast<double>(windat.framerate); }

	inline void destroy(){
		glfwDestroyWindow(glfwwindow);
	}


};

struct Context{
	inline Context(int versionmajor = 3, int versionminor = 3){
		glfwGLsetup(versionmajor, versionminor);
	}

	inline ~Context(){
		glfwTerminate();
	}
};
