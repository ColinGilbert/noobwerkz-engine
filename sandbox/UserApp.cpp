#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"

// #include "Shiny.h"

// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;
std::unique_ptr<std::string> message_profiling, message_collision;
noob::actor_handle ah;

std::string message_profiling_two;

bool noob::application::user_init()
{
	message_profiling = std::make_unique<std::string>("");
	message_collision = std::make_unique<std::string>("");
	message_profiling_two = "";
	noob::audio_sample samp;
	bool b = samp.load_file("./BlanketedLama.ogg");
	noob::globals& g = noob::globals::get_instance();
	noob::sample_handle h = g.samples.add(std::make_unique<noob::audio_sample>(samp));	

	noob::audio_sample* s = g.samples.get(h);

	noob::basic_renderer::uniform u;
	u.colour = noob::vec4(1.0, 0.0, 1.0, 1.0);
	
	g.set_shader(u, "example-shader");
	noob::shader example_shader = g.get_shader("example-shader");

	
	noob::shader example_shader_two;
	example_shader_two.type = noob::shader_type::TRIPLANAR;
	example_shader_two.handle = g.get_default_triplanar_shader().index();

	g.master_mixer.play_clip(h, 1.0);

	const float actor_radius = 10.0;
	noob::shape_handle sh = g.sphere_shape(actor_radius);
	
	noob::actor_blueprints bp;
	bp.bounds = sh;
	bp.shader.type = example_shader.type;
	bp.shader.handle = example_shader.handle;
	g.set_actor_blueprints(bp, "example-actor-bp");
	noob::actor_blueprints_handle bph = g.get_actor_blueprints("example-actor-bp");
	
	const uint32_t num_actors = 500;
	const float actor_height = 100.0;
	const float actor_offset = 2.15;

	for (uint32_t i = 0; i < num_actors; ++i)
	{	
		ah = stage.actor(bph, 0, noob::vec3((actor_radius*2.0*i)+actor_offset, actor_height, (actor_radius*2.0)+actor_offset*i), noob::versor(0.0, 0.0, 0.0, 1.0));
	}

	noob::scenery_handle scene_h = stage.scenery(g.box_shape(1000.0, 10.0, 1000.0), example_shader_two, g.get_default_reflectance(), noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));
	

	eye_pos = noob::vec3(0.0, 500.0, -100.0);

	// keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view (currently does nothing)"));
	logger::log("[Application] Successfully done (C++) user init.");
	return true;
}



void noob::application::user_update(double dt)
{

	noob::globals& g = noob::globals::get_instance();
	noob::time nowtime = noob::clock::now();
	noob::duration time_since_update = nowtime - last_ui_update;
	const uint64_t profiling_interval = 3000;
	const uint64_t collision_display_interval = 500;
	if (noob::millis(time_since_update) > profiling_interval - 1)
	{
		
		noob::profiler_snap snap = g.profile_run;

	//	std::string msg = noob::pretty_print_timing(noob::divide_duration(snap.total_time, profiling_interval));//noob::concat(s, noob::pretty_print_timing(noob::divide_duration(snap.total_time, profiling_interval)));
		//message_profiling = std::make_unique<std::string>(noob::concat("NoobWerkz editor - Frame time: ", pretty_print_timing(divide_duration(snap.total_time, profiling_interval)), std::string(", draw time: "), pretty_print_timing(divide_duration(snap.stage_draw_duration, profiling_interval)), ", physics time ", pretty_print_timing(divide_duration(snap.stage_physics_duration, profiling_interval)))); 
		message_profiling_two = "";
		message_profiling_two = noob::pretty_print_timing(noob::divide_duration(snap.total_time, profiling_interval));
		noob::logger::log(message_profiling_two);
		// message_profiling.reset();
		//message_profiling = std::make_unique<std::string>(msg);
		// message_profiling = std::make_unique<std::string>(ww.str());
		g.profile_run.total_time = g.profile_run.stage_physics_duration = g.profile_run.stage_draw_duration = time_since_update = noob::duration(0);
		last_ui_update = nowtime;
	}



/*
	if (noob::millis(time_since_update) > collision_display_interval -1)
	{
		std::vector<noob::contact_point> cps = stage.get_intersecting(ah);

		std::string s = noob::concat("[UserApp] Actor (", noob::to_string(ah.index()), "), num collisions = ", noob::to_string(cps.size()));
		
		message_collision = std::make_unique<std::string>(s);
	}
*/
	// gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
	gui.text(message_profiling_two, 50.0, 50.0);
// 	gui.text(*message_collision, 50.0, 150.0);
}
