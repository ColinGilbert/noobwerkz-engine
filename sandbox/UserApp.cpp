#include "Application.hpp"
#include <cmath>
#include <random>
// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;

void noob::application::user_init()
{
	stage.view_mat = noob::look_at(noob::vec3(0.0, 0.0, -200.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0)); //look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up)
	
	stage.show_origin = false;
	
	noob::triplanar_gradient_map_renderer::uniform_info moon_shader;
	moon_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 0.0);
	moon_shader.colours[1] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.colours[2] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.colours[3] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.mapping_blends = noob::vec3(0.0, 0.0, 1.0);
	moon_shader.scales = noob::vec3(1.0/100.0, 1.0/100.0, 1.0/100.0);
	moon_shader.colour_positions = noob::vec2(0.4, 0.6);
	moon_shader.light_dir[0] = noob::vec3(0.0, 1.0, 0.0);
	stage.set_shader(moon_shader, "moon");
	
	noob::triplanar_gradient_map_renderer::uniform_info purple_shader;
	purple_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 0.0);
	purple_shader.colours[1] = noob::vec4(1.0, 0.0, 1.0, 0.0);
	purple_shader.colours[2] = noob::vec4(1.0, 0.0, 1.0, 0.0);
	purple_shader.colours[3] = noob::vec4(0.0, 0.0, 1.0, 0.0);
	purple_shader.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	purple_shader.scales = noob::vec3(1.0/4.0, 1.0/4.0, 1.0/4.0);
	purple_shader.colour_positions = noob::vec2(0.2, 0.7);
	purple_shader.light_dir[0] = noob::vec3(0.0, 1.0, 0.0);
	stage.set_shader(purple_shader, "purple");

	noob::basic_mesh temp = noob::mesh_utils::cone(50.0, 100.0);
	temp.translate(noob::vec3(0.0, 10.0, 0.0));
	temp.rotate(noob::versor(0.3333, 0.3333, 0.3333, 0.0));
	noob::basic_mesh scene_mesh = temp;
	auto scenery_h = stage.scenery(stage.add_mesh(scene_mesh), noob::vec3(0.0, 0.0, 0.0), "moon");

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-10.0, 10.0);

	for (size_t i = 0 ; i < 500; ++i)
	{
		auto h = stage.box(4.0, 4.0, 4.0); 
		float x_pos = dis(gen) * 10.0;
		x_pos += std::copysign(1.0, x_pos)*7.0;
		float z_pos = dis(gen) * 10.0;
		z_pos += std::copysign(1.0, z_pos)*7.0;

		auto temp_body = stage.body(noob::body_type::DYNAMIC, h, 1.0, noob::vec3(x_pos, 250.0, z_pos), noob::versor(0.0, 0.0, 0.0, 1.0)); //, true);

		stage.bodies.get(temp_body); 
		stage.prop(temp_body, "purple");
	}

	keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view"));
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
