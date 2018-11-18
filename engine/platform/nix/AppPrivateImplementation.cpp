#include "AppPrivateImplementation.hpp"
#include "Application.hpp"

#include <memory>

static std::unique_ptr<noob::application> app;

void noob::app_facade::init(uint32_t Width, uint32_t Height, double DpiX, double DpiY, const std::string& FilePath)
{
	app = std::make_unique<noob::application>();
	app->init(noob::vec2ui(Width, Height), noob::vec2d(DpiX, DpiY), FilePath);
}


void noob::app_facade::window_resize(uint32_t Width, uint32_t Height)
{
	app->window_resize(noob::vec2ui(Width, Height));
}


void noob::app_facade::step()
{
	app->step();
}
