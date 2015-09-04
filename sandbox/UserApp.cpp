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
	//stage.set_shader("moon", u);

	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	//stage.set_shader("red", basic_shader_info);
	
	noob::light l;
	//stage.light("default", l);
	
	noob::reflectance r;
	//stage.reflectance("default", r);

	//std::shared_ptr<noob::drawable> drw = stage.make_drawable("character", stage.get_unit_cylinder(), stage.light("default").lock(), stage.reflectance("default").lock(), stage.get_shader("red").lock());
	//player_character = stage.make_actor("actor-prop", drw, stage.get_skeleton("human").lock(), noob::vec3(0.0, 60.0, 0.0));

	noob::basic_mesh a = noob::basic_mesh::sphere(10);
	noob::basic_mesh b = noob::basic_mesh::cone(30, 35);
	
	noob::transform_helper t;
	
	t.translate(noob::vec3(0.0, 25.0, 0.0));
	
	noob::basic_mesh c = b.transform(t.get_matrix());
	noob::basic_mesh d = noob::basic_mesh::csg(a, c, noob::csg_op::DIFFERENCE);
	noob::basic_mesh e = noob::basic_mesh::cube(1000.0, 15.0, 1000.0);
	t.translate(noob::vec3(0.0, -40.0, 0.0));
	noob::basic_mesh f = e.transform(t.get_matrix());
	noob::basic_mesh g = noob::basic_mesh::csg(d, f, noob::csg_op::UNION);
	
	//stage.add_model("ground", g);
	
	//std::shared_ptr<noob::drawable> d2 = stage.make_drawable("ground", stage.get_model("ground").lock(), stage.light("default").lock(), stage.reflectance("default").lock(), stage.get_shader("moon").lock());
	
	//std::shared_ptr<noob::drawable> d3 = stage.get_drawable("ground").lock();
	//std::shared_ptr<noob::scenery> ground = stage.make_scenery("ground", d2, noob::vec3(0.0, 0.0, 0.0));
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::header);
	//player_character->move(true, false, false, false, true);
	//logger::log(player_character->get_debug_info());
}
