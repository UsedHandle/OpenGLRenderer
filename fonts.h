#pragma once

#include <cmath>          // calculating approximate texture width & length
#include <unordered_map>  // for storing glyphs can fetching them
#include <stdexcept>      // catching errors from glyphs (map of the glyphs)

#include <glad/glad.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "shaders.h"

namespace ftr{
struct Glyph{
	float         botluvx,botluvy;  // bottom left uv coords in texture atlas
	float         topruvx,topruvy;  // top right uv coords in texture atlas
	unsigned int  width,height;     // size
	int           xMin, yMin;       // bottom left coords of glyph
	long          advance;          // x advance
};

struct Font{
	GLuint         texatl;
	GLuint         shader;
	GLuint         VAO, VBO;

	std::unordered_map<unsigned long, Glyph> glyphs;

	long linegap;
	
	long ascender;

	Font(const char*  fontfile, 
	     unsigned int pt,
	     unsigned long maxCharCode = 0);
	
	
	void draw2DText(float screenw, float screenh,
			            const std::wstring text,
	                float pen_x, float pen_y,
									bool  iscentered = false,
	                float r=1.0f, float g=1.0f, float b=1.0f);


	inline void done(){
		glDeleteTextures(1, &texatl);
		glDeleteShader(shader);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	inline ~Font(){ done(); }
};
}
