#include "Stage.hpp"


void noob::stage::init()
{
	hacd_render.init();
	shaders.init();
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


void add_drawable(const std::string& name, const noob::drawable3d* drawable)
{

}


void add_actor(const std::string& name, const noob::actor& actor)
{

}


/*
std::array<noob::vec4, 6> extract_planes(const noob::mat4& mvp)
{

}
*/
