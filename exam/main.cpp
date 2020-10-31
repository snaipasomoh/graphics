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

		// std::cout << a.x << " " << a.y << " " << a.z << " " << std::endl;
		// std::cout << b.x << " " << b.y << " " << b.z << " " << std::endl;
		// std::cout << c.x << " " << c.y << " " << c.z << " " << std::endl;
		// std::cout << std::endl;
	}
}

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

	Shader shader("shader.vert", "shader.frag");

	glm::vec3 lightTarget(0.0, 0.0, 0.0);
	glm::vec3 lightPos(0.0, 0.0, 10.0);
	glm::vec3 lightColor(1.0, 1.0, 1.0);
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

	glm::mat4 model(1.0);
	glm::mat4 view;/* = glm::lookAt(planeNorm * 40.0f, //camPos
	                             glm::vec3(0.0, 0.0, 0.0), //camTargetPos
	                             glm::vec3(0.0, 1.0, 0.0)); //camUpVec*/
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
	                                float(WIDTH)/float(HEIGHT), 0.1f, 100.0f);

	GLfloat viewRad = 10.0;
	GLfloat viewSpd = 2.0;

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


		// GLfloat vx = sin(glfwGetTime() * viewSpd) * viewRad;
		// GLfloat vz = cos(glfwGetTime() * viewSpd) * viewRad;
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
		shader.setVec3("light.direction", lightTarget - lightPos);
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", diffuseColor);
		shader.setVec3("light.specular", 1.0, 1.0, 1.0);
		shader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		// shader.setVec3("lightDir", lightDir);
		obj.draw(shader);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}