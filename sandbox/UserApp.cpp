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


	// keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view (currently does nothing)"));
	logger::log("[Application] Successfully done (C++) user init.");
	return true;
}


void noob::application::user_update(double dt)
{
	gui.text("Noobwerkz Editor", 50.0, 50.0, noob::gui::font_size::HEADER);
	fmt::MemoryWriter ww;

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

	// gui.text(ww.str(), static_cast<float>(window_width - 500), static_cast<float>(window_height - 50), noob::gui::font_size::HEADER);
}
