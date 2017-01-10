#include "Font.hpp"

#include <iostream>
#include <fstream>


noob::font::~font() noexcept(true)
{
	if(atlas_valid)
	{
		ftgl::texture_atlas_delete(atlas);
	}

	FT_Done_FreeType(ft_lib);

	if (ft_face_valid)
	{
		FT_Done_Face(ft_face);
	}
}


bool noob::font::init_library(const std::string& Mem, const noob::vec2d Dpi, const std::string& Characters, uint16_t FontSize) noexcept(true)
{
	dpi = Dpi;
	font_size = FontSize;

	FT_Init_FreeType(&ft_lib);

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&Mem[0]), Mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font!");
		return false;
	}

	max_adv = noob::vec2f(static_cast<float>(ft_face->max_advance_width) / 64, static_cast<float>(ft_face->max_advance_height) / 64);

	if (FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to select unicode face!");
		return false;
	}

	FT_Set_Char_Size(ft_face, FontSize * 64,  FontSize * 64, Dpi[0], Dpi[1]);

	ft_face_valid = true;

	const bool glyphs_good = init_glyphs(Characters);

	if (!glyphs_good) 
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Could not init glyphs!");
		ft_face_valid = false;
		return false;
	}

	ascent = ft_face->ascender / 64;
	descent = ft_face->descender / 64;

	return true;
}


bool noob::font::init_glyphs(const std::string& Characters) noexcept(true)
{
	if(ft_face_valid)
	{
		if (!is_valid_utf8(Characters))
		{
			noob::logger::log(noob::importance::ERROR, "[Fonts] Found invalid utf8 in sequence while trying to fill atlas!");
			return false;
		}

		if (atlas_valid)
		{
			ftgl::texture_atlas_delete(atlas);
			atlas_valid = false;
		}

		// Now, we can more or less assume (hope?) that our characters are be well-formed.

		uint32_t state_arg = 0;
		uint32_t codepoint_arg;
		std::vector<uint32_t> codepoints;

		for (char c = 0; c < Characters.size(); ++c)
		{
			if (!utf8_decode_hoehrmann(&state_arg, &codepoint_arg, Characters[c]))
			{
				codepoints.push_back(codepoint_arg);
			}
		}

		const uint32_t min_dim = 64;
		const uint32_t max_dim = 2048; // TODO: Set to implementation-specific max value.

		// Preserve squareness of atlas texture...
		for (uint32_t dim = min_dim; dim < max_dim + 1; dim *= 2)
		{
			if (init_glyphs_helper(codepoints, noob::vec2ui(dim, dim)))
			{
				noob::logger::log(noob::importance::INFO, noob::concat("[Font] Creating texture atlas of size ", noob::to_string(dim), "*", noob::to_string(dim), " for ", noob::to_string(codepoints.size()), " codepoints."));
				noob::graphics& gfx = noob::get_graphics();
				gfx.texture_unpack_alignment(1);

				noob::texture_loader_2d texloader;
				texloader.from_mem_raw(noob::vec2ui(dim, dim), false, noob::pixel_format::R8, atlas->data, dim*dim);
/*
				// DEBUGGING
				std::string atlas_dbg; // = "\n";

				for(int iy = 0; iy < atlas->height; ++iy)
				{
					for(int ix = 0; ix < atlas->width; ++ix)
					{
						int c = (int)atlas->data[iy * atlas->width + ix];
						std::string temp;
						if (c == 255) temp = "#";
						else temp = "`";
						atlas_dbg = noob::concat(atlas_dbg, temp);
					}
					atlas_dbg = noob::concat(atlas_dbg, "\n");
				}
				// atlas_dbg = noob::concat(atlas_dbg, "\n");

				std::ofstream dbg_file;
				dbg_file.open ("atlasdebug.txt");
				dbg_file << atlas_dbg;
				dbg_file.close();

*/

				tex = gfx.texture_2d(texloader, false, true);

				// gfx.texture_unpack_alignment(4);


				return true;
			}
		}
		atlas_valid = true;
	}
	else
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Tried initializing glyphs with invalid freetype state. Didn't continue.");
	}
	return false;
}


bool noob::font::has_glyph(uint32_t CodePoint) const noexcept(true)
{
	const noob::fast_hashtable::cell* const test = codepoints_to_glyphs.lookup_immutable(CodePoint);
	if (codepoints_to_glyphs.is_valid(test))
	{
		return true;
	}

	return false;
}


