#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;

noob::application::application()
{
	// logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	droid_font = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("."));
}


noob::application::~application()
{
	logger::log("~application()");
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}


void noob::application::step()
{
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	uint64_t uNowNano = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	double delta = (uNowNano - time) * 0.000000001f;

	time = uNowNano;

	if (!paused)
	{
		update((float)delta);
	}

	draw();
}


void noob::application::pause()
{
	paused = true;
}


void noob::application::resume()
{
	paused = false;
}


void noob::application::set_archive_dir(const std::string& filepath)
{

	{
		std::stringstream ss;
		ss << "setting archive dir(\"" << filepath << "\")";
		logger::log(ss.str());

	}	

	prefix = std::unique_ptr<std::string>(new std::string(filepath));

	{
		std::stringstream ss;
		ss << "archive dir = " << *prefix;
		logger::log(ss.str());
	}
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		{
			std::stringstream ss;
			ss << "Touch - pointerID " << pointerID << ", (" << x << ", " << y << "), action " << action;
			logger::log(ss.str());
		}

		if (pointerID < 5)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}

void noob::application::window_resize(uint32_t w, uint32_t h)
{
	window_width = w;
	window_height = h;
	{
		std::stringstream ss;
		ss << "window_resize(" << window_width << ", " << window_height << ")";
		logger::log(ss.str());
	}

	if (window_height == 0) 
	{
		window_height = 1;
	}
}
