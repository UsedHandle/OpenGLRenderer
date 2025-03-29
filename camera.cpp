#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>     // glm::rotateY()
#include <glm/gtx/vector_angle.hpp>

#include "camera.h"

// silly but saves space
#define FLOAT(value) static_cast<float>(value)

Camera::Camera(const glm::vec3& posb, 
               const glm::vec3& ub,
							 const glm::vec3& vb,
							 float FOVb, float arb, float nearzb, float farzb
							 ) : view(1.0f), 
							     pos (std::move(posb)),
									 u   (std::move(ub)),
									 v   (std::move(vb)),
									 FOV (std::move(FOVb)),nearz(std::move(nearzb)),farz(std::move(farzb))
{
	u = normalize(u);
	v = normalize(v);
	n = cross(u, v);
	
	proj = glm::perspective(FOV, arb, nearz, farz);

	calculateViewMat();
}

void Camera::pollInput(Window* window){
	// Movement
	glm::vec3 target(0.0f); 
	if(glfwGetKey(window->glfwwindow, GLFW_KEY_W))
		target += n;

	if(glfwGetKey(window->glfwwindow, GLFW_KEY_S))
		target -= n;
	
	if(glfwGetKey(window->glfwwindow, GLFW_KEY_A))
		target += cross(v, n);
	
	if(glfwGetKey(window->glfwwindow, GLFW_KEY_D))
		target -= cross(v, n);
	
	if(length(target) != 0.0f){
		target = normalize(target);
		pos += target * speed * FLOAT( window->adjDelta() );
	}



	// Keyboard Look
	if(glfwGetKey(window->glfwwindow, GLFW_KEY_LEFT)){
		u = glm::rotateY( u, 0.01f * FLOAT(window->adjDelta()) );
		v = glm::rotateY( v, 0.01f * FLOAT(window->adjDelta()) );
	}

	if(glfwGetKey(window->glfwwindow, GLFW_KEY_RIGHT)){
		u = glm::rotateY( u, -0.01f * FLOAT(window->adjDelta()) );
		v = glm::rotateY( v, -0.01f * FLOAT(window->adjDelta()) );
	}

	if(glfwGetKey(window->glfwwindow, GLFW_KEY_UP)){
		v = glm::rotate(v, -0.01f * FLOAT(window->adjDelta()), u);
	}
		
	if(glfwGetKey(window->glfwwindow, GLFW_KEY_DOWN)){
		v = glm::rotate(v, 0.01f * FLOAT(window->adjDelta()), u);
	}
	
	n = cross(u, v);
	calculateViewMat();	
}