noob::results<noob::font::shaped_text> noob::font::shape_text(const std::string& Text) noexcept(true)
{
	noob::font::shaped_text results;

	if (is_valid_utf8(Text))
	{

		uint32_t code_point;
		uint32_t state = 0;
		uint32_t counter = 0;
		std::vector<uint32_t> codepoints;
		for (auto c : Text)
		{
			if(!utf8_decode_hoehrmann(&state, &code_point, c))
			{
				if (!has_glyph(code_point) && code_point != 32)
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Could not apply shaping to text with invalid codepoint ", noob::to_string(code_point), " at position ", noob::to_string(counter)));
					return noob::results<noob::font::shaped_text>::make_invalid();
				}
				codepoints.push_back(code_point);
			}
			++counter;
		}
		// TODO: Shaping, appending, etc.
	

		noob::vec2f pen_pos = noob::vec2f(0.0, 0.0);
		noob::vec2f lower_bounds = noob::vec2f(0.0, 0.0);		
		noob::vec2f upper_bounds = noob::vec2f(0.0, 0.0);
		// noob::vec2f bounds = noob::vec2f(0.0, 0.0);
		std::string dbg_txt = "[Font] Got shaped text. Info: ";


		for (unsigned int i = 0; i < codepoints.size(); ++i)
		{
			// TODO: Get glyph and position
			const uint32_t cp = codepoints[i];

			const noob::fast_hashtable::cell* const glyph_cell = codepoints_to_glyphs.lookup_immutable(cp);
			if (cp == 32)
			{
				pen_pos += noob::vec2f(max_adv[0], 0.0);
			}
			else if (codepoints_to_glyphs.is_valid(glyph_cell))
			{
				const noob::font::glyph base_glyph = stored_glyphs[glyph_cell->value];

				const noob::vec2f dims = base_glyph.dims;
				const noob::vec2f advances = base_glyph.advances;
				// const noob::vec2f offsets = base_glyph.offsets;
				const noob::vec2f bearings = base_glyph.bearings;

				const noob::vec2f first_texcoord = noob::min(base_glyph.mapped_pos, base_glyph.mapped_pos + base_glyph.mapped_dims);
				const noob::vec2f second_texcoord = noob::max(base_glyph.mapped_pos, base_glyph.mapped_pos + base_glyph.mapped_dims);

				const noob::vec2f uv_0 = noob::vec2f(first_texcoord[0], second_texcoord[1]);
 				const noob::vec2f uv_1 = noob::vec2f(second_texcoord[0], first_texcoord[1]);

				const vec2f pos_0 = noob::vec2f(pen_pos[0] + bearings[0], std::floor(pen_pos[1] - (dims[1] - bearings[1] )));
				const vec2f pos_1 = pos_0 + dims;


				lower_bounds = noob::min(noob::min(pos_0, pos_1), lower_bounds);
				upper_bounds = noob::max(noob::max(pos_0, pos_1), upper_bounds);

				pen_pos = pen_pos + advances;//noob::vec2f(advances[0], -advances[1]);

				const noob::vec4f colour = noob::vec4f(1.0, 1.0, 1.0, 1.0); // Default colour
				// const noob::vec4f colour_dbg(0.0, 0.0, 0.0, 1.0);
				const noob::billboard_vertex vert_0 = { pos_0, uv_0, colour };
				const noob::billboard_vertex vert_1 = { noob::vec2f(pos_0[0], pos_1[1]), noob::vec2f(uv_0[0], uv_1[1]), colour };
				const noob::billboard_vertex vert_2 = { pos_1, uv_1, colour };
				const noob::billboard_vertex vert_3 = { noob::vec2f(pos_1[0], pos_0[1]), noob::vec2f(uv_1[0], uv_0[1]), colour };

				const std::array<noob::billboard_vertex, 6> quad_verts = { vert_0, vert_2, vert_1, vert_0, vert_3, vert_2 };

				results.quads.push_back(quad_verts);

				const noob::vec2f quad_dims = noob::max(pos_0, pos_1) - noob::min(pos_0, pos_1);

				// dbg_txt = noob::concat(dbg_txt, "\nCodepoint (HB/UTF8) ", noob::to_string(info_hb.codepoint), "/", noob::to_string(cp) ,", dims (", noob::to_string(quad_dims), "), uv's (", noob::to_string(uv_0), ") to (", noob::to_string(uv_1), ")");


			}
			else
			{
				noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Looked up invalid codepoint while outputting shaped text. Codepoint = ", noob::to_string(cp)));
			}

		}

		results.dims = upper_bounds - lower_bounds;

		// noob::logger::log(noob::importance::INFO, noob::concat(dbg_txt, "\n... Overall dimensions (", noob::to_string(results.dims), ")"));
	}
	else
	{
		noob::logger::log(noob::importance::WARNING, "[Font] Could not shape invalid UTF8.");

		return noob::results<noob::font::shaped_text>::make_invalid();
	}


	return noob::results<noob::font::shaped_text>(true, results);
}


