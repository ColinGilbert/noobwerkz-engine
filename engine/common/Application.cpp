#include "Application.hpp"

application* application::app_pointer = nullptr;

application::application()
{
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	cam = std::unique_ptr<noob::camera>(new noob::camera());
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
}


application::~application()
{
	app_pointer = nullptr;
}


application& application::get()
{
	assert( app_pointer && "application not created!" );
	return *app_pointer;
}


void application::step()
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

	draw(delta);
}


void application::pause()
{
	paused = true;
}


void application::resume()
{
	paused = false;
}


void application::set_archive_dir(const std::string& filepath)
{

	{
		std::stringstream ss;
		ss << "application: Setting archive dir(\"" << filepath << "\")";
		logger::log(ss.str());

	}	

	prefix = std::unique_ptr<std::string>(new std::string(filepath));

	{
		std::stringstream ss;
		ss << "application: prefix = " << *prefix;
		logger::log(ss.str());
	}
}


void application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		{
			std::stringstream ss;
			ss << "Sandbox: Touch - pointerID " << pointerID << ", (" << x << ", " << y << "), action " << action;
			logger::log(ss.str());
		}

		if (pointerID < 5)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}

void application::update(double delta)
{

}


void application::draw(double delta)
{

}


void application::window_resize(int w, int h)
{

}
