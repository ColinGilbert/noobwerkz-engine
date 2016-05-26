shader_handle moon_shader_h;
shader_handle purple_shader_h;
shader_handle red_shader_h;

void set_shaders()
{
	load_texture("grad_map", "gradient_map.dds", 0);

	triplanar_gradmap_uniform moon_shader;
	moon_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 1.0));
	moon_shader.set_colour(1, vec4(1.0, 1.0, 1.0, 1.0));
	moon_shader.set_colour(2, vec4(1.0, 1.0, 1.0, 1.0));
	moon_shader.set_colour(3, vec4(0.0, 0.0, 0.0, 1.0));
	moon_shader.blend = vec4(0.0, 0.0, 1.0, 0.0);
	moon_shader.scales = vec4(1.0/100.0, 1.0/100.0, 1.0/100.0, 0.0);
	moon_shader.colour_positions = vec4(0.5, 0.8, 0.0, 0.0);
	moon_shader.texture_map = get_texture("grad_map");
	set_shader(moon_shader, "moon");
	moon_shader_h = get_shader("moon");
	
	triplanar_gradmap_uniform purple_shader;
	purple_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 0.0));
	purple_shader.set_colour(1, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(2, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(3, vec4(0.0, 0.0, 1.0, 0.0));
	purple_shader.blend = vec4(0.2, 0.0, 0.5, 0.0);
	purple_shader.scales = vec4(1.0/25.0, 1.0/25.0, 1.0/25.0, 0.0);
	// purple_shader.scales = vec4(1.0, 1.0, 1.0, 0.0);
	purple_shader.colour_positions = vec4(0.2, 0.7, 0.0, 0.0);
	purple_shader.texture_map = get_texture("grad_map");
	set_shader(purple_shader, "purple");
	purple_shader_h = get_shader("purple");

	basic_uniform red_shader;
	red_shader.colour = vec4(1.0, 0.0, 0.0, 1.0);
	set_shader(red_shader, "red");
	red_shader_h = get_shader("red");

}


void set_stage()
{
	// default_stage s;
	vec3 eye_pos(0.0, 600.0, 600.0);
	vec3 look_to(0.0, 0.0, 0.0);
	vec3 up(0.0, 1.0, 0.0);

	default_stage.ambient_light = vec4(0.4, 0.4, 0.4, 0.1);

	default_stage.eye_pos = eye_pos;
	default_stage.view_mat = look_at(eye_pos, look_to, up);
	default_stage.show_origin = false;
	
	float light_x_coeff = 500.0;
	float light_y = 500.0;
	float light_z_coeff = 500.0;
	float light_radius = 1000.0;
	float light_falloff = 0.8;
	vec3 light_colour(1.0, 1.0, 1.0);
	
	for (int i = 0; i < 6; ++i)
	{
		light l;
		float x = i * light_x_coeff;
		float y = light_y;
		float z = i * light_z_coeff;
		l.set_position(vec3(x, y, z));
		l.set_falloff(light_falloff);
		l.set_colour(light_colour);
		l.set_radius(light_radius);
		string s = "stage-light-" + i;
		// log("[USER] Setting light " + i);
		light_handle lh = set_light(l, s);
		default_stage.set_light(i, lh);
	}
/*
	for (int i = 0; i < 3; ++i)
	{
		light l;
		float x = i * -light_x_coeff;
		float y = light_y;
		float z = i * -light_z_coeff;;
		l.set_position(vec3(x, y, z));
		l.set_colour(light_colour);
		l.set_radius(light_radius);
		string s = "stage-light-" + i;
		// log("[USER] Setting light " + i);
		light_handle lh = set_light(l, s);
		default_stage.set_light(i, lh);
	}

*/

	reflectance r;
	// r.set_diffuse(vec3(1.0, 1.0, 1.0));
	r.set_shiny(32.0);
	r.set_fresnel(0.9);
	// r.set_albedo(0.9);
	// r.set_roughness(0.3);
	reflectance_handle rh = set_reflectance(r, "lol");
	// reflectance_handle rh = get_reflectance("lol");	
	basic_mesh a = sphere_mesh(30.0);//, 100.0);
	// a.translate(vec3(0.0, -60.0, 0.0));
	// a.rotate(versor(0.3, 0.3 , 0.3, 0.1));
	basic_mesh b = box_mesh(1200.0, 10.0, 1200.0);
	// TODO: Keep track of transformations in scriptable mesh class.
	// b.translate(vec3(0.0, -10.0, 0.0));

	basic_mesh c = box_mesh(100.0, 100.0, 100.0);

	default_stage.scenery(a, vec3(0.0, 0.0, 0.0), versor(0.0, 0.0, 0.0, 1.0), purple_shader_h, rh, "one");
	default_stage.scenery(b, vec3(0.0, 0.0, 0.0), versor(0.0, 0.0, 0.0, 1.0), moon_shader_h, rh, "two");
	default_stage.scenery(c, vec3(300.0, 300.0, 300.0), versor(0.0, 0.0, 0.0, 1.0), purple_shader_h, rh, "three");

	float y_pos = 500.0;
	float cube_length = 4.0;
	shape_handle box_h = box_shape(cube_length, cube_length, cube_length);

	for (int i = -300 ; i < 300; ++i)
	{
		float x_pos = i * cube_length;
		float z_pos = i * cube_length;
		default_stage.actor(box_h, 1.0, vec3(x_pos, y_pos, z_pos), versor(0.0, 0.0, 0.0, 1.0), red_shader_h, rh);
	}

	for (int i = -300 ; i < 300; ++i)
	{
		float x_pos = i * cube_length;
		float z_pos = -i * cube_length;
		default_stage.actor(box_h, 1.0, vec3(x_pos, y_pos, z_pos), versor(0.0, 0.0, 0.0, 1.0), red_shader_h, rh);
	}
}


void main()
{
	set_shaders();
	set_stage();
}
