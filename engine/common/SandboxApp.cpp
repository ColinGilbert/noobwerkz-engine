#include "Application.hpp"
#include <fstream>

void noob::application::init()
{
	logger::log("Sandbox: Begin initialization.");

	std::string fontfile = *prefix + "/font/droidsans.ttf";

	logger::log(std::string("Font file: " + fontfile));

	std::ifstream f(fontfile.c_str());
	if (f.good())
	{
		f.close();
		logger::log("Font file present :)");
		droid_font->init(fontfile);
	}
	else
	{
		f.close();
		logger::log("Font file not found :(");
	}

	logger::log("Sandbox: initialized :)");
}

void noob::application::update(double delta)
{


}

void noob::application::draw()
{
	droid_font->change_colour(0xFFFF00FF);
	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
}
