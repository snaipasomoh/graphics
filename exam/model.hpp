#pragma once
#include "mesh.hpp"
#include <iostream>
#include <SOIL/SOIL.h>
#define STB_IMAGE_IMPLEMENTATION

// #include <stb/stb_image.h>
#include <SOIL/stb_image_aug.h>

class Model{
	public:
	Model (std::string const &path);
	void draw (Shader shader);

	private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	void loadModel (std::string path);
	void processNode (aiNode *node, aiScene const *scene);
	Mesh processMesh (aiMesh *mesh, aiScene const *scene);
	std::vector<Texture> loadMaterialTextures (aiMaterial *mat,
	                                aiTextureType type, std::string typeName);
};

Model::Model (std::string const &path){
	loadModel(path);
}

unsigned int TextureFromFile(const char *path, const std::string &directory,
                             bool gamma = false){
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	// std::cout << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data){
		GLenum format;
		if (nrComponents == 1){
			format = GL_RED;
		}
		else if (nrComponents == 3){
			format = GL_RGB;
		}
		else if (nrComponents == 4){
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		             GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::draw (Shader shader){
	for (size_t i = 0 ; i < meshes.size(); i++){
		meshes[i].draw(shader);
	}
}

void Model::loadModel (std::string path){
	Assimp::Importer importer;
	uint loadFlags = aiProcess_Triangulate | aiProcess_FlipUVs |
	                 aiProcess_CalcTangentSpace;
	aiScene const *scene = importer.ReadFile(path, loadFlags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode (aiNode *node, aiScene const *scene){
	for (size_t i = 0; i < node->mNumMeshes; i++){
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (size_t i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh (aiMesh *mesh, aiScene const *scene){
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; i++){
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		if (mesh->mTextureCoords[0]){
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else{
			vertex.TexCoords.x = 0.0;
			vertex.TexCoords.y = 0.0;
		}

		vertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++){
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0){
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
		                            aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material,
		                            aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		
		// bug in libassimp: normal map has type HEIGHT
		std::vector<Texture> normalMaps = loadMaterialTextures(material,
		                            aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}
	
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                aiTextureType type, std::string typeName){
	std::vector<Texture> textures;
	// std::cout << "called" << std::endl;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++){
		// std::cout << "called for" << std::endl;
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (size_t j = 0; j < textures_loaded.size(); j++){
			if(std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0){
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip){
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}