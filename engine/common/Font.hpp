#pragma once

// std
#include <string>
#include <vector>

// External libs
#include <texture-atlas.h>
#include <vec234.h>
#include <noob/math/math_funcs.hpp>
#include <noob/component/component.hpp>
#include <noob/fast_hashtable/fast_hashtable.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Texture.hpp"
#include "Logger.hpp"
#include "Results.hpp"
#include "Graphics.hpp"
#include "StringFuncs.hpp"
#include "BillboardModel.hpp"


namespace noob
{
	class font
	{
		public:
			struct glyph
			{
				glyph() = delete;
				// This constructor is an ugly one, I'll admit. It is, however, a manner of ensuring that only the right dimensions ever get placed inside it. The conversion is explicitly made via double-precision division in order to minimize bit-error.
				glyph(const noob::vec2f GlyphDims, const noob::vec2f Advances, const noob::vec2f Bearings,/* const noob::vec2f Offsets, */ const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) :
					dims(GlyphDims),
					advances(Advances),
					bearings(Bearings),
					// offsets(Offsets),
					mapped_dims(noob::vec2f(noob::div_fp(GlyphDims[0], AtlasDims[0]), noob::div_fp(GlyphDims[1], AtlasDims[1]))),
					mapped_pos(noob::vec2f(noob::div_fp(AtlasPos[0], AtlasDims[0]), noob::div_fp(AtlasPos[1], AtlasDims[1])))
				{}
				glyph(const noob::font::glyph&) noexcept(true) = default;
				~glyph() noexcept(true) = default;
				
				glyph& operator=(const noob::font::glyph&) noexcept(true) = default;

				// The following are in texture-space (0. 1)
				const noob::vec2f dims, advances, bearings, /* offsets, */ mapped_dims, mapped_pos;
			};
			
			
			static std::string to_string(const noob::font::glyph& Arg)
			{
				return noob::concat("dims = ", noob::to_string(Arg.dims), ", bearings = ", noob::to_string(Arg.bearings), ", mapped dims = ", noob::to_string(Arg.mapped_dims), ", mapped positions = ", noob::to_string(Arg.mapped_pos));
			}

			struct shaped_text
			{
				noob::vec2f dims = noob::vec2f(0.0, 0.0);
				std::vector<std::array<noob::billboard_vertex, 6>> quads;
			};


			enum class direction { LTR, RTL, TTB, BTT };

		
			~font() noexcept(true);
			// A point is a size the corresponds to 1/72 of an inch.
			bool init_library(const std::string& Mem, const noob::vec2d Dpi, const std::string& Characters, uint16_t SizeInPoints) noexcept(true);

			bool has_glyph(uint32_t CodePoint) const noexcept(true);

			noob::results<noob::font::shaped_text> shape_text(const std::string& Text) noexcept(true);

			noob::texture_2d_handle get_texture() const noexcept(true) { return tex; }

//			noob::billboard_buffer_handle get_billboard
		protected:
			
			noob::texture_2d_handle tex;
			noob::vec2d dpi;

			// Note: Font size is in 1/64 point units
			uint16_t font_size, pixel_size;
			float ascent = 0.0;
			float descent = 0.0;
			float line_gap = 0.0;
			// Output container
			// std::vector<noob::font::shaped_glyph> output_glyphs;
			
			// Containers used to check presence of glyphs
			noob::fast_hashtable codepoints_to_glyphs;
			std::vector<noob::font::glyph> stored_glyphs;
			
			// Texture atlas
			ftgl::texture_atlas_t* atlas;
			//noob::vec2ui Atlas
			// FreeType
			FT_Library ft_lib;
			FT_Face ft_face;
			
			noob::vec2f max_adv = noob::vec2f(0.0, 0.0);
				
			// Validity flags
			bool ft_face_valid = false;
			bool atlas_valid = false;

			bool init_glyphs(const std::string& Characters) noexcept(true);
			bool init_glyphs_helper(const std::vector<uint32_t>& CodePoints, const noob::vec2ui AtlasDims) noexcept(true);

	};

	typedef noob::handle<noob::font> font_handle;

}

