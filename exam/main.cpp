#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include <filesystem>
#include "shader.hpp"
#include "model.hpp"

const int WIDTH = 1920;
const int HEIGHT = 1080;

int main (int argc, char **argv){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exam",
	                                      nullptr, nullptr);
	if (window == nullptr){
		std::cout << glfwGetError(NULL) << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK){
		std::cout << "GLEW init fail" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	// stbi_set_flip_vertically_on_load(true);

	Shader shader("shader.vert", "shader.frag");

	glm::vec3 lightTarget(0.0, 0.0, 0.0);

	glm::mat4 model(1.0);
	glm::mat4 view;/* = glm::lookAt(planeNorm * 40.0f, //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec*/
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
	                                float(WIDTH)/float(HEIGHT), 0.1f, 100.0f);

	GLfloat viewRad = 10.0;
	GLfloat viewSpd = 2.0;

	Model obj(std::filesystem::absolute("eyeball.obj"));

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		GLfloat vx = sin(glfwGetTime() * viewSpd) * viewRad;
		GLfloat vz = cos(glfwGetTime() * viewSpd) * viewRad;
		view = glm::lookAt(glm::vec3(vx, 1.0, vz), //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec
		glm::vec3 lightDir = glm::normalize(lightTarget - glm::vec3(vx, 0.0, vz));
		// glm::vec3 lightDir = glm::normalize(lightTarget - glm::vec3(0.0, 0.0, 10.0));

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		// shader.setVec3("lightDir", lightDir);
		obj.draw(shader);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}