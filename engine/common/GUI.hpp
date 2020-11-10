#pragma once

#include <memory>
#include <vector>
#include <noob/math/math_funcs.hpp>

#include "Font.hpp"
#include "TextLabel.hpp"

namespace noob
{

	struct ui_window
	{
		float center_x, center_y, length, height;
		// noob::graphics::model_handle model;
	};

	class gui
	{
		public:
			enum class font_size
			{
				BANNER, TITLE, HEADER, READING
			};

			bool init(const std::string& Prefix, const noob::vec2ui Dims, const noob::vec2d Dpi) noexcept(true);
			void set_dims(const noob::vec2ui Dims) noexcept(true);
			// TODO: Implement
			// noob::vec2 dims(const std::string& text); 

			// TODO: More advanced text drawing and layout-aware functions
			void text(const noob::text_label lbl) noexcept(true);
			void text(const std::string& Text, const noob::vec2f Pos, noob::gui::font_size Size, const noob::vec4f Colour) noexcept(true);

			void draw() noexcept(true);
			
			// Overrides everything else in the drawlist and gives us the whole texture atlas. centered onscreen.
			void debug_draw_atlas() const noexcept(true);

			bool crosshairs_enabled;
			noob::vec4f crosshairs_colour;

		protected:
		
			noob::vec2ui dims;
			noob::font gui_font;
			uint32_t max_text_verts = 2048;
			
			noob::billboard_buffer_handle billbuf;
			uint32_t num_quads = 0;

			std::vector<noob::billboard_vertex> drawlist;


			noob::vec2f screenspace_pos(const noob::vec2f Pos) const noexcept(true);
	};
}
