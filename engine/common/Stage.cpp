#include "Stage.hpp"


void noob::stage::init()
{
	triplanar_render.init();
	basic_render.init();
	world.init();

	transform = storage.register_component<noob::mat4>("transform");
	body = storage.register_component<noob::physics_body>("body");
	shape = storage.register_component<noob::physics_shape>("shape");
	model = storage.register_component<noob::model*>("model");
	triplanar_shader = storage.register_component<noob::triplanar_renderer::uniform_info>("triplanar shader");
	basic_shader = storage.register_component<noob::basic_renderer::uniform_info>("basic shader");
	anim_name = storage.register_component<std::string>("animation name");
	anim_time = storage.register_component<std::string>("animation time");

	// triplanar.init();
/*	add_drawable("unit_sphere", noob::basic_mesh::sphere(1.0));
	add_drawable("unit_cube", noob::basic_mesh::cube(1.0, 1.0, 1.0));
	add_drawable("unit_cylinder", noob::basic_mesh::cylinder(1.0, 1.0));
	add_drawable("unit_cone", noob::basic_mesh::cone(1.0, 1.0));

	unit_sphere = get_drawable("unit_sphere");
	unit_cube = get_drawable("unit_cube");
	unit_cylinder = get_drawable("unit_cylinder");
	unit_cone = get_drawable("unit_cone");
	auto nonexistent = get_drawable("nonexistent");

	add_skeleton("human", "seymour.skel.ozz");
*/
	//noob::actor placehold;
	//add_actor("placeholder", placehold);
}


void noob::stage::update(double dt)
{
	world.step(dt);
}


void noob::stage::draw()
{

	//logger::log("[Stage] - draw()");
	/*
	   if (std::shared_ptr<noob::drawable3d> d = unit_sphere.lock())
	   {

	   noob::triplanar_renderer::uniform_info u;
	   u.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	   u.colours[1] = noob::vec4(0.8, 0.8, 0.8, 1.0);
	   u.colours[2] = noob::vec4(0.4, 0.4, 0.4, 1.0);
	   u.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	   u.mapping_blends = noob::vec3(1.0, 0.5, 0.8);
	   u.scales = noob::vec3(1.0, 1.0, 1.0);
	   u.colour_positions = noob::vec2(0.2, 0.7);

	//noob::basic_renderer::uniform_info u;
	//u.colour = noob::vec4(1.0, 1.0, 1.0, 1.0);
	shaders.draw(d.get(), u, noob::identity_mat4());//, u);
	}
	else
	{
	logger::log("[Stage] trying to draw from bad pointer.");
	}
	*/
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}

/*
void noob::stage::add_drawable(const std::string& name, const std::string& filename)
{
	
}


void noob::stage::add_drawable(const std::string& name, const noob::basic_mesh& m)
{
	auto search = drawables.find(name);
	if (search == drawables.end())
	{
		auto results = drawables.insert(std::make_pair(name, std::make_unique<noob::drawable3d>()));
		(*(results.first)).second->init(m);
	}
}


noob::drawable3d* noob::stage::get_drawable(const std::string& name) const
{
	auto search = drawables.find(name);
	if (search == drawables.end())
	{
		logger::log(fmt::format("[Stage] asking for invalid drawable \"{0}\", returning unit sphere instead.", name));
		return unit_sphere;
	}
	return (search->second).get();
}
*/

void noob::stage::add_skeleton(const std::string& name, const std::string& filename)
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{
		auto results = skeletons.insert(std::make_pair(name, std::make_unique<noob::skeletal_anim>()));
		(*(results.first)).second->load_skeleton(filename);
	}
}


noob::skeletal_anim* noob::stage::get_skeleton(const std::string& name) const
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{	
		return nullptr;
	}

	return (search->second).get();
}
