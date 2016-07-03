#include "Application.hpp"
#include <cmath>
#include <random>


// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;


bool noob::application::user_init()
{
	dynamic_graph graph;

	auto first = graph.add_node();
	auto second = graph.add_node();
	auto third = graph.add_node();
	auto fourth = graph.add_node();


	fmt::MemoryWriter ww_n;
	ww_n << "[UserApp] First node = " << first << " Second node = " << second << " Third node = " << third << " Fourth node = " << fourth; 
	logger::log(ww_n.str());


	graph.add_edge(0, first);

	graph.add_edge(first, second);

	graph.add_edge(first, third);

	graph.add_edge(second, third);

	graph.add_edge(third, fourth);

	// auto t1 = graph.get_traveller();

	bool has_loops = graph.has_loops();

	fmt::MemoryWriter ww_loops;
	ww_loops << "[UserApp] Has loops (should have)? ";
	if (has_loops)
	{
		ww_loops << "Yes.";
	}
	else
	{
		ww_loops << "No.";
	}
	logger::log(ww_loops.str());


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
