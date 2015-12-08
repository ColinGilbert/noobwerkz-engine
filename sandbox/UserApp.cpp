#include "Application.hpp"

void noob::application::user_init()
{
	noob::triplanar_gradient_map_renderer::uniform_info u;
	u.colours[0] = noob::vec4(1.0, 0.0, 0.0, 1.0);
	u.colours[1] = noob::vec4(0.0, 1.0, 0.0, 1.0);
	u.colours[2] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	u.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	u.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	// 	u.scales = noob::vec3(1.0,1.0,1.0);
	u.scales = noob::vec3(1.0/40.0, 1.0/40.0, 1.0/40.0);
	// 	u.scales = noob::vec3(1.0, 1.0, 1.0);
	// 	u.scales = noob::vec3(1/10,1/10,1/10);
	u.colour_positions = noob::vec2(0.2, 0.7);
	auto shader_id = stage.shader(u, "moon");
	// auto actor_id = stage.actor(stage.basic_models.get_handle("unit-sphere"), stage.skeletons.get_handle("null"), noob::vec3(0.0, 80.0, 0.0));
	std::vector<noob::vec3> points;

	points.push_back(noob::vec3(-1.0, 0.0, 1.0));
	points.push_back(noob::vec3(2.0, 1.0, 2.0));
	points.push_back(noob::vec3(0.0, -2.0, -2.0));
	points.push_back(noob::vec3(6.0, 5.4, 3.0));
	points.push_back(noob::vec3(3.0, 5.0, 2.0));
	points.push_back(noob::vec3(0.1, 9.8, 6.0));
	points.push_back(noob::vec3(8.0, 3.9, -5.0));
	points.push_back(noob::vec3(7.0, 1.5, 2.0));

	auto hull = stage.hull(points, "points");
	
	noob::basic_mesh m = noob::mesh_utils::cone(1.0, 2.0);
	m.from_half_edges(m.to_half_edges());
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::HEADER);
	//	player_character->move(true, false, false, false, true);
	// 	logger::log(player_character->get_debug_info());
}
