#include "Font.hpp"

/////////////////////////////////////////////////////////
// Static helpers we wished to keep out of public sight
/////////////////////////////////////////////////////////

static const hb_tag_t KernTag = HB_TAG('k', 'e', 'r', 'n'); // Kerning operations.
static const hb_tag_t LigaTag = HB_TAG('l', 'i', 'g', 'a'); // Standard ligature substitution.
static const hb_tag_t CligTag = HB_TAG('c', 'l', 'i', 'g'); // Contextual ligature substitution.

static hb_feature_t kerning(bool Val)
{
	const hb_feature_t results = { KernTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
	return results;
}


static hb_feature_t ligature(bool Val)
{
	const hb_feature_t results = { LigaTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
	return results;
}


static hb_feature_t ligature_contextual(bool Val)
{
	const hb_feature_t results = { CligTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
	return results;	
}


static noob::font::direction direction_from_hb(hb_direction_t Dir)
{
	noob::font::direction results;

	switch(Dir)
	{
		case (HB_DIRECTION_INVALID):
		case (HB_DIRECTION_LTR):
			{
				results = noob::font::direction::LTR;
				break;
			}
		case (HB_DIRECTION_RTL):
			{
				results = noob::font::direction::RTL;
				break;
			}
		case (HB_DIRECTION_TTB):
			{
				results = noob::font::direction::TTB;
				break;
			}
		case (HB_DIRECTION_BTT):
			{
				results = noob::font::direction::BTT;
				break;
			}
	}

	return results;
}


static hb_direction_t direction_to_hb(noob::font::direction Dir)
{
	hb_direction_t results;

	switch(Dir)
	{
		case (noob::font::direction::LTR):
			{
				results = HB_DIRECTION_LTR;
				break;
			}
		case (noob::font::direction::RTL):
			{
				results = HB_DIRECTION_RTL;
				break;
			}
		case (noob::font::direction::TTB):
			{
				results = HB_DIRECTION_TTB;
				break;
			}
		case (noob::font::direction::BTT):
			{
				results = HB_DIRECTION_BTT;
				break;
			}
	}

	return results;
}


//////////////////////////////////////////////////////////////
// This is where the implementation of the public API begins:
//////////////////////////////////////////////////////////////
bool noob::font::text::set_language(const std::string& Language) noexcept(true)
{
	hb_language_t temp = hb_language_from_string (Language.c_str(), Language.size());
	if (temp == HB_LANGUAGE_INVALID)
	{
		return false;
	}

	language = temp;
	return true;

}


bool noob::font::text::set_script(const std::string& Script) noexcept(true)
{
	hb_script_t temp = hb_script_from_string(Script.c_str(), Script.size());
	if (temp == HB_SCRIPT_INVALID)
	{
		return false;
	}

	script = temp;
	return true;
}


bool noob::font::text::set_script(uint32_t Tag) noexcept(true)
{
	hb_script_t temp = hb_script_from_iso15924_tag(Tag);
	if (temp == HB_SCRIPT_INVALID)
	{
		return false;
	}

	script = temp;
	return true;
}


std::string noob::font::text::get_language() const noexcept(true)
{
	return std::string(hb_language_to_string(language));
}


uint32_t noob::font::text::get_script() const noexcept(true)
{
	return hb_script_to_iso15924_tag(script);

}


noob::font::direction noob::font::text::get_dir() const noexcept(true)
{
	return direction_from_hb(dir);
}


void noob::font::text::set_dir(noob::font::direction Dir) noexcept(true)
{
	dir = direction_to_hb(Dir);
}


// We'd otherwise put this in the protected segment, but sometimes users may wish to override this behaviour themselves.
// noob::font:direction noob::font::text::dir;

// hb_language_t noob::font::text::language;
// hb_script_t noob::font::text::script;


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
		hb_font_destroy(hb_font);
		hb_buffer_destroy(hb_buf);		
	}
}


bool noob::font::init_library(const std::string& Mem, const noob::vec2d Dpi, const std::string& Characters, uint16_t FontSize) noexcept(true)
{
	dpi = Dpi;

	FT_Init_FreeType(&ft_lib);

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&Mem[0]), Mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font!");
		return false;
	}

	max_adv = noob::vec2f(static_cast<float>(ft_face->max_advance_width), static_cast<float>(ft_face->max_advance_height));

	if (FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to select unicode face!");
		return false;
	}

	FT_Set_Char_Size(ft_face, FontSize * 64, 0, Dpi[0], Dpi[1]);

	ft_face_valid = true;

	const bool glyphs_good = init_glyphs(Characters, FontSize);

	if (!glyphs_good) 
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Could not init glyphs!");
		ft_face_valid = false;
		return false;
	}


	hb_font = hb_ft_font_create(ft_face, nullptr);
	hb_buf = hb_buffer_create();

	assert(hb_buffer_allocation_successful(buffer));


	return true;
}


