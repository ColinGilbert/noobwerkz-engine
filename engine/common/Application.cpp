#include "Application.hpp"


void noob::application::init(uint32_t width, uint32_t height, const std::string filepath)
{
	logger::log(noob::importance::INFO, "[Application] Begin init.");

	window_width = width;
	window_height = height;



	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.init(width, height);

	started = paused = input_has_started = false;

	finger_positions = { noob::vec2(0.0, 0.0), noob::vec2(0.0, 0.0), noob::vec2(0.0, 0.0), noob::vec2(0.0, 0.0) };

	prefix = std::make_unique<std::string>(noob::concat(filepath, "/"));//unique_ptr<std::string>(new std::string(filepath));


	// TODO: Uncomment once noob::filesystem is fixed
	// noob::filesystem::init(*prefix);

	ui_enabled = true;
	gui.init("", window_width, window_height);


	noob::globals& g = noob::globals::get_instance();
	bool are_globals_initialized = g.init();
	assert(are_globals_initialized && "Globals not initialized!");

	noob::mat4 proj_mat = noob::perspective(60.0f, static_cast<float>(window_width) / static_cast<float>(window_height), 1.0, 2000.0);

	stage.init(window_width, window_height, proj_mat);
	logger::log(noob::importance::INFO, "[Application] Done basic init.");

	bool b = user_init();

	if (!b) 
	{
		logger::log(noob::importance::WARNING, "[Application] User C++ init failed!");
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
	const noob::time start_time = noob::clock::now();

	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.frame(window_width, window_height);

	stage.draw();

	const noob::time end_time = noob::clock::now();
	const noob::duration draw_duration = end_time - start_time;

	noob::globals& g = noob::globals::get_instance();
	g.profile_run.stage_draw_duration += draw_duration;
}


void noob::application::accept_ndof_data(const noob::ndof::data& info) noexcept(true)
{

	stage.accept_ndof_data(info);
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

		// if (!paused)
		//{
		double d = (1.0 / static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(time_since).count()));
		update(d);
		//}
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
	logger::log(noob::importance::INFO, noob::concat("[Application] Setting archive directory {", filepath, "}"));
	prefix = std::make_unique<std::string>(noob::concat(filepath, "/"));
	logger::log(noob::importance::INFO, noob::concat("[Application] Archive dir = {",  *prefix, "}"));
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		logger::log(noob::importance::INFO, noob::concat("[Application] Touch - pointer ID = ", noob::to_string(pointerID), ", ", noob::to_string(x), ", ", noob::to_string(y), ", action = ", noob::to_string(action)));

		if (pointerID < 3)
		{
			//	finger_positions[pointerID] = noob::vec2(x,y);
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

	// noob::mat4 view_mat = noob::look_at(eye_pos, eye_target, eye_up);
	noob::mat4 proj_mat = noob::perspective(60.0f, static_cast<float>(window_width) / static_cast<float>(window_height), 1.0 , 2000.0);

	stage.update_viewport_params(window_width, window_height, proj_mat);

	logger::log(noob::importance::INFO, noob::concat("[Application] Resize window to (", noob::to_string(window_width), ", ", noob::to_string(window_height), ")"));
}


void noob::application::key_input(char c)
{

}


void noob::application::remove_shapes()
{
	noob::globals& g = noob::globals::get_instance();
	
	for (size_t i = 0; i < g.shapes.count(); ++i)
	{
		g.shapes.get(noob::shape_handle::make(i)).clear();
	}
	
	g.shapes.empty();
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
