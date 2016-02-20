#include "Application.hpp"
#include <cmath>
#include <random>


// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;


bool noob::application::user_init()
{
	keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view"));
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

	gui.text(ww.str(), static_cast<float>(window_width - 500), static_cast<float>(window_height - 50), noob::gui::font_size::HEADER);
}
