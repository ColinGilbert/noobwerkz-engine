#pragma once

#include <memory>

#include <noob/math/math_funcs.hpp>

#include "ComponentDefines.hpp"


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

			void init(const std::string& prefix, size_t width, size_t height) noexcept(true);
			void window_dims(size_t width, size_t height) noexcept(true);
			// TODO: Implement
			// noob::vec2 dims(const std::string& text); 
			void text(const noob::text_label lbl) noexcept(true);
			void text(const std::string& text, float x, float y, noob::gui::font_size size = noob::gui::font_size::READING, uint32_t colour = 0xFFFF00FF) noexcept(true);

			bool crosshairs_enabled;
			noob::vec4f crosshairs_colour;
		
		protected:
			float window_width, window_height;

		//	std::unique_ptr<noob::ui_font> _banner;
		//	std::unique_ptr<noob::ui_font> _title;
		//	std::unique_ptr<noob::ui_font> _header;
		//	std::unique_ptr<noob::ui_font> _reading;
	};
}
