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
	// fmt::MemoryWriter ww;
	/*
	// Quick loop to help print key bindings to screen
	for (auto k : keystrokes)
	{
	ww << std::get<2>(k) << ": " << noob::keyboard::to_string(std::get<1>(k));
	if (noob::keyboard::to_string(std::get<1>(k)) != "")
	{
	ww << " ";
	}
	ww << noob::keyboard::to_string(std::get<0>(k)) << "\n";
	}
	*/
	// std::ostringstream profile_out;
	// PROFILER_OUTPUT(profile_out);
	// logger::log(profile_out.str());


	// output_profiling();
	// ww << *profiler_text;

	noob::globals& g = noob::globals::get_instance();

	noob::duration time_since_update = noob::clock::now() - last_ui_update;

	if (noob::millis(time_since_update) > 500)
	{
		fmt::MemoryWriter ww;
		ww << "Profiling: " << g.profile_run.to_string();
		g.strings.set(noob::string_handle::make(0), std::move(std::make_unique<std::string>(ww.str())));
	}
	
	gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
}
