#include "utility.h"
#include "shaders.h"
#include "camera.h"
#include "model.h"
#include "fonts.h"

#include <GLFW/glfw3.h>
#include <assimp/matrix4x4.h>
#include <glm/gtx/transform.hpp>

#include <string>

int main(){
	static int width = 720;
	static int height = 720;

	float FOV   = 45.0f;
	float nearz = 0.2f;
	float farz  = 100.0f;

	static Camera camera(
		glm::vec3{0.0f, 0.0f, -10.0f},
		glm::vec3{1.0f, 0.0f, 0.0f},
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::radians(FOV), 1.0f, nearz, farz
	);

	Context oglcontext;
	Window window(width, height, "test", nullptr, nullptr, 75,
			{GL_DEPTH_TEST, GL_MULTISAMPLE, GL_CULL_FACE});
	/* glfwSwapInterval(0); */	

	glfwSetFramebufferSizeCallback(window.glfwwindow, [](GLFWwindow* cbwindow, int fbWidth, int fbHeight){
		WinData* data = reinterpret_cast<WinData*>(glfwGetWindowUserPointer(cbwindow));
		glfwGetWindowSize(cbwindow, &data->width, &data->height);
		width = data->width;
		height = data->height;
		data->aspectrat = static_cast<float>(data->width)/static_cast<float>(data->height);

		camera.proj = glm::perspective(camera.FOV, data->aspectrat, camera.nearz, camera.farz);
		glViewport(0, 0, fbWidth, fbHeight);
	});

	//Model model("resources/box.glb");
	//Model model("resources/newell_teaset/teapot.obj");
	/* Model model("./bunny/scene.gltf"); */
	/* Model model("FlightHelmet/glTF/FlightHelmet.gltf"); */
	Model model("resources/Sponza/glTF/Sponza.gltf", glm::scale(glm::mat4(1.f), glm::vec3(0.02f)));
	/* Model model("TextureCoordinateTest/glTF/TextureCoordinateTest.gltf"); */
	/* Model model("./backpack/backpack.obj"); // aiProcess_FlipUVs */
	/* Model model("./blenderTransformations/untitled.glb"); */

	ftr::Font font("resources/fonts/RobotoMono-Thin.ttf", 28, 128);

	GLuint shaderProgram = shaderWithFile("shaders/shadervert.glsl", "shaders/shaderfrag.glsl");
	GLuint lightProgram  = shaderWithFile("shaders/lightvert.glsl", "shaders/lightfrag.glsl");

	glm::vec3 lightPos(3.0f, 6.0f, 0.0f);
	glm::vec3 lightCol(0.8f, 0.88f, 0.96f);
	glm::vec3 ambience = lightCol * 0.35f;

	GLuint lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float), &lightPos[0], GL_STATIC_DRAW);	

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(float)*3, reinterpret_cast<void*>(0));
	
	glPointSize(15.0f);
	glClearColor(0.168f, 0.2f, 0.223f, 1.0f);

	GLint diffLoc   = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLint specLoc   = glGetUniformLocation(shaderProgram, "material.specular");
	GLint objLoc    = glGetUniformLocation(shaderProgram, "object");
	GLint sceneTLoc = glGetUniformLocation(shaderProgram, "sceneTrans");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while(!window.shouldClose()){
		window.updateAndClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.pollInput(&window);

		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 128);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuse"), 1, &lightCol[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.specular"), 1, &lightCol[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambience"), 1, &ambience[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, &camera.pos[0]);
		camera.sendProjMat(glGetUniformLocation(shaderProgram, "proj"));	
		camera.sendViewMat(glGetUniformLocation(shaderProgram, "view"));
		model.draw(diffLoc, specLoc, objLoc, sceneTLoc);
		
		double fps = 1.0/window.dt;
		font.draw2DText(static_cast<float>(width), static_cast<float>(height), std::to_wstring(fps), 0.0f, static_cast<float>(height));
		glUseProgram(lightProgram);
		camera.sendProjMat(glGetUniformLocation(lightProgram, "proj"));	
		camera.sendViewMat(glGetUniformLocation(lightProgram, "view"));
		glUniform3fv(glGetUniformLocation(lightProgram, "lightCol"), 1, &lightCol[0]);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_POINTS, 0, 1);
	}


	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);
}