bool noob::font::init_glyphs_helper(const std::vector<uint32_t>& CodePoints, const noob::vec2ui AtlasDims) noexcept(true)
{
	stored_glyphs.clear();
	codepoints_to_glyphs.clear();

	atlas = ftgl::texture_atlas_new(AtlasDims[0], AtlasDims[1], 1);

	for (uint32_t cp : CodePoints)
	{	
		const noob::fast_hashtable::cell* const test = codepoints_to_glyphs.lookup_immutable(cp);
		if (!codepoints_to_glyphs.is_valid(test)) // If the codepoint isn't in there yet
		{
			// Load character glyph, skipping ones that fail.
			if (FT_Load_Char(ft_face, cp, FT_LOAD_RENDER))
			{
				noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to load glyph for codepoint ", noob::to_string(cp)));
			}
			else
			{
				FT_GlyphSlot slot = ft_face->glyph;
		
				if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Got glyph with area zero! Codepoint = ", noob::to_string(cp), ", width/height = ", noob::to_string(slot->bitmap.width), "/", noob::to_string(slot->bitmap.rows) ));
					continue;
				}
				// DEBUGGING
				/*		
						std::string glyph_dbg = "\n";

						for(int iy = 0; iy < slot->bitmap.rows; ++iy)
						{
						for(int ix = 0; ix < slot->bitmap.width; ++ix) {
						int c = (int) slot->bitmap.buffer[iy * slot->bitmap.width + ix];
						std::string temp;
						if (c == 255) temp = "#";
						else temp = "`";
						glyph_dbg = noob::concat(glyph_dbg, temp);
						}
						glyph_dbg = noob::concat(glyph_dbg, "\n");
						}
						glyph_dbg = noob::concat(glyph_dbg, "\n");
						noob::logger::log(noob::importance::INFO, glyph_dbg);
						*/

				const ftgl::ivec4 reg = ftgl::texture_atlas_get_region(atlas, slot->bitmap.width+2, slot->bitmap.rows+2); // One-bit padding for all sides

				if (reg.height > 0) // If our atlas hasn't run out of space
				{
					// glyph(const noob::vec2f GlyphDims, const noob::vec2f Advances, const noob::vec2f Bearings, const noob::vec2f Offsets, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims)
					
					const auto dims = noob::vec2f(slot->bitmap.width, slot->bitmap.rows);
					const auto advances = noob::vec2f(slot->advance.x, slot->advance.y) / 64.0;
					const auto bearings = noob::vec2f(slot->metrics.horiBearingX, slot->metrics.horiBearingY) / 64.0;
					//const auto offsets = noob::vec2f(slot->bitmap_left, slot->bitmap_top);
					const auto atlas_pos = noob::vec2ui(reg.x+1, reg.y+1);

					ftgl::texture_atlas_set_region(atlas, atlas_pos[0], atlas_pos[1], slot->bitmap.width, slot->bitmap.rows, slot->bitmap.buffer, slot->bitmap.pitch);



					stored_glyphs.push_back(noob::font::glyph(dims, advances, bearings, atlas_pos, AtlasDims));
					
					noob::fast_hashtable::cell* incell = codepoints_to_glyphs.insert(cp);
					incell->value = stored_glyphs.size() - 1;
				}
				else
				{
					stored_glyphs.clear();
					codepoints_to_glyphs.clear();
					ftgl::texture_atlas_delete(atlas);
					atlas_valid = false;
					return false;
				}
			}
		}
	}
	atlas_valid = true;
	return true;
}
