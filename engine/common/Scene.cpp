#include "Scene.hpp"

void noob::scene::init()
{
	// voxels.init();
	physics.init();
	hacd_render.init();
	triplanar_render.init();
}


/*
void noob::scene::add_actor(const noob::actor& a)
{

}
*/

void noob::scene::draw()
{
	for (auto t : terrains)
	{
		triplanar_render.draw(*t->model.drawable, t->model.xform.get_matrix(), t->colouring_info);
	}
}
