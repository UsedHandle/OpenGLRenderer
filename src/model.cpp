#include "model.h"

void Model::processNode(aiNode *node, aiMatrix4x4 accumTrans){
	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		Mesh& outDataMesh = meshes[ node->mMeshes[i] ];
		outDataMesh.transform = assimpToglm(accumTrans);
	}
	accumTrans = node->mTransformation * accumTrans;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], accumTrans);

}

void Model::populateMeshesAndBuffers(const aiScene *scene)
{
	// copy data into mesh array and collect info
	unsigned int totalNumVertices = 0;
	unsigned int totalNumIndices  = 0;
	for(unsigned int i = 0; i < scene->mNumMeshes; i++){
		meshes[i].baseVertex = totalNumVertices;
		meshes[i].baseIndex  = totalNumIndices;
		meshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		
		totalNumVertices += scene->mMeshes[i]->mNumVertices;
		totalNumIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	// uv array is made because when there
	// is no uv coords, (0.0, 0.0) is assumed
	// the to be the coordinates
	std::unique_ptr<float[]> uvs      =
		std::make_unique<float[]>(totalNumVertices*2);

	
	// indices array is made because it is
	// not possible to copy directly from
	// scene->mMeshes[i]->mFaces as it
	// contains a pointer to a few indices 
	// (likely 3) and number of indices
	std::unique_ptr<unsigned int[]> indices =
		std::make_unique<unsigned int[]>(totalNumIndices);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBOs);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	
	// create positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[POSITION_LOC]);
	glBufferData(GL_ARRAY_BUFFER, totalNumVertices*3*sizeof(float),
			nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION_LOC);
	glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE,
			sizeof(float)*3, nullptr);

	
	// create normals buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[NORM_LOC]);
	glBufferData(GL_ARRAY_BUFFER, totalNumVertices*3*sizeof(float),
			nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(NORM_LOC);
	glVertexAttribPointer(NORM_LOC, 3, GL_FLOAT, GL_FALSE,
			sizeof(float)*3, nullptr);



	glBindBuffer(GL_ARRAY_BUFFER, VBOs[TEXCOORD_LOC]);
	glBufferData(GL_ARRAY_BUFFER, totalNumVertices*3*sizeof(float),
			nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(TEXCOORD_LOC);
	glVertexAttribPointer(TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE,
			sizeof(float)*2, nullptr);

	// copy data into buffers
	for(unsigned int i = 0; i < scene->mNumMeshes; i++){
		// glBufferSubData(target, offsetInBytes, sizeOfData, data)
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[POSITION_LOC]);
		glBufferSubData(GL_ARRAY_BUFFER,
			meshes[i].baseVertex*3*sizeof(float),
			scene->mMeshes[i]->mNumVertices*3*sizeof(float),
			scene->mMeshes[i]->mVertices);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[NORM_LOC]);
		glBufferSubData(GL_ARRAY_BUFFER,
				meshes[i].baseVertex*3*sizeof(float),
				scene->mMeshes[i]->mNumVertices*3*sizeof(float),
				scene->mMeshes[i]->mNormals);

		// copy texture coords
		// scene->mMeshes[i]->mTextureCoords[0] is 3D vector
		for(unsigned int verti = 0; verti < scene->mMeshes[i]->mNumVertices; verti++){
			if(scene->mMeshes[i]->mTextureCoords[0] != nullptr){
				memcpy(&uvs[(verti+meshes[i].baseVertex)*2],
							 &scene->mMeshes[i]->mTextureCoords[0][verti],
							 2*sizeof(float));
			} else {
				uvs[(verti+meshes[i].baseVertex)] = 0.0f;
				uvs[(verti+meshes[i].baseVertex)] = 0.0f;
			}
		}

		// copy indices
		for(unsigned int facei = 0; facei < scene->mMeshes[i]->mNumFaces; facei++){
			const aiFace& face = scene->mMeshes[i]->mFaces[facei];
			memcpy(&indices[(facei*3) + meshes[i].baseIndex],
						 face.mIndices,
						 sizeof(float)*3);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[TEXCOORD_LOC]);
	glBufferData(GL_ARRAY_BUFFER, totalNumVertices*2*sizeof(float),
			&uvs[0], GL_STATIC_DRAW);
	



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalNumIndices*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Model::draw(
		GLint diffLoc,
		GLint specLoc,
		GLint objLoc,
		GLint sceneTLoc) const
{
	glBindVertexArray(VAO);
	for(unsigned int i = 0; i < numMeshes; i++){
		textures[ meshes[i].diffIndex ].active(0);
		textures[ meshes[i].specIndex ].active(1);
		glUniform1i(diffLoc, 0);
		glUniform1i(specLoc, 1);

		glUniformMatrix4fv(objLoc, 1, GL_FALSE, &meshes[i].transform[0][0]);
		glUniformMatrix4fv(sceneTLoc, 1, GL_FALSE, &sceneTrans[0][0]);

		glDrawElementsBaseVertex(GL_TRIANGLES, 
				meshes[i].numIndices,
				GL_UNSIGNED_INT,
				reinterpret_cast<void*>( meshes[i].baseIndex * sizeof(unsigned int) ),
				meshes[i].baseVertex);
	}

	glBindVertexArray(0);
}
