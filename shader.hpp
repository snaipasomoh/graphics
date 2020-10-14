#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

class Shader{
	public:

	GLuint Program;

	Shader (GLchar const *vertexPath, GLchar const *fragmentPath);

	void Use ();

	void SetVec3 (GLchar const *arg, GLfloat x, GLfloat y, GLfloat z);

	void SetFloat (GLchar const *arg, GLfloat x);
};

Shader::Shader (GLchar const *vertexPath, GLchar const *fragmentPath){
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	vertexFile.exceptions(std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::badbit);

	try{
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (std::ifstream::failure e){
		std::cerr << e.what() << std::endl;
	}

	GLchar const *vertexCodeC = vertexCode.c_str();
	GLchar const *fragmentCodeC = fragmentCode.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCodeC, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cerr << "VShader compilation failure\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCodeC, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cerr << "FShader compilation failure\n" << infoLog << std::endl;
	}

	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);

	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cerr << "Program linking failure\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use (){
	glUseProgram(this->Program);
}

void Shader::SetVec3 (GLchar const *arg, GLfloat x, GLfloat y, GLfloat z){
	glUniform3f(glGetUniformLocation(this->Program, arg), x, y, z);
}

void Shader::SetFloat (GLchar const *arg, GLfloat x){
	glUniform1f(glGetUniformLocation(this->Program, arg), x);
}

#endif