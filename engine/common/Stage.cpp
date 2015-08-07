
#include "Stage.hpp"


void noob::stage::init()
{
	hacd_render.init();
	triplanar_render.init();
	world.init();
}


// TODO: Call physics_world->update() on every 1/60 second
void noob::stage::update()
{

}


void noob::stage::draw()
{

	bgfx::submit(0);
}

std::array<noob::vec4, 6> extract_planes(const noob::mat4& mvp)
{

}
