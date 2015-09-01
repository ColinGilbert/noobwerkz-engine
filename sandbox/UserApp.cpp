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

	btRigidBody* actor_bod = stage.body(stage.cylinder(0.5, 2.0), 1.5, noob::vec3(0.0, 55.0, 0.0));
	std::shared_ptr<noob::prop> actor_prop = stage.make_prop("actor-prop", actor_bod, stage.get_unit_sphere(), stage.get_shader("red").lock());

	noob::basic_mesh a = noob::basic_mesh::sphere(10);
	noob::basic_mesh b = noob::basic_mesh::cone(30, 35);
	noob::transform_helper t;
	t.translate(noob::vec3(0.0, 25.0, 0.0));
	b = b.transform(t.get_matrix());
	noob::basic_mesh c = noob::basic_mesh::csg(a, b, noob::csg_op::DIFFERENCE);

	btRigidBody* plane_bod = stage.body(stage.breakable_mesh(c), 0.0, noob::vec3(0.0, 0.0, 0.0));
	stage.add_model("ground", c);
	std::shared_ptr<noob::prop> plane_prop = stage.make_prop("ground", plane_bod, stage.get_model("ground").lock(), stage.get_shader("moon").lock());
	//plane_prop->drawing_scale = noob::vec3(1000.0, 0.5, 1000.0);


/*
	noob::transform_helper actor_transform;
	actor_transform.translate(noob::vec3(0.0, 20.0, 0.0));
	player_character = stage.make_actor("player-character", stage.get_unit_cube(), stage.get_skeleton("human").lock(), stage.get_debug_shader(), actor_transform.get_matrix());


*/

	//fmt::MemoryWriter output_mesh_filename;
	//output_mesh_filename << "./terrain.bin";
	//std::ofstream os(output_mesh_filename.c_str(), std::ios::binary);
	//cereal::BinaryOutputArchive archive(os);
	//archive(terrain);

/*	
 	noob::physics_mesh terrain;
	std::ifstream is("./terrain.bin", std::ios::binary);
	cereal::BinaryInputArchive archive(is);
	archive(terrain);
	stage.add_model("terrain", terrain.mesh);
*/
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::header);
	//player_character->move(true, false, false, false, false);//true);
//	stage.draw(player_character);
	//player_character->print_debug_info();
}
