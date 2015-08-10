#include "Stage.hpp"


void noob::stage::init()
{
	hacd_render.init();
	triplanar_render.init();
	world.init();
}


void noob::stage::update(double dt)
{
	world.step(dt);
}


void noob::stage::draw()
{

	
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
	bgfx::submit(0);
}

/*
std::array<noob::vec4, 6> extract_planes(const noob::mat4& mvp)
{

}
*/
