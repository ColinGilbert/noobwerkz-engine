#include "Application.hpp"

#include <random>

void noob::application::user_init()
{
	view_mat = noob::look_at(noob::vec3(0.0, 20.0, -50.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0)); //look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up)
	noob::triplanar_gradient_map_renderer::uniform_info moon_shader;
	moon_shader.colours[0] = noob::vec4(1.0, 0.0, 0.0, 1.0);
	moon_shader.colours[1] = noob::vec4(0.0, 1.0, 0.0, 1.0);
	moon_shader.colours[2] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	moon_shader.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	moon_shader.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	// moon_shader.scales = noob::vec3(1.0,1.0,1.0);
	moon_shader.scales = noob::vec3(1.0/40.0, 1.0/40.0, 1.0/40.0);
	// moon_shader.scales = noob::vec3(1.0, 1.0, 1.0);
	// moon_shader.scales = noob::vec3(1/10,1/10,1/10);
	moon_shader.colour_positions = noob::vec2(0.2, 0.7);
	stage.set_shader(moon_shader, "moon");
	// auto actor_id = stage.actor(stage.basic_models.get_handle("unit-sphere"), stage.skeletons.get_handle("null"), noob::vec3(0.0, 80.0, 0.0));

	noob::triplanar_gradient_map_renderer::uniform_info blue_shader;
	blue_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	blue_shader.colours[1] = noob::vec4(1.0, 0.0, 1.0, 1.0);
	blue_shader.colours[2] = noob::vec4(1.0, 0.0, 1.0, 1.0);
	blue_shader.colours[3] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	blue_shader.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	// blue_shader.scales = noob::vec3(1.0,1.0,1.0);
	blue_shader.scales = noob::vec3(1.0/40.0, 1.0/40.0, 1.0/40.0);
	// blue_shader.scales = noob::vec3(1.0, 1.0, 1.0);
	// blue_shader.scales = noob::vec3(1/10,1/10,1/10);
	blue_shader.colour_positions = noob::vec2(0.2, 0.7);
	stage.set_shader(blue_shader, "blue");

	// Make a basic scenery
	noob::basic_mesh temp_1 = noob::mesh_utils::box(500.0, 20.0, 500.0);
	noob::basic_mesh temp_2 = noob::mesh_utils::cone(15.0, 60.0);
	temp_2.translate(noob::vec3(0.0, -30.0, 0.0));
	noob::basic_mesh scene_mesh = noob::mesh_utils::csg(temp_1, temp_2, noob::csg_op::UNION);
	
	auto scenery_h = stage.scenery(stage.add_mesh(scene_mesh), noob::vec3(0.0, 0.0, 0.0), "moon");

	// Now, drop randomly-shaped hull objects all over it.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-10, 10);
	for (size_t i = 0 ; i < 500; ++i )
	{
		//std::vector<noob::vec3> points;
		//for (size_t k = 0; k < 5; ++k)
		//{
		//	points.push_back(noob::vec3(dis(gen)+3, dis(gen)+5, dis(gen)+3));
		//}
		// auto h = stage.hull(points);
		stage.prop(stage.body(stage.unit_sphere_shape, 1.0, noob::vec3(std::abs(dis(gen)*10.0), std::abs(dis(gen)*10.0), std::abs(dis(gen)*10.0))), "blue");
	}
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::HEADER);
	// player_character->move(true, false, false, false, true);
	// logger::log(player_character->get_debug_info());
}
