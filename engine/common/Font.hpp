#pragma once

// std
#include <string>
#include <vector>

// External libs
#include <texture-atlas.h>
#include <vec234.h>
#include <noob/math/math_funcs.hpp>
#include <noob/fast_hashtable/fast_hashtable.hpp>

// Freetype-specific
#include <ft2build.h>
#include FT_FREETYPE_H  


#include "Texture.hpp"
#include "NoobUtils.hpp"
#include "ReturnType.hpp"

namespace noob
{
	class font
	{
		public:
			class glyph
			{
				public:
					glyph() = delete;
					// This constructor is an ugly one, I'll admit. It is, however, a manner of ensuring that only the right dimensions ever get placed inside it. The conversion is explicitly made via double-precision division in order to minimize bit-error.
					glyph(const FT_Glyph_Metrics Metrics, const noob::vec2ui GlyphDims, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) : metrics(Metrics), mapped_dims(noob::vec2f(static_cast<float>(static_cast<double>(GlyphDims[0]) / static_cast<double>(AtlasDims[0])), static_cast<float>(static_cast<double>(GlyphDims[1]) / static_cast<double>(AtlasDims[1])))), mapped_pos(noob::vec2f(static_cast<float>(static_cast<double>(AtlasPos[0]) / static_cast<double>(AtlasDims[0])), static_cast<float>(static_cast<double>(AtlasPos[1] / static_cast<double> (AtlasDims[1]))))) {}

					glyph(const noob::font::glyph&) noexcept(true) = default;
					glyph& operator=(const noob::font::glyph&) = default;

					noob::vec2ui pixel_dims() const noexcept(true)
					{
						return noob::vec2ui(metrics.width, metrics.height);
					}

					uint32_t bearing() const noexcept(true)
					{
						return metrics.horiBearingX;
					}

					uint32_t advance() const noexcept(true)
					{
						return metrics.horiAdvance;
					}
					noob::vec2f tex_dims() const noexcept(true)
					{
						return mapped_dims;
					}

					noob::vec2f tex_pos() const noexcept(true)
					{
						return mapped_pos;
					}

				protected:
					// The FT_Glyph_Metrics struct saves information in pixel-space.
					FT_Glyph_Metrics metrics;
					// The following are in texture-space (0. 1)
					noob::vec2f mapped_dims, mapped_pos; // Positions are topleft corner (x,y)
			};

			void init_library(const std::string& Mem, const noob::vec2d Dpi) noexcept(true);
			// A point is a size the corresponds to 1/72 of an inch.
			bool init_glyphs(const std::string& Characters, uint16_t SizeInPoints) noexcept(true);
			noob::return_type<noob::font::glyph> get_glyph(char) noexcept(true);

		protected:
			bool ft_library_valid = false;
			bool ft_face_valid = false;
			bool has_vertical = false;

			// Font size in 1/64 point units
			uint16_t font_size, pixel_size;//, ascender, descender, line_gap, max_advance_width, max_advance_height;

			noob::texture_2d_handle tex;
			noob::vec2d dpi;
			std::vector<noob::font::glyph> stored_glyphs;

			FT_Library ft_lib;
			FT_Face ft_face;

			ftgl::texture_atlas_t* atlas;

			bool init_glyphs_helper(const std::string& Characters, const noob::vec2ui AtlasDims) noexcept(true);

	};
}
