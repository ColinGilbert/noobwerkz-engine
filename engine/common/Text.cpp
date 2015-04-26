#include "Text.hpp"

bool noob::text::init(const std::string& font, int font_sz, float screen_w, float screen_h)
{
	filename = "fonts/" + font;

	font_size = font_sz;
	screen_width = screen_w;
	screen_height = screen_h;

	if(FT_Init_FreeType(&ft))
	{
		logger::log("Could not init freetype library");
		return false;
	}

	if(FT_New_Face(ft, filename.c_str(), 0, &face))
	{
		logger::log("Could not open font");
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	bool atlas_created = pack_atlas("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+[]\\{}|;':\",./<>?`~ ");
/*
	program = noob::utils::load_program(noob::utils::load_file_as_string("text.vert"), noob::utils::load_file_as_string("text.frag"), true);

	attribute_coord = uniform_tex = uniform_color = -1;

	attribute_coord = glGetAttribLocation(program, "coord");
	uniform_tex = glGetUniformLocation(program, "tex");
	uniform_color = glGetUniformLocation(program, "color");

	if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
		return false;

	glGenBuffers(1, &vbo);
*/
}

/*
   void noob::text::draw()
   {

   glUseProgram(shader);

   {
   glUniform1i(glGetUniformLocation(shader, "texture"), 0);
   glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, 0, &mvp.m[0]);
   buffer->Render(GL_TRIANGLES);
   }
   }
   */

void noob::text::resize_screen(float width, float height)
{
	//	projection = noob::ortho(0, width, 0, height, -1, 1);
	screen_height = height;
	screen_width = width;
}


void noob::text::init()
{
/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// GLuint vbo;
	// glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLfloat color[4] = {red, green, blue, alpha};
	glUniform4fv(uniform_color, 1, color);
*/
}

void noob::text::draw_text(const std::string& text, float x, float y)
{
	//	std::array<char> txt = std::array(text.c_str());
	//	size_t len = text.length();

	struct point
	{
		float x;
		float y;
		float s;
		float t;
	} coords[6 * text.length()];

	int n = 0;

	for(char c : text)
	{

		auto it = atlas.find(c);

		if (it == atlas.end())
		{
			std::stringstream ss;
			ss << "Glyph " << c << " not in atlas.";
			logger::log(ss.str());
			continue; // TODO: Change behaviour?
		}

		glyph_handle_t g = it->second;

		float x2 =  x + g.bl * screen_width;
		float y2 = -y - g.bt * screen_height;
		float w = g.bw * screen_width;
		float h = g.bh * screen_height;

		/* Advance the cursor to the start of the next character */
		x += g.ax * screen_width;
		y += g.ay * screen_height;

		/* Skip glyphs that have no pixels */
		if(!w || !h)
		{
			continue;
		}

		coords[n++] = (point){x2, -y2, g.tx, 0};
		coords[n++] = (point){x2 + w, -y2, g.tx + g.bw / atlas_width, 0};
		coords[n++] = (point){x2, -y2 - h, g.tx, g.bh / atlas_height}; // remember: each glyph occupies a different amount of vertical space
		coords[n++] = (point){x2 + w, -y2, g.tx + g.bw / atlas_width, 0};
		coords[n++] = (point){x2, -y2 - h, g.tx, g.bh / atlas_height};
		coords[n++] = (point){x2 + w, -y2 - h, g.tx + g.bw / atlas_width, g.bh / atlas_height};
	}



//	glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
//	glDrawArrays(GL_TRIANGLES, 0, n);
}

void noob::text::set_font_color(float r, float g, float b, float a)
{
	/*
	   color.red = r;
	   color.blue = b;
	   color.green = g;
	   color.alpha = a;
	   */
}

bool noob::text::pack_atlas(const std::string& letters)
{
	FT_GlyphSlot g = face->glyph;

	unsigned int w = 0;
	unsigned int h = 0;

	bool success = true;

	for (char c : letters)
	{
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::stringstream ss;
			ss << "Loading character " << c << " failed!";
			logger::log(ss.str());
			success = false;
			continue;
		}
		w += g->bitmap.width;
		h = std::max(h, g->bitmap.rows);
	}
	atlas_width = w;
	atlas_height = h;

/*	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
*/
	int x = 0;
	for(char c : letters)
	{
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::stringstream ss;
			ss << "Packing character " << c << " failed!";
			logger::log(ss.str());
			success = false;
			continue;
		}
//		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		x += g->bitmap.width;

		glyph_handle_t glyph_info;

		glyph_info.ax = g->advance.x >> 6;
		glyph_info.ax = g->advance.x >> 6;
		glyph_info.ay = g->advance.y >> 6;
		glyph_info.bw = g->bitmap.width;
		glyph_info.bh = g->bitmap.rows;
		glyph_info.bl = g->bitmap_left;
		glyph_info.bt = g->bitmap_top;
		glyph_info.tx = static_cast<float>(x) / static_cast<float>(w);

		atlas.insert(std::make_pair(c, glyph_info));
	}

	return success;
}
