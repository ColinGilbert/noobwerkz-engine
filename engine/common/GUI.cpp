#include "GUI.hpp"

// #define GUI_FILL_LAYER_1 1
// #define GUI_FILL_LAYER_2 2
// #define GUI_FILL_LAYER_3 3
#define GUI_TEXT_LAYER 4


void noob::gui::init(const std::string& prefix, const noob::vec2ui Dims) noexcept(true)
{
/*	_reading = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_header = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_title = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_banner = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	
	std::string fontfile_path = "font/opendyslexic-3-regular.ttf";
	
	_reading->init(fontfile_path, 14, width, height);
	_header->init(fontfile_path, 18, width, height);
	_title->init(fontfile_path, 32, width, height);
	_banner->init(fontfile_path, 48, width, height);
	
	window_width = static_cast<float>(width);
	window_height = static_cast<float>(height);
*/
}


void noob::gui::set_dims(const noob::vec2ui Dims) noexcept(true)
{
}


void noob::gui::text(const std::string& text, const noob::vec2f Pos, noob::gui::font_size size, uint32_t colour) noexcept(true)
{
/*
	switch (size)
	{
		case (noob::gui::font_size::READING):
			_reading->set_colour(colour);
			_reading->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::HEADER):
			_header->set_colour(colour);
			_header->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::TITLE):
			_title->set_colour(colour);
			_title->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::BANNER):
			_banner->set_colour(colour);
			_banner->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
	};
*/
}
