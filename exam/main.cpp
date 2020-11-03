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
#include "camera.hpp"

const int WIDTH = 1920;
const int HEIGHT = 1080;

Camera camera(glm::vec3(0.0, 0.0, 3.0));
bool keys[1024];

GLfloat deltaTime = 0.0;
GLfloat lastFrame = 0.0;


GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

void key_callback (GLFWwindow *window, int key, int scancode, int action,
                                                                      int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS){
		keys[key] = true;
	}
	if (action == GLFW_RELEASE){
		keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	static GLfloat lastX = xpos, lastY = ypos;

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	camera.ProcessMouseScroll(yoffset);
}

void do_movement (){
	if(keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if(keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if(keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if(keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

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
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	Shader shader("shader.vert", "shaderG.frag");

	glm::vec3 lightTarget(0.0, 0.0, 0.0);
	glm::vec3 lightPos(0.0, 10.0, 0.0);
	glm::vec3 lightColor(1.0, 1.0, 1.0);
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

	glm::mat4 model(1.0);
	glm::mat4 view;/* = glm::lookAt(planeNorm * 40.0f, //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec*/
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
	                                float(WIDTH)/float(HEIGHT), 0.1f, 100.0f);

	GLfloat lightRad = 10.0;
	GLfloat lightSpd = 0.5;

	Model obj(std::filesystem::absolute("octo.obj"));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();

		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// GLfloat ly = sin(glfwGetTime() * lightSpd) * lightRad;
		// GLfloat lz = cos(glfwGetTime() * lightSpd) * lightRad;
		// lightPos = glm::vec3(0.0, ly, lz);
		// view = glm::lookAt(glm::vec3(vx, 1.0, vz), //camPos
	    //                          glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	    //                          glm::vec3(0.0, 1.0, 0.0)); //camUpVec
		// glm::vec3 lightDir = glm::normalize(lightTarget - glm::vec3(vx, 0.0, vz));
		// glm::vec3 lightDir = glm::normalize(lightTarget - glm::vec3(0.0, 0.0, 10.0));

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
	                                float(WIDTH)/float(HEIGHT), 0.1f, 100.0f);

		view = camera.GetViewMatrix();

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("light.position", lightPos);
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", diffuseColor);
		shader.setVec3("light.specular", 1.0, 1.0, 1.0);
		shader.setFloat("light.constant", 1.0);
		shader.setFloat("light.linear", 0.022);
		shader.setFloat("light.quadratic", 0.0019);
		shader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		// shader.setVec3("lightDir", lightDir);
		obj.draw(shader);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}