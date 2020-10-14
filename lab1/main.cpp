#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"

const int WIDTH = 1920;
const int HEIGHT = 1080;

int main (int argc, char **argv){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Lab1",
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

	GLfloat vertices[] = {
		 2.0,  0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
		 3.0,  3.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
		 0.0,  3.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		 0.0, -3.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0,
		 3.0, -3.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	GLuint EBO;
	glGenBuffers(1, &EBO);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 9*sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT),
	                      (GLvoid *)(3*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT),
	                      (GLvoid *)(6*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	             GL_STATIC_DRAW);
	
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	Shader shader("shader.vert", "shader.frag");
	glm::mat4 model(1.0);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 10.0), //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
	                                        float(WIDTH/HEIGHT), 0.1f, 20.0f);

	glm::vec3 lightDir(0.0, 0.0, -1.0);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightDir", lightDir);


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
}