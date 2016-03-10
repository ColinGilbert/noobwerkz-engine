shader_handle moon_shader_h;
shader_handle purple_shader_h;

void set_shaders()
{
	load_texture("grad_map", "gradient_map.dds", 0);

	lit_triplanar_gradmap_uniform moon_shader;
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
	
	lit_triplanar_gradmap_uniform purple_shader;
	purple_shader.set_colour(0, vec4(1.0, 1.0, 1.0, 0.0));
	purple_shader.set_colour(1, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(2, vec4(1.0, 0.0, 1.0, 0.0));
	purple_shader.set_colour(3, vec4(0.0, 0.0, 1.0, 0.0));
	purple_shader.blend = vec4(0.2, 0.0, 0.5, 0.0);
	purple_shader.scales = vec4(1.0/25.0, 1.0/25.0, 1.0/25.0, 0.0);
	purple_shader.colour_positions = vec4(0.2, 0.7, 0.0, 0.0);
	purple_shader.texture_map = get_texture("grad_map");
	set_shader(purple_shader, "purple");
	purple_shader_h = get_shader("purple");

}


void set_stage()
{
	// default_stage s;
	vec3 eye_pos(0.0, 1500.0, 1500.0);
	vec3 look_to(0.0, 0.0, 0.0);
	vec3 up(0.0, 1.0, 0.0);

	default_stage.eye_pos = vec3(0.0, 1000.0, -500.0);
	default_stage.view_mat = look_at(eye_pos, look_to, up);
	default_stage.show_origin = false;


	for (int i = 0; i < 6; ++i)
	{
		light l;
		float x = i * 500.0;
		float y = 200.0;
		float z = i * 500.0;
		l.set_position(vec3(x, y, z));
		l.set_colour(vec3(1.0, 1.0, 1.0));
		string s = "stage-light-" + i;
		// log("[USER] Setting light " + i);
		light_handle lh = set_light(l, s);
		default_stage.set_light(i, lh);
	}
/*
	for (int i = 0; i < 3; ++i)
	{
		light l;
		float x = (float)i * -300.0;
		float y = 200.0;
		float z = (float)i * -300.0;
		l.set_position(vec3(x, y, z));
		l.set_colour(vec3(1.0, 1.0, 1.0));
		string s = "stage-light-" + i;
		// log("[USER] Setting light " + i);
		light_handle lh = set_light(l, s);
		default_stage.set_light(i, lh);
	}

*/

	reflectance r;
	// r.set_diffuse(vec3(1.0, 1.0, 1.0));
	r.set_specular_shiny(26.0);
	r.set_fresnel(0.9);
	r.set_albedo(0.9);
	r.set_roughness(0.3);
	reflectance_handle rh = set_reflectance(r, "lol");
	// reflectance_handle rh = get_reflectance("lol");	
	basic_mesh a = sphere_mesh(30.0);//, 100.0);
	// a.translate(vec3(0.0, -60.0, 0.0));
	// a.rotate(versor(0.3, 0.3 , 0.3, 0.1));
	basic_mesh b = box_mesh(4000.0, 10.0, 4000.0);
	// TODO: Keep track of transformations in scriptable mesh class.
	// b.translate(vec3(0.0, -10.0, 0.0));

	default_stage.scenery(a, vec3(0.0, 0.0, 0.0), versor(0.0, 0.0, 0.0, 1.0), purple_shader_h, rh, "one");
	default_stage.scenery(b, vec3(0.0, 0.0, 0.0), versor(0.0, 0.0, 0.0, 1.0), moon_shader_h, rh, "two");
/*	
	for (int i = -100 ; i < 100; ++i)
	{
		float x_pos = i;
		float z_pos = i;
		float s = random() * 10.0;
		shape_handle box_h = box_shape(s, s, s); 
		default_stage.actor(box_h, 1.0, vec3(x_pos, 250.0, z_pos), versor(0.0, 0.0, 0.0, 1.0), purple_shader_h);
	}

	for (int i = -100 ; i < 100; ++i)
	{
		float x_pos = i;
		float z_pos = -i;
		float s = random() * 10.0;
		shape_handle box_h = box_shape(s, s, s); 
		default_stage.actor(box_h, 1.0, vec3(x_pos, 250.0, z_pos), versor(0.0, 0.0, 0.0, 1.0), purple_shader_h);
	}
*/
}

void loop()
{

}

void main()
{
	set_shaders();
	set_stage();
}
