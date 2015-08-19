#include "Stage.hpp"


bool noob::stage::init()
{
	triplanar_render.init();
	basic_render.init();
	world.init();

	lifetime = storage.register_component<float>("lifetime");
	transform = storage.register_component<noob::mat4>("transform");
	body = storage.register_component<noob::physics_body>("body");
	shape = storage.register_component<noob::physics_shape>("shape");
	model = storage.register_component<std::weak_ptr<noob::model>>("model");
	shader = storage.register_component<noob::shaders::info*>("shader");

	anim_skel = storage.register_component<std::weak_ptr<noob::skeletal_anim>>("skel_anim");
	anim_name = storage.register_component<std::string>("anim_name");
	anim_time = storage.register_component<std::string>("anim_time");

	actor_name = storage.register_component<std::string>("name");

	add_model("unit-sphere", noob::basic_mesh::sphere(1.0));
	add_model("unit-cube", noob::basic_mesh::cube(1.0, 1.0, 1.0));
	add_model("unit-cylinder", noob::basic_mesh::cylinder(1.0, 1.0));
	add_model("unit-cone", noob::basic_mesh::cone(1.0, 1.0));


	unit_sphere = get_model("unit-sphere");
	unit_cube = get_model("unit-cube");
	unit_cylinder = get_model("unit-cylinder");
	unit_cone = get_model("unit-cone");

	add_skeleton("human", "seymour.skel.ozz");

	logger::log("[Stage] init complete.");
	return true;
}


void noob::stage::update(double dt)
{
	if (!paused)
	{
		world.step(dt);
	}
}


void noob::stage::draw()
{

	//logger::log("[Stage] - draw()");
	if (auto d = unit_sphere.lock())
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
		//logger::log("[Stage] trying to draw from bad pointer.");
	}

	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::add_model(const std::string& name, const std::string& filename)
{
	return false;	
}


void noob::stage::add_model(const std::string& name, const noob::basic_mesh& m)
{
	auto search = models.find(name);
	if (search == models.end())
	{
		logger::log(fmt::format("Adding model: {0}", name));
		models.insert(std::make_pair(name, std::make_shared<noob::model>(m)));
		// The following gave a linker error instead of a compiler error upon removing init() function. gcc 4.9.2
		// (*(results.first)).second->init(m);
	}
	// logger::log("[Stage] Model added");
}


std::weak_ptr<noob::model> noob::stage::get_model(const std::string& name) const
{
	auto search = models.find(name);
	
	if (search == models.end())
	{
		logger::log(fmt::format("[Stage] Cannot find requested model: {0}", name));
		// TODO: Verify if this is proper form
		return {};
		//logger::log(fmt::format("[Stage] asking for invalid drawable \"{0}\", returning unit sphere instead.", name));
		//return unit_sphere;
	}

	std::weak_ptr<noob::model> temp;
	temp = search->second;
	return temp;
}


void noob::stage::add_skeleton(const std::string& name, const std::string& filename)
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{
		auto results = skeletons.insert(std::make_pair(name, std::make_unique<noob::skeletal_anim>()));
		(*(results.first)).second->init(filename);
	}
}


std::weak_ptr<noob::skeletal_anim> noob::stage::get_skeleton(const std::string& name) const
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{
		// TODO: Verify if this form is proper
		return {};
	}

	return search->second;//.get();
}
