#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;

noob::application::application() 
{
	logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("./"));
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


void noob::application::init()
{
	logger::log("");

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	voxels.init();
	stage.init();

	logger::log("[Application] done init.");
	user_init();
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);

	// static double time_elapsed = 0.0;
	// time_elapsed += delta;
	stage.update(delta);
	user_update(delta);
/*
	if (time_elapsed > 0.25)
	{
		boost::filesystem::path p;
		p += *prefix;
		p += "script.chai";
		boost::system::error_code ec;

		static std::time_t last_write = 0;
		std::time_t t = boost::filesystem::last_write_time(p, ec);
		if (ec != 0)
		{
			logger::log(fmt::format("[Application] - update() - error reading {0}: {1}", p.generic_string(), ec.message()));
		}	
		else if (last_write != t)
		{
			init();

			try
			{
				chai->eval_file(p.generic_string());
			}
			catch(std::exception e)
			{
				logger::log(fmt::format("[Application]. Caught ChaiScript exception: ", e.what()));
			}
			last_write = t;
		}
		time_elapsed = 0.0;
	}
*/
}


void noob::application::draw()
{

	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	stage.draw();
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		float damping = 360.0;
		noob::vec3 rotation(info.rotation);
		noob::vec3 translation(info.translation);
		view_mat = noob::rotate_x_deg(view_mat, -rotation[0]/damping);
		view_mat = noob::rotate_y_deg(view_mat, -rotation[1]/damping);
		view_mat = noob::rotate_z_deg(view_mat, -rotation[2]/damping);
		view_mat = noob::translate(view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
	}
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
		update(delta);
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

	logger::log(fmt::format("[Application] Setting archive directory (\"{0}\")", filepath));
	prefix = std::unique_ptr<std::string>(new std::string(filepath));
	logger::log(fmt::format("[Application] Archive dir = {0}", *prefix));
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		logger::log(fmt::format("[Application] Touch - pointer ID = {0}, ({1}, {2}), action = {3}", pointerID, x, y, action));

		if (pointerID < 3)
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
	if (window_height == 0) 
	{
		window_height = 1;
	}

	logger::log(fmt::format("[Application] Resize window to ({0}, {1})", window_width, window_height));
}
