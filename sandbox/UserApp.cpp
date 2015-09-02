#include "Application.hpp"
// TODO: Cleanup

std::shared_ptr<noob::actor> player_character;
std::shared_ptr<noob::prop> ground;
bool forward, back, left, right, jump, move;

void reset_locals()
{
	forward = back = left = right = jump = move = false;
}

void noob::application::user_init()
{
	reset_locals();
	//	view_mat = noob::look_at(noob::vec3(0, 50.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
	view_mat = noob::look_at(noob::vec3(0, 75.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
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

	rp3d::RigidBody* actor_bod = stage.body(noob::vec3(0.0, 50.0, 0.0));
	float actor_width = 0.5;
	float actor_height = 1.0;
	std::shared_ptr<noob::prop> actor_prop = stage.make_prop("actor-prop", actor_bod, stage.get_unit_sphere(), stage.get_shader("red").lock());
	actor_prop->attach_capsule(actor_width/2.0, actor_height, 1.0);

	player_character = stage.make_actor("player-character", actor_prop, stage.get_skeleton("human").lock(), actor_width, actor_height);

	noob::basic_mesh a = noob::basic_mesh::sphere(10);
	noob::basic_mesh b = noob::basic_mesh::cone(30, 35);
	noob::transform_helper t;
	t.translate(noob::vec3(0.0, 25.0, 0.0));
	b = b.transform(t.get_matrix());
	noob::basic_mesh c = noob::basic_mesh::csg(a, b, noob::csg_op::DIFFERENCE);
	noob::basic_mesh d = noob::basic_mesh::cube(100.0, 5.0, 100.0);
	t.translate(noob::vec3(0.0, -35.0, 0.0));
	noob::transform_helper t2;
	t2.translate(noob::vec3(0.0, -2.0, 0.0));
	d = d.transform(t2.get_matrix());
	noob::basic_mesh e = noob::basic_mesh::csg(c, d, noob::csg_op::UNION);
	rp3d::RigidBody* ground_bod = stage.body(noob::vec3(0.0, -20.0, 0.0));
	ground_bod->setType(rp3d::STATIC);

	stage.add_model("ground", e);

	std::shared_ptr<noob::prop> ground_prop = stage.make_prop("ground", ground_bod, stage.get_model("ground").lock(), stage.get_shader("moon").lock());
	ground_prop->attach_hull(c, 1.0);
	ground_prop->attach_hull(e, 1.0);

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
	if (move) player_character->move(forward, back, left, right, jump);
	reset_locals();
	//logger::log(player_character->get_debug_info());
	//player_character->move(true, false, false, false, true);//false);//true);
	//player_character->print_debug_info();
}

//enum class key_type { SPACE, APOSTROPHE, COMMA, MINUS, PERIOD, SLASH, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, SEMICOLON, EQUAL, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V ,W, X, Y, Z, LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, GRAVE_ACCENT, ESC, ENTER, TAB, BACKSPACE, INSERT, DELETE, RIGHT_ARROW, LEFT_ARROW, DOWN_ARROW, UP_ARROW, PAGE_UP, PAGE_DOWN, HOME, END, CAPLOCK, SCROLL_LOCK, NUM_LOCK, PRINT_SCREEN, PAUSE, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9, NUMPAD_DOT, NUMPAD_DIV, NUMPAD_MULT, NUMPAD_SUB, NUMPAD_ADD, NUMPAD_ENTER, NUMPAD_EQUAL, SHIFT_LEFT, CTRL_LEFT, ALT_LEFT, SUPER_LEFT, SHIFT_RIGHT, CTRL_RIGHT, ALT_RIGHT, SUPER_RIGHT, MENU, UNKNOWN};

//enum class key_state { PRESS, RELEASE, REPEAT, UNKNOWN }; 

void noob::application::key_input(noob::key_type type, noob::key_state state, int scancode)
{
	if (state == noob::key_state::PRESS || state == noob::key_state::REPEAT)
	{

			if (type == noob::key_type::W)
			{

				forward = true;
				move = true;
			}
			if (type == noob::key_type::A)
			{
				left = true;
				move = true;
			}
			if (type == noob::key_type::D)
			{
				right = true;
				move = true;
			}
			if (type == noob::key_type::S)
			{
				back = true;
				move = true;
			}
			if (type == noob::key_type::SPACE)
			{
				jump = true;
				move = true;
			}

	}
}
