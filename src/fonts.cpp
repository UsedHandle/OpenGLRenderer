#include "fonts.h"

#include <memory>

ftr::Font::Font(const char*  fontfile, 
                unsigned int pt,
                unsigned long maxCharCode)
{
	FT_Library ft;
	if(FT_Init_FreeType(&ft)){	
		fprintf(stderr, "Could not init freetype\n");
		exit(EXIT_FAILURE);
	}

	FT_Face face;
	if(FT_New_Face(ft, fontfile, 0, &face)){
		fprintf(stderr, "Could not load font\n");
		exit(EXIT_FAILURE);
	}
	
	// 96 is dpi resolution
	// 72 is also standard
	FT_Set_Char_Size(face, 0, pt<<6, 96, 96);
	if(FT_Load_Char(face, ' ', FT_LOAD_RENDER)){
		fprintf(stderr, "Could not load glyph\n");
		exit(EXIT_FAILURE);
	}


	unsigned long num_glyphs{};
	// face->num_glyphs is long but FT_Load_Char takes
	// unsigned longs
	if(maxCharCode == 0){
		num_glyphs = static_cast<unsigned long>( face->num_glyphs );
	} else{
		// count number of glyphs with char code smaller than
		// maxCharCode
		unsigned long charcode;
		unsigned int i;

		charcode = FT_Get_First_Char( face, &i );
		while(i != 0 && charcode <= maxCharCode){
			num_glyphs++;
			
			charcode = FT_Get_Next_Char(face, charcode, &i);
		}
	}


	std::size_t texatl_width = 
		// face->size->metrics.height is the max glyph size * 64
		static_cast<size_t>(face->size->metrics.height >> 6) * 
		static_cast<size_t>(1.0 + std::floor(std::sqrt(num_glyphs)));

	// is cropped next if texatl_height is too large
	std::size_t texatl_height = texatl_width;

	unsigned int pen_x{};
	unsigned int pen_y{};

	{
		unsigned long charcode;
		unsigned int i;

		charcode = FT_Get_First_Char(face, &i);

		// cuts off extra space
		while(i != 0){
			if(maxCharCode != 0 && charcode > maxCharCode)
				break;

			FT_Load_Char(face, charcode, FT_LOAD_RENDER);

			// short cut
			const FT_GlyphSlot glyph = face->glyph;
			const FT_Bitmap* bitmap = &glyph->bitmap;

			if(pen_x + bitmap->width >= texatl_width){
				pen_x = 0;
				pen_y += ((face->size->metrics.height >> 6) + 1);
			}
		

			pen_x += bitmap->width + 1;
			charcode = FT_Get_Next_Char(face, charcode, &i);

			// if the glyph is the last of the face
			if(i == 0 || charcode > maxCharCode)
				texatl_height = 
					pen_y + 
					static_cast<size_t>(face->size->metrics.height>>6) + 1;

		}
	}

	// reset pen coords
	pen_x = 0;
	pen_y = 0;

	// allocate buffer for texture atlas
	std::unique_ptr<unsigned char[]> texatl_pixels = 
		std::make_unique<unsigned char[]>(texatl_width * texatl_height);	

	unsigned long charcode;
	unsigned int i;
	charcode = FT_Get_First_Char(face, &i);

	// store data of every glyph
	while(i != 0){
		// if maxCharCode is specified and current character
		// exceeds the maxCharCode, break;
		if(maxCharCode != 0 && charcode > maxCharCode)
			break;
		

		// renders to bitmap with FT_LOAD_RENDER
		FT_Load_Char(face, charcode, FT_LOAD_RENDER);
		
		// short cut
		const FT_GlyphSlot glyph = face->glyph;
		const FT_Bitmap* bitmap = &glyph->bitmap;

		if(pen_x + bitmap->width >= texatl_width){
			pen_x = 0;
			pen_y += (face->size->metrics.height >> 6) + 1;
		}


		// copy pixels of the glyph to the texture atlas
		for(unsigned int row = 0; row < bitmap->rows; row++){
			for(unsigned int col = 0; col < bitmap->width; col++){
				unsigned int x = pen_x + col;
				unsigned int y = pen_y + row;
				texatl_pixels[y*texatl_width + x] = bitmap->buffer[row*bitmap->width + col];
			}
		}
		


		
		float botluvx, botluvy;
		botluvx = static_cast<float>(pen_x);
		botluvy = static_cast<float>(pen_y);

		float topruvx, topruvy;
		topruvx = static_cast<float>(pen_x + bitmap->width);
		topruvy = static_cast<float>(pen_y + bitmap->rows);
	
		// 0 to 1 texture coords
		const float normfactorx = 1.0f/static_cast<float>(texatl_width);
		const float normfactory = 1.0f/static_cast<float>(texatl_height);

		botluvx *= normfactorx; botluvy *= normfactory;
		topruvx *= normfactorx; topruvy *= normfactory;
		
		const int xMin = glyph->bitmap_left;
		const int yMin = -( static_cast<int>(bitmap->rows) - glyph->bitmap_top );

		// glyph {
		//	bottom left uv choord in texture atlas (x, y),
		//	size,
		//	bottom left coords of glyph (x, y)
		//	x advance
		// }
	
		glyphs[charcode] = {
			botluvx, botluvy,
			topruvx, topruvy,
			bitmap->width, bitmap->rows,
			xMin, yMin,
			(face->glyph->advance.x >> 6)
		};

		pen_x += bitmap->width + 1;
		charcode = FT_Get_Next_Char(face, charcode, &i);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	glGenTextures(1, &texatl);
	glBindTexture(GL_TEXTURE_2D, texatl);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
							 texatl_width,
							 texatl_height,
							 0, 
							 GL_RED,
							 GL_UNSIGNED_BYTE,
							 &texatl_pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	ascender = face->size->metrics.ascender >> 6;
	linegap = face->size->metrics.height >> 6;

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	constexpr const char* vssrc = 
		"#version 330 core\n"
		"layout (location = 0) in vec2 vertex;\n"
		"layout (location = 1) in vec2 texcoorduv;\n"

		"out vec2 TexCoord;\n"

		"uniform vec2 resolution;\n"

		"void main(){\n"
		"vec2 normVertCoords = vertex/resolution * 2.0 - 1.0;\n"
		"	gl_Position = vec4(normVertCoords, 0.0, 1.0);\n"
		"	TexCoord = texcoorduv;\n"
		"}\0";

	constexpr const char* fssrc =
		"#version 330 core\n"
		"out vec4 FragColor;\n"

		"in vec2 TexCoord;\n"
		
		"uniform vec3 color;\n"

		"uniform sampler2D font;\n"
		"void main(){\n"
		"	FragColor = texture(font, TexCoord).r * vec4(color.r, color.g, color.b, 1.0f);\n"
		"}\0";

	shader = shaderWithSource(vssrc, fssrc);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// maximum of 4000 characters
	// 24 is amount of floats per character
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24*4000, nullptr, GL_STREAM_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 
												4*sizeof(float),
												reinterpret_cast<void*>( 2*sizeof(float) ));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Beware of the static casts
void ftr::Font::draw2DText(float screenw, float screenh,
                           const std::wstring text,
                           float pen_x, float pen_y,
                           bool  iscentered,
                           float r, float g, float b)
{
	// if the coords are of the center of the text
	// compute length and width of the text and make adjustments to
	// pen_x and pen_y
	if(iscentered){
		float width = pen_x;
		float height = pen_y;

		float maxx = width;

		for(unsigned long i = 0; i < text.length(); i++){
			const wchar_t& character = text[i]; 
			const unsigned long charindex = static_cast<unsigned long>(character);

		
			Glyph glyph;
			try{
				glyph = glyphs.at(charindex);
			}
			catch(std::out_of_range e){
				fprintf(stderr, "Error: Drawing char that has not be loaded\n");
				exit(EXIT_FAILURE);
			}

			const float w = static_cast<float>(glyph.width);

			if(width + w >= screenw){
				width = 0;
				height -= static_cast<float>(linegap);
			}
			
			width += static_cast<float>(glyph.advance);

			if(width > maxx) maxx = width;
		}
		maxx = maxx - pen_x;
		height = height - pen_y;

		pen_x -= maxx/2.0f;
		pen_y -= height/2.0f;

		pen_y -= static_cast<float>(ascender)/2.0f;
	} else{
		// text coords are specified by top left coord of text box
		// ascender is subracted to get the bottom left coord
		pen_y -= static_cast<float>(ascender);
	}


	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader, "font"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texatl);

	glUniform2f(glGetUniformLocation(shader, "resolution"), screenw, screenh);

	glUniform3f(glGetUniformLocation(shader, "color"), r, g, b);	


	/* 
	 * Pointers to pointers for arr[][] do not work because opengl
	 * takes a collection of floats together in memory. pointers to
	 * pointers make groups (length = d) of floats that can be in
	 * entirely different places in the heap
	*/

	std::unique_ptr<float[]> textvertices = std::make_unique<float[]>(6*text.length() * 4);
	

	for(unsigned long i = 0; i < text.length(); i++){
		const wchar_t& character = text[i]; 
		const unsigned long charindex = static_cast<unsigned long>(character);


		Glyph glyph;
		try{
			glyph = glyphs.at(charindex);
		}
		catch(std::out_of_range e){
			fprintf(stderr, "Error: Drawing char that has not be loaded\n");
			exit(EXIT_FAILURE);
		}

		const float w = static_cast<float>(glyph.width);
		const float h = static_cast<float>(glyph.height);

		if(pen_x + w >= screenw){
			pen_x = 0;
			pen_y -= static_cast<float>(linegap);
		}

		const float botlx = pen_x + static_cast<float>(glyph.xMin);
		const float botly = pen_y + static_cast<float>(glyph.yMin);
		
		const float toprx = botlx + w;
		const float topry = botly + h;
	

		const float botluvx = glyph.botluvx;
		const float botluvy = glyph.botluvy;
		
		const float topruvx = glyph.topruvx;
		const float topruvy = glyph.topruvy;


		float glyphvertices[6][4]{
			{ botlx, topry, botluvx, botluvy },
			{ botlx, botly, botluvx, topruvy },
			{ toprx, botly, topruvx, topruvy },

			{ botlx, topry, botluvx, botluvy },
			{ toprx, botly, topruvx, topruvy },
			{ toprx, topry, topruvx, botluvy }
		};
		
		for(unsigned short row = 0; row < 6; row++)
			for(unsigned short col = 0; col < 4; col++)
				textvertices[(4*((i*6) + row)) + col] = glyphvertices[row][col];

	
		pen_x += static_cast<float>(glyph.advance);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*24*text.length(), &textvertices[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(GL_TRIANGLES, 0, 6 * text.length());
	glDisable(GL_BLEND);

	glBindVertexArray(0);
	
	glUseProgram(0);
}
