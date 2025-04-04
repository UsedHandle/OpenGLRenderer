#include "textures.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "stb_image.h"
#pragma clang diagnostic pop

#else
#include "stb_image.h"
#endif

Texture::Texture(
		const char* fileLocation,
		GLenum minCfg, GLenum magCfg,
		GLenum wrapCfg,
		bool mipmaps,
		bool anisotropy)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nChannels;
	unsigned char* data = stbi_load(fileLocation, &width, &height, &nChannels, 0);
	if (!data) {
		std::cerr << "Could not get texture at: " << fileLocation << '\n';
		std::exit(EXIT_FAILURE);
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLenum format = GL_RGB;
	GLenum internalFormat = GL_RGB8;
	switch(nChannels){
		case 1:
			format = GL_RED;
			internalFormat = GL_R8;
			break;
		
		case 2:
			format = GL_RG;
			internalFormat = GL_RG8;
			break;
		
		case 3:
			format = GL_RGB;
			internalFormat = GL_RGB8;
			break;

		case 4:
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
	}
	

	glTexImage2D(
			GL_TEXTURE_2D, 0, internalFormat,
			width, height, 0, format, GL_UNSIGNED_BYTE, data);


	textureCfg(minCfg, magCfg, wrapCfg, mipmaps, anisotropy);
	stbi_image_free(data);
}

Texture::Texture(
			unsigned char* data,
			int width, int height, GLenum format,
			GLenum minCfg, GLenum magCfg,
			GLenum wrapCfg,
			bool mipmaps,
			bool anisotropy)
{
	GLenum internalFormat = GL_RGB8;
	switch(internalFormat){
		case GL_RED:
			internalFormat = GL_R8;
			break;
		
		case GL_RG:
			internalFormat = GL_RG8;
			break;
		
		case GL_RGB:
			internalFormat = GL_RGB8;
			break;

		case GL_RGBA:
			internalFormat = GL_RGBA8;
			break;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			width, height, 0, format, GL_UNSIGNED_BYTE, data);

	textureCfg(minCfg, magCfg, wrapCfg, mipmaps, anisotropy);
}

// If textures is an array
void Texture::deleteTexture(unsigned int numTextures){
		glDeleteTextures(numTextures, &texture);
}
