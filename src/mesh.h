#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/vector3.h>
#include <glad/glad.h>

#include "textures.h"

#define POSITION_LOCATION  0
#define NORMAL_LOCATION    1
#define TEXCOORD_LOCATION  2

struct Mesh {
	GLuint VAO, VBOs[2], EBO;
	
	unsigned int numIndices;

	Mesh(aiMesh* mesh){
	GLuint VAO, VBOs[2], EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBOs);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), mesh->mVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, reinterpret_cast<void*>(0));

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), mesh->mNormals, GL_STATIC_DRAW);


	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, reinterpret_cast<void*>(0));
	
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	/* std::vector<unsigned int> indices; */

	unsigned int numIndices = mesh->mNumFaces * 3;

	for(unsigned int i = 0; i < mesh->mNumFaces; i++){
		const aiFace& face = mesh->mFaces[i];
		
		indices[(i*3)]   = face.mIndices[0];
		indices[(i*3)+1] = face.mIndices[1];
		indices[(i*3)+2] = face.mIndices[2];
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	delete[] indices;
	}
	
	Mesh& operator=(const Mesh& mesh){
		/* textures = mesh.textures; */
		VAO = mesh.VAO;
		VBO = mesh.VBO;
		EBO = mesh.EBO;
		return *this;
	}
	Mesh(const Mesh& mesh){
		*this = mesh;
	}

	~Mesh(){
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

};
