#pragma once

#include <glad/glad.h>

#include <utility>

struct Texture{
	GLuint texture;

	Texture(
			const char* fileLocation,
			GLenum minCfg = GL_LINEAR_MIPMAP_LINEAR, GLenum magCfg = GL_LINEAR,
			GLenum wrapCfg = GL_REPEAT,
			bool mipmaps = true,
			bool anisotropy = false);

	Texture(
			unsigned char* data,
			int width, int height, GLenum format,
			GLenum minCfg = GL_LINEAR_MIPMAP_LINEAR, GLenum magCfg = GL_LINEAR,
			GLenum wrapCfg = GL_REPEAT,
			bool mipmaps = true,
			bool anisotropy = false);
	
	Texture(){}
	Texture(const Texture& b) = delete;
	Texture& operator=(const Texture& b) = delete;

	Texture(Texture&& b) : texture(b.texture) {
		b.texture = 0;
	}

	Texture& operator=(Texture&& b) noexcept {
		if(this != &b){
			deleteTexture();
			std::swap(texture, b.texture);
		}
		return *this;
	}

	~Texture(){
		deleteTexture();
	}

	void deleteTexture(unsigned int numTextures = 1);

	inline void textureCfg(
			GLenum minCfg, GLenum magCfg,
			GLenum wrapCfg,
			bool mipmaps,
			bool anisotropy)
	{
		if(mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapCfg);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapCfg);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minCfg);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magCfg);
		
		if(anisotropy){
			float aniso;
			//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			//glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		}
	}

	inline void active(unsigned int slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
};
