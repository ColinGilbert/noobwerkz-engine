#include "Application.hpp"
#include <cmath>
#include <random>
// #include "Shiny.h"

// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;


bool noob::application::user_init()
{
	noob::audio_sample samp;
	bool b = samp.load_file("./BlanketedLama.ogg");
	noob::globals& g = noob::globals::get_instance();
	noob::sample_handle h = g.samples.add(std::make_unique<noob::audio_sample>(samp));	

	noob::audio_sample* s = g.samples.get(h);


	g.master_mixer.play_clip(h, 1.0);

	// *profiler_text = get_profiler_text();

	// keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view (currently does nothing)"));
	logger::log("[Application] Successfully done (C++) user init.");
	return true;
}



void noob::application::user_update(double dt)
{
	gui.text("Noobwerkz Editor", 50.0, 50.0, noob::gui::font_size::HEADER);

	noob::globals& g = noob::globals::get_instance();

	noob::time nowtime = noob::clock::now();
	noob::duration time_since_update = nowtime - last_ui_update;
	const uint64_t interval = 500;
	if (noob::millis(time_since_update) > interval - 1)
	{
		noob::profiler_snap snap = g.profile_run;
		fmt::MemoryWriter ww;
		ww << "[Application] Profiling: Frame time: " << pretty_print_timing(divide_duration(snap.total_time, interval)) << ", draw time: " << pretty_print_timing(divide_duration(snap.stage_draw_time, interval)) << ", physics time " << pretty_print_timing(divide_duration(snap.stage_physics_time, interval));
		logger::log(ww.str());
		// time_since_update = noob::duration(0);
		g.profile_run.total_time = g.profile_run.stage_physics_time = g.profile_run.stage_draw_time = time_since_update = noob::duration(0);
		// last_ui_update = nowtime;
	}
	
	// gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
}
