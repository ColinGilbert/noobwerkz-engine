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


bool noob::font::init_library(const std::string& Mem, const noob::vec2d Dpi) noexcept(true)
{
	dpi = Dpi;

	FT_Init_FreeType(&ft_lib);

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&Mem[0]), Mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font!");
		return false;
	}

	hb_font = hb_ft_font_create(ft_face, nullptr);
	hb_buf = hb_buffer_create();

	assert(hb_buffer_allocation_successful(buffer));

	ft_face_valid = true;

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
				noob::texture_loader_2d texloader;
				texloader.from_mem_raw(noob::vec2ui(dim, dim), false, noob::pixel_format::R8, atlas->data, dim*dim);
				tex = gfx.texture_2d(texloader, false);
				return true;
			}
		}
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


bool noob::font::shape_text(const noob::font::text& Text) noexcept(true)
{
	if (is_valid_utf8(Text.data))
	{
		hb_buffer_reset(hb_buf);

		hb_buffer_set_direction(hb_buf, Text.get_dir_hb());
		hb_buffer_set_script(hb_buf, Text.get_script_hb());
		hb_buffer_set_language(hb_buf, Text.get_language_hb());

		uint32_t code_point;
		uint32_t state = 0;
		for (auto c : Text.data)
		{
			if(!utf8_decode_hoehrmann(&state, &code_point, c))
			{
				if (!has_glyph(code_point))
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Could not shape text containing invalid codepoint ", noob::to_string(code_point)));
					return false;
				}
			}
		}

		hb_buffer_add_utf8(hb_buf, Text.data.c_str(), Text.data.size(), 0, Text.data.size());

		// Harfbuzz shaping
		hb_shape(hb_font, hb_buf, features.empty() ? nullptr : &features[0], features.size());

		unsigned int glyph_count;
		hb_glyph_info_t* glyph_infos = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
		hb_glyph_position_t* glyph_positions = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);
	}
	else
	{
		noob::logger::log(noob::importance::WARNING, "[Font] Could not shape invalid UTF8");
		return false;
	}

	return true;
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
			if (FT_Load_Char(ft_face, cp, FT_LOAD_DEFAULT))
			{
				noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to load glyph for codepoint ", noob::to_string(cp)));
			}
			else
			{
				FT_GlyphSlot slot = ft_face->glyph;
				FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

				if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Got glyph with area zero! Codepoint = ", noob::to_string(cp), ", width/height = ", noob::to_string(slot->bitmap.width), "/", noob::to_string(slot->bitmap.rows) ));
					break;
				}

				const ftgl::ivec4 reg = ftgl::texture_atlas_get_region(atlas, slot->bitmap.width, slot->bitmap.rows);

				if (reg.height > 0) // If our atlas hasn't run out of space
				{
					texture_atlas_set_region(atlas, reg.x, reg.y, reg.width, reg.height, slot->bitmap.buffer, 1);
					stored_glyphs.push_back(noob::font::glyph(noob::vec2ui(reg.width, reg.height), noob::vec2ui(reg.x, reg.y), noob::vec2ui(AtlasDims[0], AtlasDims[1])));
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