bool noob::font::init_glyphs(const std::string& Characters, uint16_t FontSize) noexcept(true)
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

		// FT_GlyphSlot slot = ft_face->glyph;
		if (FT_Set_Char_Size(ft_face,  FontSize * 64, FontSize * 64, static_cast<uint32_t>(dpi[0]), static_cast<uint32_t>(dpi[1])))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to set font size!"));
			return false;
		}

		font_size = FontSize;

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


				// gfx.texture_min_filter(noob::tex_min_filter::LINEAR);
				// gfx.texture_mag_filter(noob::tex_mag_filter::LINEAR);

				// std::array<noob::tex_wrap_mode, 2> modes;
				// modes[0] = modes[1] = noob::tex_wrap_mode::CLAMP_TO_EDGE;

				// gfx.texture_wrap_mode(modes);

				tex = gfx.texture_2d(texloader, false);

				// gfx.texture_unpack_alignment(4);
				// modes[0] = modes[1] = noob::tex_wrap_mode::REPEAT;
				// gfx.texture_wrap_mode(modes);
				// gfx.texture_min_filter(noob::tex_min_filter::LINEAR_MIPMAP_NEAREST);

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


void noob::font::add_feature(noob::font::feature Feature, bool Enable) noexcept(true)
{
	switch(Feature)
	{
		case(noob::font::feature::KERNING):
			features.push_back(kerning(Enable));
			break;
		case (noob::font::feature::LIGATURE):
			features.push_back(ligature(Enable));
			break;
		case (noob::font::feature::CONTEXTUAL_LIGATURE):
			features.push_back(ligature_contextual(Enable));
			break;
	};
}


