#include "Application.hpp"


void noob::application::init(const noob::vec2ui Dims, const noob::vec2d Dpi, const std::string& FilePath)
{
	logger::log(noob::importance::INFO, "[Application] Begin init.");

	window_dims = Dims;
	dpi = Dpi;

	prefix = std::make_unique<std::string>(FilePath);

	const std::string tex_src = noob::load_file_as_string(noob::concat(*prefix, "/texture/gradient_map.tga"));

	noob::texture_loader_2d tex_data;
	tex_data.from_mem(tex_src, false);
	
	noob::texture_info tex_info;
	tex_info.mips = 0;
	tex_info.pixels = tex_data.format();
	
	noob::graphics& gfx = noob::get_graphics();
	gfx.init(window_dims, tex_data);

	tex_data.free();

	started = paused = input_has_started = false;

	finger_positions = { noob::vec2f(0.0, 0.0), noob::vec2f(0.0, 0.0), noob::vec2f(0.0, 0.0), noob::vec2f(0.0, 0.0) };

	ui_enabled = true;
	gui.init("", window_dims);

	noob::globals& g = noob::get_globals();
	const bool are_globals_initialized = g.init();
	assert(are_globals_initialized && "Globals not initialized!");

	noob::mat4f proj_mat = noob::perspective<float>(60.0f, static_cast<float>(window_dims[0]) / static_cast<float>(window_dims[1]), 1.0, 2000.0);

	stage.init(window_dims, proj_mat);
	logger::log(noob::importance::INFO, noob::concat("[Application] Done basic init. Filepath = ", FilePath, ". Window dims: ", noob::to_string(window_dims), ". DPI: ", noob::to_string(dpi)));

	bool b = user_init();

	if (!b) 
	{
		logger::log(noob::importance::WARNING, "[Application] User C++ init failed!");
	}
}


void noob::application::update(double delta)
{
	gui.set_dims(window_dims);

	stage.update(delta);
	user_update(delta);
}


void noob::application::draw()
{
	const noob::time start_time = noob::clock::now();

	noob::graphics& gfx = noob::get_graphics();
	gfx.frame(window_dims);

	stage.draw();

	const noob::time end_time = noob::clock::now();
	const noob::duration draw_duration = end_time - start_time;

	noob::globals& g = noob::get_globals();
	g.profile_run.stage_draw_duration += draw_duration;
}


void noob::application::accept_ndof_data(const noob::ndof::data& info) noexcept(true)
{
	stage.accept_ndof_data(info);
}


void noob::application::step()
{
	noob::globals& g = noob::get_globals();
	if (g.finished_init())
	{
		// PROFILER_UPDATE();
		const noob::time start_time = noob::clock::now();
		const noob::duration time_since = last_step - start_time;

		// Engine code
		const double d = (1.0 / static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(time_since).count()));
		update(d);
		draw();

		const noob::time end_time = noob::clock::now();
		// Update class member
		last_step = end_time;
		
		const noob::duration time_taken = end_time - start_time;
		noob::globals& g = noob::get_globals();
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


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		logger::log(noob::importance::INFO, noob::concat("[Application] Touch - pointer ID = ", noob::to_string(pointerID), ", ", noob::to_string(x), ", ", noob::to_string(y), ", action = ", noob::to_string(action)));

		if (pointerID < 3)
		{
			// finger_positions[pointerID] = noob::vec2f(x,y);
		}
	}
	else input_has_started = true;
}


void noob::application::window_resize(const noob::vec2ui Dims)
{
	window_dims = Dims;

	if (window_dims[0] == 0) 
	{
		window_dims[0] = 1;
	}
	if (window_dims[1] == 0)
	{
		window_dims[1] = 1;
	}

	noob::mat4f proj_mat = noob::perspective<float>(60.0f, static_cast<float>(window_dims[0]) / static_cast<float>(window_dims[1]), 1.0, 2000.0);

	stage.update_viewport_params(window_dims, proj_mat);

	logger::log(noob::importance::INFO, noob::concat("[Application] Resize window to (", noob::to_string(window_dims), ")"));
}


void noob::application::key_input(char c)
{

}


void noob::application::remove_shapes()
{
	noob::globals& g = noob::get_globals();

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
