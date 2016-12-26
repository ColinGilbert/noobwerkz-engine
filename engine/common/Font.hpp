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

#include <hb.h>
#include <hb-ft.h>
#include FT_GLYPH_H

#include "Texture.hpp"
#include "NoobUtils.hpp"
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
				glyph(const noob::vec2ui GlyphDims, const noob::vec2ui Bearings, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) :
					dims(GlyphDims),
					bearings(Bearings),
					mapped_dims(noob::vec2f(noob::div_fp(GlyphDims[0], AtlasDims[0]), noob::div_fp(GlyphDims[1], AtlasDims[1]))),
					mapped_pos(noob::vec2f(noob::div_fp(AtlasPos[0], AtlasDims[0]), noob::div_fp(AtlasPos[1], AtlasDims[1])))
				{}
				glyph(const noob::font::glyph&) noexcept(true) = default;
				~glyph() noexcept(true) = default;
				
				glyph& operator=(const noob::font::glyph&) noexcept(true) = default;

				const noob::vec2ui dims, bearings;
				// The following are in texture-space (0. 1)
				const noob::vec2f mapped_dims, mapped_pos;
			};
			
			
			static std::string to_string(const noob::font::glyph& Arg)
			{
				return noob::concat("dims = ", noob::to_string(Arg.dims), ", bearings = ", noob::to_string(Arg.bearings), ", mapped dims = ", noob::to_string(Arg.mapped_dims), ", mapped positions = ", noob::to_string(Arg.mapped_pos));
			}

/*
			struct shaped_glyph : public glyph
			{
				shaped_glyph() = delete;
				// This constructor is an ugly one, I'll admit. It is, however, a manner of ensuring that only the right dimensions ever get placed inside it. The conversion is explicitly made via double-precision division in order to minimize bit-error.
				shaped_glyph(const noob::vec2ui GlyphDims, const noob::vec2ui Bearings, const noob::vec2ui AtlasPos, const noob::vec2ui AtlasDims) noexcept(true) :
					 glyph(GlyphDims, Bearings, AtlasPos, AtlasDims)
				{}
				~shaped_glyph() noexcept(true) = default;
				shaped_glyph(const noob::font::shaped_glyph&) noexcept(true) = default;

				shaped_glyph& operator=(const noob::font::shaped_glyph&) noexcept(true) = default;

					

			};
*/
			struct shaped_text
			{
				noob::vec2f dims = noob::vec2f(0.0, 0.0);
				std::vector<std::array<noob::billboard_vertex, 6>> quads;
			};

			enum class feature { KERNING, LIGATURE, CONTEXTUAL_LIGATURE };

			enum class direction { LTR, RTL, TTB, BTT };

			class text
			{
				public:
				bool set_language(const std::string& Language) noexcept(true);
				bool set_script(const std::string& Script) noexcept(true);
				// uint32_t = ISO 15924 tag
				bool set_script(uint32_t Tag) noexcept(true);
			
				std::string get_language() const noexcept(true);
				// uint32_t = ISO 15924 tag
				uint32_t get_script() const noexcept(true);
				// This is where your text goes.
				std::string data;

				noob::font::direction get_dir() const noexcept(true);
				void set_dir(noob::font::direction) noexcept(true);
				
				// NOTE: Used by the font shaping API.
				hb_language_t get_language_hb() const noexcept(true) { return language; }
				hb_script_t get_script_hb() const noexcept(true) { return script; }
				hb_direction_t get_dir_hb() const noexcept(true) { return dir; }

				protected:
				hb_language_t language = hb_language_from_string ("en", 2); // Je suis perdu sans ma langue familiale...
				hb_script_t script = HB_SCRIPT_LATIN;
				hb_direction_t dir = HB_DIRECTION_LTR;
			};


			~font() noexcept(true);
			// A point is a size the corresponds to 1/72 of an inch.
			bool init_library(const std::string& Mem, const noob::vec2d Dpi, const std::string& Characters, uint16_t SizeInPoints) noexcept(true);

			bool has_glyph(uint32_t CodePoint) const noexcept(true);

			void add_feature(noob::font::feature Feature, bool Enable) noexcept(true);

			noob::results<noob::font::shaped_text> shape_text(const noob::font::text& Text) noexcept(true);

			noob::texture_2d_handle get_texture() const noexcept(true) { return tex; }

//			noob::billboard_buffer_handle get_billboard
		protected:
			
			noob::texture_2d_handle tex;
		//	;
			noob::vec2d dpi;

			// Note: Font size is in 1/64 point units
			uint16_t font_size, pixel_size;
			
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
			
			// Harfbuzz-related
			hb_font_t* hb_font = nullptr;
			hb_buffer_t* hb_buf = nullptr;
			std::vector<hb_feature_t> features;
			
			// Validity flags
			bool ft_face_valid = false;
			bool atlas_valid = false;

			bool init_glyphs(const std::string& Characters, uint16_t SizeInPoints) noexcept(true);
			bool init_glyphs_helper(const std::vector<uint32_t>& CodePoints, const noob::vec2ui AtlasDims) noexcept(true);

	};

	typedef noob::handle<noob::font> font_handle;

}

