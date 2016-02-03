void main()
{
	// default_stage s;
	vec3 eye_pos(0.0, 1000.0, -500.0);
	vec3 look_to(0.0, 0.0, 0.0);
	vec3 up(0.0, 1.0, 0.);

	default_stage.view_mat = look_at(eye_pos, look_to, up);
	
	default_stage.show_origin = false;

	load_texture("grad_map", "gradient_map.dds", 0);

	triplanar_gradmap_uniform moon_shader;
	moon_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 0.0));
	moon_shader.set_colour(1, vec4(0.0, 0.0, 0.0, 0.0));
	moon_shader.set_colour(2, vec4(0.0, 0.0, 0.0, 0.0));
	moon_shader.set_colour(3, vec4(0.0, 0.0, 0.0, 0.0));
	moon_shader.blend = vec4(0.0, 0.0, 1.0, 0.0);
	moon_shader.scales = vec4(1.0/ 100.0, 1.0/100.0, 1.0/100.0, 0.0);
	moon_shader.colour_positions = vec4(0.5, 0.8, 0.0, 0.0);
	moon_shader.texture_map = get_texture("grad_map");

	set_shader(moon_shader, "moon");

	triplanar_gradmap_uniform purple_shader;
	purple_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 0.0));
	purple_shader.set_colour(1, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(2, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(3, vec4(0.0, 0.0, 1.0, 0.0));
	purple_shader.blend = vec4(0.2, 0.0, 0.5, 0.0);
	purple_shader.scales = vec4(1.0/4.0, 1.0/4.0, 1.0/4.0, 0.0);
	purple_shader.colour_positions = vec4(0.2, 0.7, 0.0, 0.0);
	purple_shader.texture_map = get_texture("grad_map");
	
	set_shader(purple_shader, "purple");

	basic_mesh a = cone_mesh(50.0, 100.0);
	basic_mesh b = box_mesh(500.0, 10.0, 500.0);
	b.translate(vec3(0.0, -10.0, 0.0));

	auto scenery_one = default_stage.scenery_from_mesh(add_mesh(a), vec3(0.0, 0.0, 0.0), "moon", versor(0.0, 0.0, 0.0, 1.0));
	auto scenery_two = default_stage.scenery_from_mesh(add_mesh(b), vec3(0.0, 0.0, 0.0), "moon", versor(0.0, 0.0, 0.0, 1.0));
	
	// std::random_device rd;
	// std::mt19937 gen(rd());
	// std::uniform_real_distribution<> dis(-10.0, 10.0);

	vector_vec3 p;
	p.push_back(vec3(-50.0, 0.0, -50.0));
	p.push_back(vec3(0.0, 35.0, 0.0));
	p.push_back(vec3(25.0, 0.0, 20.0));
	p.push_back(vec3(0.0, 10.0, 80.0));

	uint hull_h = hull(p);

	auto scenery_three = default_stage.scenery_from_shape(hull_h, vec3(0.0, 0.0, 0.0), "moon", versor(0.0, 0.0, 0.0, 1.0));

	for (int i = -500 ; i < 500; ++i)
	{
		uint h = box(4.0, 4.0, 4.0); 
		float x_pos = i;
		float z_pos = i; //dis(gen) * 10.0;
		uint temp_body = default_stage.body(DYNAMIC, h, 1.0, vec3(x_pos, 250.0, z_pos), versor(0.0, 0.0, 0.0, 1.0), false);
		// default_stage.bodies.get(temp_body); 
		default_stage.prop(temp_body, "purple");
	}

	for (int i = -500 ; i < 500; ++i)
	{
		uint h = box(4.0, 4.0, 4.0); 
		float x_pos = i;
		float z_pos = -i; //dis(gen) * 10.0;
		uint temp_body = default_stage.body(DYNAMIC, h, 1.0, vec3(x_pos, 250.0, z_pos), versor(0.0, 0.0, 0.0, 1.0), false);
		// default_stage.bodies.get(temp_body); 
		default_stage.prop(temp_body, "purple");
	}
}
