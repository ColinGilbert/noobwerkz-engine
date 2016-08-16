#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"

// #include "Shiny.h"

// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;
noob::actor_handle ah;

bool noob::application::user_init()
{
	noob::audio_sample samp;
	bool b = samp.load_file("./BlanketedLama.ogg");
	noob::globals& g = noob::globals::get_instance();
	noob::sample_handle h = g.samples.add(std::make_unique<noob::audio_sample>(samp));	

	noob::audio_sample* s = g.samples.get(h);


	g.master_mixer.play_clip(h, 1.0);

	noob::shape_handle sh = g.sphere_shape(100.0);

	noob::actor_blueprints bp;
	bp.bounds = sh;
	// bp.shader = g.default_triplanar();
	g.set_actor_blueprints(bp, "test-collisions");
	noob::actor_blueprints_handle bph = g.get_actor_blueprints("test-collisions");
	ah = stage.actor(bph, 0, noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));

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
	const uint64_t interval = 3000;
	static std::unique_ptr<std::string> message;
	fmt::MemoryWriter ww, www;
	if (noob::millis(time_since_update) > interval - 1)
	{
		noob::profiler_snap snap = g.profile_run;
		ww << "NoobWerkz editor - Frame time: " << pretty_print_timing(divide_duration(snap.total_time, interval)) << ", draw time: " << pretty_print_timing(divide_duration(snap.stage_draw_time, interval)) << ", physics time " << pretty_print_timing(divide_duration(snap.stage_physics_time, interval));

		/*
		   std::vector<noob::contact_point> cps = stage.get_intersecting(ah);

		   www << "[UserApp] Actor (" << ah.index() << "), num collisions = " << cps.size();
		   for (noob::contact_point cp : cps)
		   {
		   www << ", " << cp.to_string();
		   }

		   logger::log(www.str());
		   */	

		message = std::make_unique<std::string>(ww.str());
		g.profile_run.total_time = g.profile_run.stage_physics_time = g.profile_run.stage_draw_time = time_since_update = noob::duration(0);
		last_ui_update = nowtime;
	}
	// gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
	gui.text(*message, 50.0, 50.0);
}
