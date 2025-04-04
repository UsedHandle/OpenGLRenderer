#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <glm/glm.hpp>

#include <glad/glad.h>

#include <vector>
#include <memory>

#include "textures.h"

#define ASSIMP_LOAD_FLAGS ( aiProcess_Triangulate )

#define POSITION_LOC 0
#define NORM_LOC     1
#define TEXCOORD_LOC 2

#define TEXTURES_PER_MESH 2

#define DEFAULT_DIFF_FNAME ( "DEFAULT_DIFF_FILENAME" )
#define DEFAULT_SPEC_FNAME ( "DEFAULT_SPEC_FILENAME" )

struct Mesh {
	unsigned int diffIndex, specIndex;

	unsigned int numIndices;
	unsigned int baseIndex;
	unsigned int baseVertex;

	glm::mat4 transform;
};
#include <iostream>
#include <glm/ext/matrix_transform.hpp>

struct Model {
	GLuint VAO, VBOs[3], EBO;

	unsigned int numMeshes;
	std::unique_ptr<Mesh[]> meshes;
	
	std::vector<Texture> textures;
	
	glm::mat4 sceneTrans = glm::mat4(1.0f);
	static inline aiMatrix4x4 glmToAssimp(const glm::mat4& b){
		return aiMatrix4x4(b[0][0], b[1][0], b[2][0], b[3][0],
		                   b[0][1], b[1][1], b[2][1], b[3][1],
		                   b[0][2], b[1][2], b[2][2], b[3][2],
		                   b[0][3], b[1][3], b[2][3], b[3][3]);
	}

	static inline glm::mat4 assimpToglm(const aiMatrix4x4& b){
		return glm::mat4(b[0][0], b[1][0], b[2][0], b[3][0],
		                 b[0][1], b[1][1], b[2][1], b[3][1],
		                 b[0][2], b[1][2], b[2][2], b[3][2],
		                 b[0][3], b[1][3], b[2][3], b[3][3]);
	}
	Model(std::string filename, glm::mat4 trans = glm::mat4(1.f)){
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
			fprintf(stderr, "Assimp: %s\n", importer.GetErrorString());
			exit(EXIT_FAILURE);
		}

		numMeshes = scene->mNumMeshes;
		meshes = std::make_unique<Mesh[]>(scene->mNumMeshes);

		processNode(scene->mRootNode, glmToAssimp(trans));
		populateMeshesAndBuffers(scene);
		createTextures(scene, filename);	
	}

	~Model(){
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, VBOs);
		glDeleteBuffers(1, &EBO);
	}
	
	void processNode(aiNode* node, aiMatrix4x4 accumTrans = aiMatrix4x4());

	void populateMeshesAndBuffers(const aiScene* scene);


	inline void createTextures(const aiScene* scene, const std::string& filename){
		std::string directory = filename.substr(0, filename.find_last_of('/'));
		std::vector<std::string> loadedTextures;

		for(unsigned int i = 0; i < numMeshes; i++){
			const aiMaterial* meshMaterial =
				scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
			
			// index if texture is already loaded
			bool hasDiffuse  = meshMaterial->GetTextureCount(aiTextureType_DIFFUSE);
			bool hasSpecular = meshMaterial->GetTextureCount(aiTextureType_SPECULAR);

			if(hasDiffuse){
				loadTextureWithFile(
						aiTextureType_DIFFUSE, meshMaterial,
						directory,
						loadedTextures,
						meshes[i]);
			} else {
				unsigned char data[4] = { 192, 192, 192, 255 };
				loadTextureWithData(
						DEFAULT_DIFF_FNAME,
						data, 1, 1, GL_RGBA,
						loadedTextures,
						meshes[i].diffIndex);
			}

			if(hasSpecular){
				loadTextureWithFile(
						aiTextureType_SPECULAR, meshMaterial,
						directory,
						loadedTextures,
						meshes[i]);
			} else {
				unsigned char data[4] = { 155, 155, 155, 255};
				loadTextureWithData(
						DEFAULT_SPEC_FNAME,
						data, 1, 1, GL_RGBA,
						loadedTextures,
						meshes[i].specIndex);
			}
		}
	}


	inline void loadTextureWithFile(
			aiTextureType texType,
			const aiMaterial* meshMaterial,
			const std::string& directory,
			std::vector<std::string>& loadedTextures,
			Mesh& mesh)
	{
		aiString aiStrTextureFile;
		// bug 2754 in assimp that is related to utf-8
		// aiStrTextureFile.data has to be subtracted by 4 because (I assume) it accounts for a 4 byte length prefix that doesn't exist
		if (meshMaterial->GetTexture(texType, 0, &aiStrTextureFile, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS) {
			std::cerr << "Error loading texture from assimp: " << (char*)aiStrTextureFile.data-4 << '\n';
			std::exit(EXIT_FAILURE);
		}
		std::string textureFile = directory + "/" + (((char*)aiStrTextureFile.data)-4);
		auto arrIter =
			find(loadedTextures.begin(), loadedTextures.end(), textureFile);
		
		unsigned int index =
			static_cast<unsigned int>( arrIter - loadedTextures.begin() );

		
		if(texType == aiTextureType_DIFFUSE)
			mesh.diffIndex = index;

		if(texType == aiTextureType_SPECULAR)
			mesh.specIndex = index;

		if(index == loadedTextures.size()){
			textures.push_back( Texture(textureFile.c_str()) );
			loadedTextures.push_back(textureFile);
		}
	}


	inline void loadTextureWithData(
			const std::string& textureFile,
			unsigned char* data,
			int width, int height,
			GLenum format,
			std::vector<std::string>& loadedTextures,
			unsigned int& texIndex)
	{
		auto arrIter =
			find(loadedTextures.begin(), loadedTextures.end(), textureFile);

		unsigned int index =
			static_cast<unsigned int>( arrIter - loadedTextures.begin() );

		texIndex = index;

		if(index == loadedTextures.size()){
			textures.push_back( Texture(data, width, height, format) );
			loadedTextures.push_back(textureFile);
		}
	}



	void draw(
			GLint diffLoc,
			GLint specLoc,
			GLint objLoc,
			GLint sceneTLoc) const;
};
