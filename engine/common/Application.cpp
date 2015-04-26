#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;

noob::application::application()
{
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	cam = std::unique_ptr<noob::camera>(new noob::camera());
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
}


noob::application::~application()
{
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	assert( app_pointer && "application not created!" );
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

	draw(delta);
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

void noob::application::update_cam(double delta)
{
	cam->update(delta);

	static float cam_x = 0;
	static float cam_y = 0;
	static float cam_z = 0;

	noob::vec3 cam_pos = cam->get_position();

	if (cam_x != cam_pos.v[0] || cam_y != cam_pos.v[1] || cam_z != cam_pos.v[2])
	{
		cam_x = cam_pos.v[0];
		cam_y = cam_pos.v[1];
		cam_z = cam_pos.v[2];
		std::stringstream ss;
		ss << "cam_pos = " << cam_x << " " << cam_y << " " << cam_z;
		logger::log(ss.str());
	}

}

void noob::application::window_resize(int w, int h)
{
	width = static_cast<float>(w);
	height = static_cast<float>(h);

	{
		std::stringstream ss;
		ss << "window_resize(" << width << ", " << height << ")";
		logger::log(ss.str());
	}

	if (height == 0) 
	{
		height = 1;
	}

	float ratio = width/height;

	proj_matrix = noob::perspective(67.0f, ratio, 0.01f, 1000.0f);
}
