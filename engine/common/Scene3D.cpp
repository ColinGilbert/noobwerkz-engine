#include "Scene3D.hpp"

void noob::scene3d::init()
{
	// TODO: Add node 0 (default)
	hacd_render.init();
	triplanar_render.init();
}

void noob::scene3d::loop()
{

}

void noob::scene3d::draw()
{
	
}


size_t noob::scene3d::add(const noob::scene3d::node& node_to_add, bool active)
{
	return 0;
}


size_t noob::scene3d::get_id(const std::string& name) const
{
	return 0;
}

noob::scene3d::node noob::scene3d::get(size_t id) const
{

}


void activate(const std::vector<size_t>& node_ids)
{

}


void disactivate(const std::vector<size_t>& node_ids)
{

}


/*
void noob::scene3d::dynamic_graph_from_static()
{
	
}


void noob::scene3d::static_graph_from_dynamic()
{
	
}
*/
