#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"

// #include "Shiny.h"

// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;
noob::actor_handle ah;
std::unique_ptr<std::string> message_profiling, message_collision;

bool noob::application::user_init()
{
	message_profiling = std::make_unique<std::string>("");
	message_collision = std::make_unique<std::string>("");
	noob::audio_sample samp;
	bool b = samp.load_file("./BlanketedLama.ogg");
	noob::globals& g = noob::globals::get_instance();
	noob::sample_handle h = g.samples.add(std::make_unique<noob::audio_sample>(samp));	

	noob::audio_sample* s = g.samples.get(h);

	noob::basic_renderer::uniform u;
	u.colour = noob::vec4(1.0, 0.0, 1.0, 1.0);
	
	g.set_shader(u, "templol");
	noob::shader templol = g.get_shader("templol");

	g.master_mixer.play_clip(h, 1.0);

	noob::shape_handle sh = g.sphere_shape(6.0);

	noob::actor_blueprints bp;
	bp.bounds = sh;
	// bp.shader = g.default_triplanar();
	g.set_actor_blueprints(bp, "test-collisions");
	noob::actor_blueprints_handle bph = g.get_actor_blueprints("test-collisions");
	ah = stage.actor(bph, 0, noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));
	// noob::scenery_handle scenery(const noob::shape_handle shape_arg, const noob::shader shader_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg);
	noob::scenery_handle scene_h = stage.scenery(g.box_shape(100.0, 10.0, 100.0), templol, g.get_default_reflectance(), noob::vec3(0.0, -10.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));
	// *profiler_text = get_profiler_text();
	fmt::MemoryWriter ww;
	ww << "[UserApp] Loaded actor " << ah.index();
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
		fmt::MemoryWriter ww;
		noob::profiler_snap snap = g.profile_run;
	
		ww << "NoobWerkz editor - Frame time: " << pretty_print_timing(divide_duration(snap.total_time, profiling_interval)) << ", draw time: " << pretty_print_timing(divide_duration(snap.stage_draw_duration, profiling_interval)) << ", physics time " << pretty_print_timing(divide_duration(snap.stage_physics_duration, profiling_interval));


		message_profiling = std::make_unique<std::string>(ww.str());
		g.profile_run.total_time = g.profile_run.stage_physics_duration = g.profile_run.stage_draw_duration = time_since_update = noob::duration(0);
		last_ui_update = nowtime;
	}

	if (noob::millis(time_since_update) > collision_display_interval -1)
	{
		fmt::MemoryWriter ww;

		std::vector<noob::contact_point> cps = stage.get_intersecting(ah);

		ww << "[UserApp] Actor (" << ah.index() << "), num collisions = " << cps.size();
		
		for (noob::contact_point cp : cps)
		{
			ww << ", " << cp.to_string();
		}
		message_collision = std::make_unique<std::string>(ww.str());
	}
	// gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
	gui.text(*message_profiling, 50.0, 50.0);
	gui.text(*message_collision, 50.0, 150.0);
}
