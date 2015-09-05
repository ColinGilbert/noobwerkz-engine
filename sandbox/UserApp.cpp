#include "Application.hpp"

noob::actor* player_character;
noob::prop* ground;

void noob::application::user_init()
{
	noob::triplanar_renderer::uniform_info u;
	u.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	u.colours[1] = noob::vec4(0.8, 0.8, 0.8, 1.0);
	u.colours[2] = noob::vec4(0.4, 0.4, 0.4, 1.0);
	u.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	u.mapping_blends = noob::vec3(1.0, 0.5, 0.8);
	u.scales = noob::vec3(15.0, 5.0, 2.0);
	u.colour_positions = noob::vec2(0.2, 0.7);
	stage.set_shader_name(stage.add_shader(u), "moon");
	
	// noob::basic_renderer::uniform_info basic_shader_info;
	// basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	// stage.set_shader_name(stage.add_shader(basic_shader_info), "debug");
	
	size_t actor_id = stage.add_actor(stage.add_drawable(stage.get_model_id("unit-cylinder"), stage.get_light_id("default"), stage.get_reflectance_id("default"), stage.get_shader_id("debug")), stage.get_skeleton_id("human"), noob::vec3(0.0, 60.0, 0.0));
	player_character = stage.get_actor(actor_id);
	

	noob::basic_mesh a = noob::basic_mesh::sphere(10);
	noob::basic_mesh b = noob::basic_mesh::cone(30, 35);
	
	noob::transform_helper t;
	
	t.translate(noob::vec3(0.0, 25.0, 0.0));
	
	noob::basic_mesh c = b.transform(t.get_matrix());
	noob::basic_mesh d = noob::basic_mesh::csg(a, c, noob::csg_op::DIFFERENCE);
	noob::basic_mesh e = noob::basic_mesh::cube(1000.0, 15.0, 1000.0);
	t.translate(noob::vec3(0.0, -30.0, 0.0));
	noob::basic_mesh f = e.transform(t.get_matrix());
	noob::basic_mesh g = noob::basic_mesh::csg(f, d, noob::csg_op::UNION);
	
	stage.set_model_name(stage.add_model(g), "ground");
	
	stage.set_drawable_name(stage.add_drawable(stage.get_model_id("ground"), stage.get_light_id("default"), stage.get_reflectance_id("default"), stage.get_shader_id("moon")), "ground");
	size_t scenery = stage.add_scenery(stage.get_drawable_id("ground"), noob::vec3(0.0, 0.0, 0.0));
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::header);
	// player_character->move(true, false, false, false, true);
	//logger::log(player_character->get_debug_info());
}
