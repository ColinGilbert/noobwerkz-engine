#include "Application.hpp"

std::shared_ptr<noob::actor> player_character;
std::shared_ptr<noob::prop> ground;

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
	stage.set_shader("moon", u);

	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	stage.set_shader("red", basic_shader_info);

	player_character = stage.make_actor("actor-prop", stage.get_unit_cylinder(), stage.get_shader("red").lock(), stage.get_skeleton("human").lock());

	player_character->set_position(noob::vec3(0.0, 50.0, 0.0));
	noob::basic_mesh a = noob::basic_mesh::sphere(10);
	noob::basic_mesh b = noob::basic_mesh::cone(30, 35);
	noob::transform_helper t;
	t.translate(noob::vec3(0.0, 25.0, 0.0));
	b = b.transform(t.get_matrix());
	noob::basic_mesh c = noob::basic_mesh::csg(a, b, noob::csg_op::DIFFERENCE);

	std::shared_ptr<noob::scenery> ground = stage.make_scenery("ground", c, stage.get_shader("moon").lock(), noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::header);
	//player_character->move(true, false, false, false, true);//true);
//	stage.draw(player_character);
	logger::log(player_character->get_debug_info());//player_character->print_debug_info();
}
