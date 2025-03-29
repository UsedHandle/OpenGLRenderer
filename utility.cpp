#include "utility.h"
#include <GLFW/glfw3.h>

Window::Window
(
	int widthb, int heightb, const char* titleb,
	GLFWmonitor* monitorb, GLFWwindow* shareb,
	unsigned int framerateb,
	std::initializer_list<GLenum> caplist
)
{
	windat = {
		0.0, 0.0,
		static_cast<double>(widthb)/2.0, static_cast<double>(heightb)/2.0,
		static_cast<float>(widthb)/static_cast<float>(heightb),
		widthb, heightb,
		framerateb,
  };
	glfwwindow = glfwCreateWindow(windat.width, windat.height, titleb, monitorb, shareb);
	glfwSetWindowUserPointer(glfwwindow, &windat);
	configureglfwWindow(glfwwindow, caplist);
	glfwSetCursorPosCallback(glfwwindow,
		[](GLFWwindow* cbwindow, double cbxpos, double cbypos){	
			WinData* data = reinterpret_cast<WinData*>(glfwGetWindowUserPointer(cbwindow));
			data->xoffset = data->xpos;
			data->yoffset = data->ypos;
			
			data->xpos = cbxpos;
			data->ypos = cbypos;

			data->xoffset = data->xpos - data->xoffset;
			data->yoffset = data->ypos - data->yoffset;
	});
}


void Window::updateAndClear(GLenum bufferbit){
	dt = time;
	time = glfwGetTime();
	dt = time - dt;

	glfwSwapBuffers(glfwwindow);
	glfwPollEvents();	
	glClear(bufferbit);	
}
