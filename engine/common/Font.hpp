#pragma once

// std
#include <string>
#include <vector>

// External libs
#include <texture-atlas.h>
#include <vec234.h>
#include <noob/math/math_funcs.hpp>
#include <noob/fast_hashtable/fast_hashtable.hpp>

#include <hb.h>
#include <hb-ft.h>
// Freetype-specific
//#include <ft2build.h>
//#include FT_FREETYPE_H  


#include "Texture.hpp"
#include "NoobUtils.hpp"
#include "ReturnType.hpp"
#include "Graphics.hpp"

namespace noob
{
	class font
	{
		public:
			struct glyph
			{
				glyph() = delete;
				// This constructor is an ugly one, I'll admit. It is, however, a manner of ensuring that only the right dimensions ever get placed inside it. The conversion is explicitly made via double-precision division in order to minimize bit-error.
				glyph(const noob::vec2ui GlyphDims, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) :
					dims(GlyphDims),
					mapped_dims(noob::vec2f(static_cast<float>(noob::div_dp(GlyphDims[0], AtlasDims[0])), static_cast<float>(noob::div_dp(GlyphDims[1], AtlasDims[1])))),
					mapped_pos(noob::vec2f(static_cast<float>(noob::div_dp(AtlasPos[0], AtlasDims[0])), static_cast<float>(noob::div_dp(AtlasPos[1], AtlasDims[1]))))
				{}

				glyph(const noob::font::glyph&) noexcept(true) = default;
				glyph& operator=(const noob::font::glyph&) noexcept(true) = default;

				// The FT_Glyph_Metrics struct saves information in pixel-space.
				// const FT_Glyph_Metrics metrics;
				const noob::vec2ui dims;
				// The following are in texture-space (0. 1)
				const noob::vec2f mapped_dims, mapped_pos; // Positions are topleft corner (x,y)
			};

			struct screen_text
			{
				std::string data;
				std::string language;
				hb_script_t script;
				hb_direction_t direction;
				const char* c_data() { return data.c_str(); };
			};

			static const hb_tag_t KernTag = HB_TAG('k', 'e', 'r', 'n'); // Kerning operations.
			static const hb_tag_t LigaTag = HB_TAG('l', 'i', 'g', 'a'); // Standard ligature substitution.
			static const hb_tag_t CligTag = HB_TAG('c', 'l', 'i', 'g'); // Contextual ligature substitution.

			static hb_feature_t ligature(bool Val)
			{
				const hb_feature_t results = { noob::font::LigaTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
				return results;
			}

			static hb_feature_t kerning(bool Val)
			{
				const hb_feature_t results = { noob::font::KernTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
				return results;
			}

			static hb_feature_t ligature_contextual(bool Val)
			{
				const hb_feature_t results = { noob::font::CligTag, static_cast<uint32_t>(Val ? 1 : 0), 0, std::numeric_limits<unsigned int>::max() };
				return results;	
			}


			~font() noexcept(true);
			bool init_library(const std::string& Mem, const noob::vec2d Dpi) noexcept(true);
			// A point is a size the corresponds to 1/72 of an inch.
			bool init_glyphs(const std::string& Characters, uint16_t SizeInPoints) noexcept(true);
			bool has_glyph(uint32_t CodePoint) const noexcept(true);

		protected:
			bool ft_library_valid = false;
			bool ft_face_valid = false;
			bool atlas_valid = false;

			// Font size in 1/64 point units
			uint16_t font_size, pixel_size;

			noob::fast_hashtable codepoints_to_glyphs;
			std::vector<noob::font::glyph> stored_glyphs;

			noob::texture_2d_handle tex;
			noob::vec2d dpi;

			FT_Library ft_lib;
			FT_Face ft_face;

			hb_font_t* hb_font = nullptr;
			hb_buffer_t* hb_buf = nullptr;

			ftgl::texture_atlas_t* atlas;

			bool init_glyphs_helper(const std::vector<uint32_t>& CodePoints, const noob::vec2ui AtlasDims) noexcept(true);

	};
}
