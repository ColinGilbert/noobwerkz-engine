
#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;

noob::application::application() 
{
	app_pointer = this;
}


noob::application::~application()
{
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
	logger::log("[Application] Begin init.");

	started = paused = input_has_started = false;

	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };

	prefix = std::unique_ptr<std::string>(new std::string("./"));


	// TODO: Uncomment once noob::filesystem is fixed
	// noob::filesystem::init(*prefix);

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);

	controller.set_eye_pos(noob::vec3(0.0, 300.0, -100.0));
	controller.set_eye_target(noob::vec3(0.0, 0.0, 0.0));
	controller.set_eye_up(noob::vec3(0.0, 1.0, 0.0));

	noob::globals& g = noob::globals::get_instance();

	if (g.init())
	{
		stage.init();
	}
	else 
	{
		logger::log("[Application] Global storage init failed :(");
	}

	voxels.init(512, 512, 512);

	logger::log("[Application] Done basic init.");

	bool b = user_init();

	if (!b) 
	{
		logger::log("[Application] User C++ init failed!");
	}

	//	network.init(3);
	//	network.connect("localhost", 4242);
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);
	/*
	   network.tick();

	   while (network.has_message())
	   {
	   std::string s = network.get_message();
	   if (s.compare(0, 6, "INIT: ") == 0)
	   {
	   stage.tear_down();
	   eval("init", s.substr(6, std::string::npos), true);
	   }
	   else if (s.compare(0, 8, "UPDATE: ") == 0)
	   {
	// TODO: Implement
	}
	else if (s.compare(0, 5, "CMD: ") == 0)
	{
	eval("cmd", s.substr(5, std::string::npos), true);
	}
	}
	*/
	stage.update(delta);
	user_update(delta);
}


void noob::application::draw()
{
	noob::mat4 proj_mat = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 1.0, 2000.0);
	stage.draw(window_width, window_height, controller.get_eye_pos(), controller.get_eye_target(), controller.get_eye_up(), proj_mat);
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		// float damping = 360.0;
		// noob::vec3 rotation(info.rotation);
		// noob::vec3 translation(info.translation);
		// view_mat = noob::rotate_x_deg(stage.view_mat, -rotation[0]/damping);
		// view_mat = noob::rotate_y_deg(stage.view_mat, -rotation[1]/damping);
		// view_mat = noob::rotate_z_deg(stage.view_mat, -rotation[2]/damping);
		// view_mat = noob::translate(stage.view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
		// stage.eye_pos = stage.eye_pos - translation;//translation);
	}
}


// TODO: Refactor
void noob::application::step()
{
	noob::globals& g = noob::globals::get_instance();
	if (g.finished_init())
	{
		// PROFILER_UPDATE();
		noob::time start_time = noob::clock::now();
		noob::duration time_since = last_step - start_time;

		if (!paused)
		{
			double d = (1.0 / static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(time_since).count()));
			update(d);
		}

		draw();

		noob::time end_time = noob::clock::now();
		last_step = end_time;
		noob::duration time_taken = end_time - start_time;

		noob::globals& g = noob::globals::get_instance();
		g.profile_run.total_time += time_taken;
	}
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
		fmt::MemoryWriter w;
		w << "[Application] Touch - pointer ID = " << pointerID << ", " << x << ", " << y << ", action = " << action;
		logger::log(w.str());

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

	fmt::MemoryWriter ww;
	ww << "[Application] Resize window to (" << window_width << ", " << window_height << ")";
	logger::log(ww.str());
}


void noob::application::key_input(char c)
{

}

void noob::application::remove_shapes()
{
	noob::globals& g = noob::globals::get_instance();
	for (size_t i = 0; i < g.shapes.items.size(); ++i)
	{
		if (g.shapes.items[i].physics_valid)
		{
			delete g.shapes.items[i].inner_shape;
		}
	}
}


/*
   std::string noob::application::get_profiler_text()
   {
   std::ostringstream profile_out;
   PROFILER_OUTPUT(profile_out);
   return profile_out.str();
// *profiler_text = output_profiling();
// logger::log(s);
//logger::log(profile_out.str());
}

void noob::application::output_profiling()
{
logger::log(get_profiler_text());//PROFILER_OUTPUT());
}

*/
