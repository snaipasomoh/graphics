#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "shader.hpp"

struct Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture{
	uint id;
	std::string type;
	aiString path;
};

class Mesh{
	public:
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<Texture> textures;

	Mesh (std::vector<Vertex> vertices, std::vector<uint> indices,
	      std::vector<Texture> textures);
	void draw (Shader shader);

	private:
	uint VAO, VBO, EBO;
	
	void setupMesh ();
};


Mesh::Mesh (std::vector<Vertex> vertices, std::vector<uint> indices,
      std::vector<Texture> textures){
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->setupMesh();
}

void Mesh::setupMesh (){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
	             &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(),
	             &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      (void *)offsetof(Vertex, Normal));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      (void *)offsetof(Vertex, TexCoords));
	
	glBindVertexArray(0);
}

void Mesh::draw (Shader shader){
	uint diffuseNr = 1;
	uint specularNr = 1;
	uint normalNr = 1;
	uint emissiveNr = 1;
	
	for (size_t i = 0; i < textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse"){
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular"){
			number = std::to_string(specularNr++);
		}
		else if (name == "texture_normal"){
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_emissive"){
			number = std::to_string(emissiveNr++);
		}
		shader.setInt((name + number).c_str(), i);
		// std::cout << name + number << std::endl;
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}