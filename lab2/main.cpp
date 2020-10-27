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

void addNormals (GLfloat *vert, GLuint *ind, size_t n,
                 GLfloat *res, size_t len = 6){
	for (size_t i = 0, j = 0; i < n / 3; i++){
		size_t vertAind = (ind[i*3] * len);
		size_t vertBind = (ind[i*3 + 1] * len);
		size_t vertCind = (ind[i*3 + 2] * len);
		glm::vec3 a (vert[vertAind], vert[vertAind + 1], vert[vertAind + 2]);
		glm::vec3 b (vert[vertBind], vert[vertBind + 1], vert[vertBind + 2]);
		glm::vec3 c (vert[vertCind], vert[vertCind + 1], vert[vertCind + 2]);
		glm::vec3 r (glm::normalize(glm::cross(a - b, c - b)));

		for (size_t k = 0; k < len; k++){
			res[j + k] = vert[vertAind + k];
		}
		for (size_t k = len; k < len + 3; k++){
			res[j + k] = r[k - len];
		}
		j += len + 3;
		for (size_t k = 0; k < len; k++){
			res[j + k] = vert[vertBind + k];
		}
		for (size_t k = len; k < len + 3; k++){
			res[j + k] = r[k - len];
		}
		j += len + 3;
		for (size_t k = 0; k < len; k++){
			res[j + k] = vert[vertCind + k];
		}
		for (size_t k = len; k < len + 3; k++){
			res[j + k] = r[k - len];
		}
		j += len + 3;
	}
}

int main (int argc, char **argv){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Lab2",
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
		 2.0,  0.0, 0.0, 1.0, 0.0, 0.0,
		 3.0,  3.0, 0.0, 0.0, 1.0, 0.0,
		 0.0,  3.0, 0.0, 0.0, 0.0, 1.0,
		 0.0, -3.0, 0.0, 1.0, 1.0, 0.0,
		 3.0, -3.0, 0.0, 0.0, 1.0, 1.0,
		 0.0,  0.0, 5.0, 1.0, 0.0, 1.0
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		4, 3, 5,
		3, 2, 5,
		2, 1, 5,
		1, 0, 5
	};

	size_t indLen = sizeof(indices) / sizeof(*indices);

	GLfloat vertNorm [indLen * 9];
	addNormals(vertices, indices, indLen, vertNorm);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	// GLuint EBO;
	// glGenBuffers(1, &EBO);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertNorm), vertNorm, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 9*sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT),
	                      (GLvoid *)(3*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT),
	                      (GLvoid *)(6*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	//              GL_STATIC_DRAW);
	
	glBindVertexArray(0);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 9*sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	Shader shader("shader.vert", "shader.frag");
	Shader lampShader("lampShader.vert", "lampShader.frag");


	glm::vec3 ncStart(0.0, 0.0, 20.0);
	glm::vec3 planeNorm(-1.0, -1.0, 1.0);
	planeNorm = glm::normalize(planeNorm);

	glm::vec3 ncZ = planeNorm;
	glm::vec3 ncX = glm::normalize(glm::cross(ncZ, glm::vec3(0, 1, 0)));
	glm::vec3 ncY = glm::normalize(glm::cross(ncZ, ncX));
	// std::cout << glm::dot(ncZ, ncX) << std::endl;
	// std::cout << glm::dot(ncX, ncY) << std::endl;
	// std::cout << glm::dot(ncZ, ncY) << std::endl;

	glm::mat3 trans (ncX, ncY, ncZ);
	// trans = glm::inverse(trans);


	glm::vec3 lightTarget(0.0, 0.0, 0.0);

	GLfloat radA = 5.0;
	GLfloat radB = 10.0;
	GLfloat spd = 5.0;

	ncStart = planeNorm * 20.0f / sqrtf(3.0f);

	glm::mat4 model(1.0);
	glm::mat4 view = glm::lookAt(planeNorm * 40.0f, //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
	                                float(WIDTH)/float(HEIGHT), 0.1f, 100.0f);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat x = sin(glfwGetTime() * spd) * radA;
		GLfloat y = cos(glfwGetTime() * spd) * radB;
		glm::vec3 lightPos = trans * glm::vec3(x, y, 0) + ncStart;
		glm::vec3 lightDir = glm::normalize(lightTarget - lightPos);
		std::cout << -lightPos.x - lightPos.y + lightPos.z - 20 << std::endl;
		// std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << " " << std::endl;


		// view = glm::lookAt(glm::vec3(x, 0.0, z), //camPos
	    //                          glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	    //                          glm::vec3(0.0, 1.0, 0.0)); //camUpVec

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightDir", lightDir);


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, indLen);
		// glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glm::mat4 lampModel(1.0);
		lampModel = glm::translate(lampModel, lightPos);
		lampModel = glm::scale(lampModel, glm::vec3(0.1, 0.1, 0.1));

		lampShader.use();
		lampShader.setMat4("model", lampModel);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);


		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, indLen);
		// glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
}