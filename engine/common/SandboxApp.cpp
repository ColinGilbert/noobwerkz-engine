#include "Application.hpp"
#include <fstream>

void noob::application::init()
{
	std::string fontfile = *prefix + "/font/droidsans.ttf";
	droid_font->init(fontfile);
}

void noob::application::update(double delta)
{


}

void noob::application::draw()
{
	droid_font->change_colour(0xFFFF00FF);
	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
	droid_font->drawtext(std::string("Font test 2"), 100.0f, 100.0f, (int)width, (int)height);
}
