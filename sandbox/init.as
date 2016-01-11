void main()
{
	// default_stage s;
	vec3 eye_pos;
	eye_pos[0] = 0.0;
	eye_pos[1] = 0.0;
	eye_pos[2] = -200.0;

	vec3 look_to;
	look_to[0] = 0.0;
	look_to[1] = 0.0;
	look_to[2] = 0.0;

	vec3 up;
	up[0] = 0.0;
	up[1] = 1.0;
	up[2] = 0.0;

	default_stage.view_mat = look_at(eye_pos, look_to, up);
	
	default_stage.show_origin = false;

	load_texture("grad_map", "gradient_map.dds", 0);// BGFX_TEXTURE_NONE);

	triplanar_gradmap_uniform moon_shader;
	moon_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 0.0));
}

/*
	default_stage.view_mat = noob::look_at(noob::vec3(0.0, 0.0, -200.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0)); //look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up)
	
	default_stage.show_origin = false;
	noob::graphics::load_texture("grad_map", "gradient_map.dds", BGFX_TEXTURE_NONE);
	
	noob::triplanar_gradient_map_renderer::uniform moon_shader;
	
	moon_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 0.0);
	moon_shader.colours[1] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.colours[2] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.colours[3] = noob::vec4(0.0, 0.0, 0.0, 0.0);
	moon_shader.blend = noob::vec4(0.0, 0.0, 1.0, 0.0);
	moon_shader.scales = noob::vec4(1.0/100.0, 1.0/100.0, 1.0/100.0, 0.0);
	moon_shader.colour_positions = noob::vec4(0.5, 0.8, 0.0, 0.0);
	moon_shader.texture_map = noob::graphics::get_texture("grad_map");

	default_stage.set_shader(moon_shader, "moon");
	
	noob::triplanar_gradient_map_renderer::uniform purple_shader;
	purple_shader.colours[0] = noob::vec4(1.0, 1.0, 1.0, 0.0);
	purple_shader.colours[1] = noob::vec4(1.0, 0.0, 1.0, 0.0);
	purple_shader.colours[2] = noob::vec4(1.0, 0.0, 1.0, 0.0);
	purple_shader.colours[3] = noob::vec4(0.0, 0.0, 1.0, 0.0);
	purple_shader.blend = noob::vec4(0.2, 0.0, 0.5, 0.0);
	purple_shader.scales = noob::vec4(1.0/4.0, 1.0/4.0, 1.0/4.0, 0.0);
	purple_shader.colour_positions = noob::vec4(0.2, 0.7, 0.0, 0.0);
	purple_shader.texture_map = moon_shader.texture_map;

	default_stage.set_shader(purple_shader, "purple");

	noob::basic_mesh temp = noob::mesh_utils::cone(50.0, 100.0);
	temp.translate(noob::vec3(0.0, 10.0, 0.0));
	temp.rotate(noob::versor(0.3333, 0.3333, 0.3333, 0.0));
	noob::basic_mesh scene_mesh = temp;
	auto scenery_h = default_stage.scenery(default_stage.add_mesh(scene_mesh), noob::vec3(0.0, 0.0, 0.0), "moon");

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-10.0, 10.0);

	for (size_t i = 0 ; i < 500; ++i)
	{
		auto h = default_stage.box(4.0, 4.0, 4.0); 
		float x_pos = dis(gen) * 10.0;
		x_pos += std::copysign(1.0, x_pos)*7.0;
		float z_pos = dis(gen) * 10.0;
		z_pos += std::copysign(1.0, z_pos)*7.0;

		auto temp_body = default_stage.body(noob::body_type::DYNAMIC, h, 1.0, noob::vec3(x_pos, 250.0, z_pos), noob::versor(0.0, 0.0, 0.0, 1.0)); //, true);

		default_stage.bodies.get(temp_body); 
		default_stage.prop(temp_body, "purple");
	}
*/
