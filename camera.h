#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // glm::translate()
#include <glm/gtc/type_ptr.hpp>          // value_ptr();

#include "utility.h"


struct Camera{
	glm::mat4 view;
	glm::mat4 proj;

	glm::vec3 pos;
	
	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 n;
	
	float FOV, nearz, farz;

	float speed = 0.05f;

	Camera(const glm::vec3& pos, 
	       const glm::vec3& u,
				 const glm::vec3& v,
				 float FOV, float ar, float nearz, float farz);

	inline void calculateViewMat(){
		view = glm::lookAt(pos, pos + n, v);
	}

	template<typename T> constexpr
	inline void rotate(T radians, const glm::vec3& axis){
		view = glm::rotate(view, radians, axis);
	}

	
	void pollInput(Window* window);


	inline void sendViewMat(GLint uniLoc){ 
		glUniformMatrix4fv(uniLoc, 1, GL_FALSE, value_ptr(view));
	}

	inline void sendProjMat(GLint uniLoc){
		glUniformMatrix4fv(uniLoc, 1, GL_FALSE, value_ptr(proj));
	}

	inline void sendCamMat (GLint uniLoc){
		glUniformMatrix4fv(uniLoc, 1, GL_FALSE, value_ptr(proj * view));
	}
};
