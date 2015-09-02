#include "GUI.hpp"

#include "Logger.hpp"

#define GUI_FILL_LAYER_1 1
#define GUI_FILL_LAYER_2 2
#define GUI_FILL_LAYER_3 3
#define GUI_TEXT_LAYER 4
/*
noob::vec2 noob::gui::element::to_topleft(const noob::vec2& position, const noob::vec2& window_dims, noob::gui::element::anchor coords)
{
	switch (coords)
	{
		case noob::gui::element::anchor_type::TOP_LEFT:
			
			break;
		case noob::gui::element::anchor_type::TOP_RIGHT:

			break;
		case noob::gui::element::anchor_type::BOTTOM_LEFT:

			break;
		case noob::gui::element::anchor_type::TOP_RIGHT:

			break;
	}
}

*/
void noob::gui::init(const std::string& prefix, size_t width, size_t height)
{
	_reading = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_header = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_title = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	_banner = std::unique_ptr<noob::ui_font>(new noob::ui_font());

	std::string fontfile_path = prefix + "/font/OpenDyslexic3-Regular.ttf";

	_reading->init(fontfile_path, 14, width, height);
	_header->init(fontfile_path, 18, width, height);
	_title->init(fontfile_path, 32, width, height);
	_banner->init(fontfile_path, 48, width, height);

	window_width = static_cast<float>(width);
	window_height = static_cast<float>(height);

}


void noob::gui::window_dims(size_t width, size_t height)
{
	_reading->set_window_dims(width, height);
	_header->set_window_dims(width, height);
	_title->set_window_dims(width, height);
	_banner->set_window_dims(width, height);
}


void noob::gui::text(const std::string& text, float x, float y, noob::gui::font_size size, uint32_t colour)
{
	switch (size)
	{
		case (noob::gui::font_size::reading):
			_reading->set_colour(colour);
			_reading->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::header):
			_header->set_colour(colour);
			_header->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::title):
			_title->set_colour(colour);
			_title->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
		case (noob::gui::font_size::banner):
			_banner->set_colour(colour);
			_banner->draw_text(GUI_TEXT_LAYER, text, x, y);
			break;
	};

}


void noob::gui::frame()
{
	/*
	if (crosshairs_enabled == true) 
	{
		draw_crosshairs();
	}
	*/
}


void noob::gui::crosshairs(bool enabled)
{
	crosshairs_enabled = enabled;
}


void noob::gui::draw_crosshairs()
{

}
