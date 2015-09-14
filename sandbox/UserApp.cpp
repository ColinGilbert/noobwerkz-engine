#include "Application.hpp"

// noob::actor* player_character;
// noob::prop* ground;

void noob::application::user_init()
{
	reset_locals();
	//	view_mat = noob::look_at(noob::vec3(0, 50.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
	view_mat = noob::look_at(noob::vec3(0, 75.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
	noob::triplanar_renderer::uniform_info u;
	u.colours[0] = noob::vec4(1.0, 0.0, 0.0, 1.0);
	u.colours[1] = noob::vec4(0.0, 1.0, 0.0, 1.0);
	u.colours[2] = noob::vec4(0.0, 0.0, 1.0, 1.0);
	u.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	u.mapping_blends = noob::vec3(0.2, 0.0, 0.5);
	// 	u.scales = noob::vec3(1.0,1.0,1.0);
	u.scales = noob::vec3(1.0/40.0, 1.0/40.0, 1.0/40.0);
	// 	u.scales = noob::vec3(1.0, 1.0, 1.0);
	// 	u.scales = noob::vec3(1/10,1/10,1/10);
	u.colour_positions = noob::vec2(0.2, 0.7);
	//	stage.set_shader_name(stage.add_shader(u), "moon");

	// 	noob::basic_renderer::uniform_info basic_shader_info;
	// 	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	// 	stage.set_shader_name(stage.add_shader(basic_shader_info), "debug");

	//	size_t actor_id = stage.add_actor(stage.add_drawable(stage.get_model_id("unit-sphere"), stage.get_light_id("default"), stage.get_reflectance_id("default"), stage.get_shader_id("debug")), stage.get_skeleton_id("human"), noob::vec3(0.0, 80.0, 0.0));
	//	player_character = stage.get_actor_ptr(actor_id);

	//	voxels.sphere(20, 50, 50, 50);
	//	noob::basic_mesh a = voxels.extract();
	//	noob::basic_mesh g = a.to_origin();


	// 	noob::basic_mesh a = noob::mesh_utils::sphere(10);
	// 	noob::basic_mesh b = noob::mesh_utils::cone(30, 35);

	// 	b.translate(noob::vec3(0.0, 25.0, 0.0));
	// 	noob::basic_mesh c = noob::mesh_utils::csg(a, b, noob::csg_op::DIFFERENCE);
	//	noob::basic_mesh c = noob::mesh_utils::cube(20.0, 60.0, 20.0);
	//	noob::basic_mesh d = noob::mesh_utils::cube(1000.0, 15.0, 1000.0);
	//	d.translate(noob::vec3(0.0, -20.0, 0.0));
	//	noob::basic_mesh e = noob::mesh_utils::csg(c, d, noob::csg_op::UNION);
	//	size_t ground_model_id = stage.add_model(e);//, "ground");

	std::vector<noob::vec3> points;

	points.push_back(noob::vec3(-1.0, 0.0, 1.0));
	points.push_back(noob::vec3(2.0, 1.0, 2.0));
	points.push_back(noob::vec3(0.0, -2.0, -2.0));
	points.push_back(noob::vec3(6.0, 5.4, 3.0));
	points.push_back(noob::vec3(3.0, 5.0, 2.0));
	points.push_back(noob::vec3(0.1, 9.8, 6.0));
	points.push_back(noob::vec3(8.0, 3.9, -5.0));
	points.push_back(noob::vec3(7.0, 1.5, 2.0));

	// TODO: Fix crash!
	noob::basic_mesh h = noob::mesh_utils::hull(points);

		//	size_t ground_drawable_id = stage.add_drawable(ground_model_id, stage.get_light_id("default"), stage.get_reflectance_id("default"), stage.get_shader_id("moon"));//, "ground");
		//	stage.add_scenery(ground_drawable_id, noob::vec3(0.0, 0.0, 0.0));
}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::HEADER);
	//	player_character->move(true, false, false, false, true);
	// 	logger::log(player_character->get_debug_info());
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
