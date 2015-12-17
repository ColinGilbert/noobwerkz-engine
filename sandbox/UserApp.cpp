#include "Application.hpp"

#include <random>
// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;

void noob::application::user_init()
{
	view_mat = noob::look_at(noob::vec3(0.0, 20.0, -50.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0)); //look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up)
	noob::triplanar_gradient_map_renderer::uniform_info moon_shader;
	moon_shader.colours[0] = noob::vec4(1.0, 0.0, 0.0, 1.0);
	moon_shader.colours[1] = noob::vec4(0.0, 1.0, 0.0, 1.0);
	moon_shader.colours[2] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	moon_shader.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	moon_shader.mapping_blends = noob::vec3(0.7, 0.5, 1.0);
	// moon_shader.scales = noob::vec3(1.0,1.0,1.0);
	moon_shader.scales = noob::vec3(1.0/10.0, 1.0/10.0, 1.0/10.0);
	// moon_shader.scales = noob::vec3(1.0, 1.0, 1.0);
	// moon_shader.scales = noob::vec3(1/10,1/10,1/10);
	moon_shader.colour_positions = noob::vec2(0.4, 0.6);
	stage.set_shader(moon_shader, "moon");
	// auto actor_id = stage.actor(stage.basic_models.get_handle("unit-sphere"), stage.skeletons.get_handle("null"), noob::vec3(0.0, 80.0, 0.0));

	// This shader isn't really blue, but bear with me :P
	noob::triplanar_gradient_map_renderer::uniform_info purple_shader;
	purple_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	purple_shader.colours[1] = noob::vec4(1.0, 0.0, 1.0, 1.0);
	purple_shader.colours[2] = noob::vec4(1.0, 0.0, 1.0, 1.0);
	purple_shader.colours[3] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	purple_shader.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	// purple_shader.scales = noob::vec3(1.0,1.0,1.0);
	purple_shader.scales = noob::vec3(1.0, 1.0, 1.0);
	// purple_shader.scales = noob::vec3(1.0, 1.0, 1.0);
	// purple_shader.scales = noob::vec3(1/10,1/10,1/10);
	purple_shader.colour_positions = noob::vec2(0.2, 0.7);
	stage.set_shader(purple_shader, "purple");

	// Make a basic scenery
	noob::basic_mesh temp_1 = noob::mesh_utils::box(1000.0, 50.0, 1000.0);
	temp_1.translate(noob::vec3(0.0, -50.0, 0.0));
	auto scenery_h = stage.scenery(stage.add_mesh(temp_1), noob::vec3(0.0, 0.0, 0.0), "moon");

	// Now, drop randomly-shaped hull objects all over it.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-10, 10);

	for (size_t i = 0 ; i < 500; ++i )
	{
		// std::vector<noob::vec3> points;
		// for (size_t k = 0; k < 5; ++k)
		// {	
		// 	points.push_back(noob::vec3(dis(gen), dis(gen), dis(gen)));
		// }
		//auto h = stage.hull(points);
		auto h = stage.unit_sphere_shape;
		auto temp_body = stage.body(h, 1.0, noob::vec3(std::abs(dis(gen)*10.0), std::abs(dis(gen)*10.0) + 50.0, std::abs(dis(gen)*10.0)), noob::versor(0.0, 0.0, 0.0, 1.0));//, true);
		stage.bodies_holder.get(temp_body)->set_self_control(false);
		stage.prop(temp_body, "purple");
	}


	keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view"));
}


void noob::application::user_update(double dt)
{
	gui.text("Noobwerkz Editor", 50.0, 50.0, noob::gui::font_size::HEADER);
	// player_character->move(true, false, false, false, true);
	// logger::log(player_character->get_debug_info());
	fmt::MemoryWriter ww;
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
	

	//gui.text();
	//gui.text();


}
