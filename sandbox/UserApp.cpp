#include "Application.hpp"





std::shared_ptr<noob::actor> player_character;
std::shared_ptr<noob::prop> ground;

void noob::application::user_init()
{
	player_character = stage.make_actor("player-character", stage.get_unit_cube(), stage.get_skeleton("human").lock(), stage.get_debug_shader(), noob::identity_mat4());

}


void noob::application::user_update(double dt)
{
	gui.text("THE NIMBLE MONKEY GRABS THE APRICOT", 50.0, 50.0, noob::gui::font_size::header);
	stage.draw(player_character);
}