noob::results<noob::font::shaped_text> noob::font::shape_text(const noob::font::text& Text) noexcept(true)
{
	noob::font::shaped_text results;

	if (is_valid_utf8(Text.data))
	{
		hb_buffer_reset(hb_buf);

		hb_buffer_set_direction(hb_buf, Text.get_dir_hb());
		hb_buffer_set_script(hb_buf, Text.get_script_hb());
		hb_buffer_set_language(hb_buf, Text.get_language_hb());

		uint32_t code_point;
		uint32_t state = 0;
		uint32_t counter = 0;
		std::vector<uint32_t> codepoints;
		for (auto c : Text.data)
		{
			if(!utf8_decode_hoehrmann(&state, &code_point, c))
			{
				if (!has_glyph(code_point))
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Could not apply shaping to text with invalid codepoint ", noob::to_string(code_point), " at position ", noob::to_string(counter)));
					return noob::results<noob::font::shaped_text>::make_invalid();
				}
				codepoints.push_back(code_point);
			}
			++counter;
		}

		hb_buffer_add_codepoints(hb_buf, &codepoints[0], codepoints.size(), 0, codepoints.size());

		// Harfbuzz shaping
		hb_shape(hb_font, hb_buf, features.empty() ? nullptr : &features[0], features.size());

		unsigned int glyph_count;
		hb_glyph_info_t* glyph_infos = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
		hb_glyph_position_t* glyph_positions = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);

		// Debug text output.

		noob::vec2f pen_pos = noob::vec2f(0.0, 0.0);
		noob::vec2f lower_bounds = noob::vec2f(0.0, 0.0);		
		noob::vec2f upper_bounds = noob::vec2f(0.0, 0.0);
		// noob::vec2f bounds = noob::vec2f(0.0, 0.0);
		std::string dbg_txt = "[Font] Got shaped text. Info: ";


		for (unsigned int i = 0; i < glyph_count; ++i)
		{
			const hb_glyph_info_t info_hb = glyph_infos[i];
			const hb_glyph_position_t pos_hb = glyph_positions[i];

			const uint32_t cp = codepoints[i];

			const noob::fast_hashtable::cell* const glyph_cell = codepoints_to_glyphs.lookup_immutable(cp);
			if (cp == 32)
			{
				pen_pos += max_adv;
			}
			else if (codepoints_to_glyphs.is_valid(glyph_cell))
			{
				const noob::font::glyph base_glyph = stored_glyphs[glyph_cell->value];

				const noob::vec2f advances = noob::vec2f(noob::div_fp(pos_hb.x_advance, 64), noob::div_fp(pos_hb.y_advance, 64));
				const noob::vec2f offsets = noob::vec2f(noob::div_fp(pos_hb.x_offset, 64), noob::div_fp(pos_hb.y_offset, 64));

				const noob::vec2f uv_0 = base_glyph.mapped_pos;
				const noob::vec2f uv_1 = base_glyph.mapped_pos + base_glyph.mapped_dims;

				const vec2f pos_0 = noob::vec2f(pen_pos[0] + offsets[0] + base_glyph.bearings[0], std::floor(pen_pos[1] + offsets[1] + base_glyph.bearings[1]));
				const vec2f pos_1 = noob::vec2f(pos_0[0] + base_glyph.dims[0], std::floor(pos_0[1] + base_glyph.dims[1]));

				lower_bounds = noob::min(noob::min(pos_0, pos_1), lower_bounds);
				upper_bounds = noob::max(noob::max(pos_0, pos_1), upper_bounds);

				pen_pos += advances;

				const noob::vec4f colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);

				const noob::billboard_vertex vert_0 = { pos_0, uv_0, colour };
				const noob::billboard_vertex vert_1 = { noob::vec2f(pos_0[0], pos_1[1]), noob::vec2f(uv_0[0], uv_1[1]), colour };
				const noob::billboard_vertex vert_2 = { pos_1, uv_1, colour };
				const noob::billboard_vertex vert_3 = { noob::vec2f(pos_1[0], pos_0[1]), noob::vec2f(uv_1[0], uv_0[1]), colour };

				const std::array<noob::billboard_vertex, 6> quad_verts = { vert_0, vert_1, vert_2, vert_0, vert_2, vert_3 };

				results.quads.push_back(quad_verts);

				const noob::vec2f quad_dims = noob::max(pos_0, pos_1) - noob::min(pos_0, pos_1);

				// dbg_txt = noob::concat(dbg_txt, "\nCodepoint (HB/UTF8) ", noob::to_string(info_hb.codepoint), "/", noob::to_string(cp) ,", dims (", noob::to_string(quad_dims), "), uv's (", noob::to_string(uv_0), ") to (", noob::to_string(uv_1), ")");


			}
			else
			{
				noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Looked up invalid codepoint while outputting shaped text. Codepoint = ", noob::to_string(info_hb.codepoint)));
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
				// FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
/*				FT_Glyph glyph_local;
				if (FT_Get_Glyph(slot, &glyph_local))
				{
					noob::logger::log(noob::importance::ERROR, "[Font] could not get glyph for slot");
					continue;
				}

				if (FT_Glyph_To_Bitmap(&glyph_local, FT_RENDER_MODE_NORMAL, 0, 1))
				{
					noob::logger::log(noob::importance::ERROR, "[Font] could not convert glyph to bitmap.");
					continue;

				}

				FT_Done_Glyph(glyph_local);
*/

				if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Got glyph with area zero! Codepoint = ", noob::to_string(cp), ", width/height = ", noob::to_string(slot->bitmap.width), "/", noob::to_string(slot->bitmap.rows) ));
					continue;
				}
				// DEBUGGING
		
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


				const ftgl::ivec4 reg = ftgl::texture_atlas_get_region(atlas, slot->bitmap.width, slot->bitmap.rows);

				if (reg.height > 0) // If our atlas hasn't run out of space
				{
					texture_atlas_set_region(atlas, reg.x, reg.y, reg.width, reg.height, slot->bitmap.buffer, atlas->depth);
					// C'tor signature: glyph(const noob::vec2ui GlyphDims, const noob::vec2ui Bearings, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) :
					// TODO: Replace with non-c'tored version? Current version enforces good use.
					stored_glyphs.push_back(noob::font::glyph(noob::vec2ui(reg.width, reg.height), noob::vec2ui(slot->bitmap_left, slot->bitmap_top), noob::vec2ui(reg.x, reg.y), AtlasDims));
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
