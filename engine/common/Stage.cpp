#include "Stage.hpp"


void noob::stage::init()
{
	hacd_render.init();
	shaders.init();
	world.init();
	add_drawable("unit_sphere", noob::mesh::sphere(1.0));
	add_drawable("unit_cube", noob::mesh::cube(1.0, 1.0, 1.0));
	add_drawable("unit_cylinder", noob::mesh::cylinder(1.0, 1.0));
	add_drawable("unit_cone", noob::mesh::cone(1.0, 1.0));
	unit_sphere = get_drawable("unit_sphere");
	unit_cube = get_drawable("unit_cube");
	unit_cylinder = get_drawable("unit_cylinder");
	unit_cone = get_drawable("unit_cone");
	add_skeleton("human", "seymour.skel.ozz");
	//noob::actor placehold;
	//add_actor("placeholder", placehold);
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


void noob::stage::add_drawable(const std::string& name, const noob::mesh& m)
{
	auto search = drawables.find(name);
	if (search == drawables.end())
	{
		auto results = drawables.insert(std::make_pair(name, std::make_shared<noob::drawable3d>()));
		(*(results.first)).second->init(m);
	}
}

std::weak_ptr<noob::drawable3d> noob::stage::get_drawable(const std::string& name) const
{
	auto search = drawables.find(name);
	if (search == drawables.end())
	{
		return unit_sphere;
	}
	return search->second;
}




void noob::stage::add_actor(const std::string& name, const noob::actor& actor)
{
	actors.insert(std::make_pair(name, actor));
}


void noob::stage::add_skeleton(const std::string& name, const std::string& filename)
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{
		auto results = skeletons.insert(std::make_pair(name, std::make_shared<noob::animated_model>()));
		(*(results.first)).second->load_skeleton(filename);
	}
}


std::weak_ptr<noob::animated_model> noob::stage::get_skeleton(const std::string& name) const
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{	
		return {}; //return std::make_shared<noob::animated_model>(nullptr);//weak_ptr<nullptr>;
		//auto results = skeletons.insert(std::make_pair(name, std::make_shared<noob::animated_model>()));
		//(*(results.first)).second->load_skeleton(filename);
	}

	return search->second;

}


void noob::stage::remove_actor(const std::string& name)
{
	auto search = actors.find(name);
	if (search != actors.end())
	{
		actors.erase(name);
	}
}


noob::actor noob::stage::get_actor(const std::string& name) const
{
	auto search = actors.find(name);
	if (search != actors.end())
	{
		return search->second;
	}
	return actors.find("placeholder")->second; 
}
