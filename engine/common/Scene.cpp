#include "Scene.hpp"

void noob::scene::init()
{
	hacd_render.init();
	triplanar_render.init();
	
}


void noob::scene::draw()
{
	
}


bool noob::scene::add_to_scene(const noob::scene::node& node_to_add, const std::string& parent_name)
{
	return false;
}


noob::scene::node noob::scene::get_node(const std::string& name) const
{
	static noob::scene::node blah;
	return blah;
}


void noob::scene::dynamic_graph_from_static()
{
	
}


void noob::scene::static_graph_from_dynamic()
{
	
}
