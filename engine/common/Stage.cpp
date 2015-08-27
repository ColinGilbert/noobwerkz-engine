#include "Stage.hpp"

bool noob::stage::init()
{
	world.setEventListener(&phyz_listener);
	world.start();
	shaders.init();

	add_model("unit-sphere", noob::basic_mesh::sphere(0.5));
	add_model("unit-cube", noob::basic_mesh::cube(1.0, 1.0, 1.0));
	add_model("unit-cylinder", noob::basic_mesh::cylinder(1.0, 1.0));
	add_model("unit-cone", noob::basic_mesh::cone(1.0, 1.0));

	unit_sphere = get_model("unit-sphere").lock();
	unit_cube = get_model("unit-cube").lock();
	unit_cylinder = get_model("unit-cylinder").lock();
	unit_cone = get_model("unit-cone").lock();

	add_skeleton("human", "seymour.skel.ozz");

	noob::triplanar_renderer::uniform_info u;
	u.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	u.colours[1] = noob::vec4(0.8, 0.8, 0.8, 1.0);
	u.colours[2] = noob::vec4(0.4, 0.4, 0.4, 1.0);
	u.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	u.mapping_blends = noob::vec3(1.0, 0.5, 0.8);
	u.scales = noob::vec3(1.0, 1.0, 1.0);
	u.colour_positions = noob::vec2(0.2, 0.7);
	
	set_shader("moon", u);

	noob::transform_helper xform;
	xform.translate(noob::vec3(10.0, 65.0, 10.0));

	std::shared_ptr<noob::actor> test = make_actor("test", unit_cube, get_skeleton("human").lock(), get_shader("moon").lock(), xform.get_matrix(), 1.0, 1.0, 2.0, 5.0);
	
	logger::log("[Stage] init complete.");
	return true;
}


void noob::stage::update(double dt)
{
	if (!paused)
	{
		static double accum = 0.0;
		accum += dt;
		if (accum > 1.0/60.0)
		{
			world.update();
			accum -= 1.0/60.0;
		}
		for (auto actor_it : actors)
		{
			actor_it.second->update(dt, true);
		}
	}
}


void noob::stage::draw() const
{
	for (auto a : actors)
	{
		draw(a.second);
	}
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::draw(const std::shared_ptr<noob::actor>& a) const
{
	shaders.draw(a->get_prop().get_model().get(), *(a->get_prop().get_shading().get()), a->get_prop().get_transform());
}


std::shared_ptr<noob::actor> noob::stage::make_actor(const std::string& name, const std::shared_ptr<noob::model>& model, const std::shared_ptr<noob::skeletal_anim>& skel_anim, const std::shared_ptr<noob::prepared_shaders::info>& shader_uniform, const noob::mat4& transform, float mass, float width, float height, float max_speed)
{
	// TODO: Optimize
	auto a = std::make_shared<noob::actor>();

	a->init(&world, model, skel_anim, shader_uniform, transform, mass, width, height, max_speed);

	actors[name] = a;

	return actors[name];
}


bool noob::stage::add_model(const std::string& name, const std::string& filename)
{
	auto search = models.find(name);
	if (search == models.end())
	{
		logger::log(fmt::format("Adding model: {0}", name));
		models.insert(std::make_pair(name, std::make_shared<noob::model>(filename)));
		return true;
	}
	else return false;
}


bool noob::stage::add_model(const std::string& name, const noob::basic_mesh& m)
{
	auto search = models.find(name);
	if (search == models.end())
	{
		logger::log(fmt::format("Adding model: {0}", name));
		models.insert(std::make_pair(name, std::make_shared<noob::model>(m)));
		return true;
	}
	else return false;
}


bool noob::stage::add_skeleton(const std::string& name, const std::string& filename)
{
	auto search = skeletons.find(name);
	if (search == skeletons.end())
	{
		auto results = skeletons.insert(std::make_pair(name, std::make_unique<noob::skeletal_anim>()));
		(*(results.first)).second->init(filename);
		return true;
	}
	else return false;
}


void noob::stage::set_shader(const std::string& name, const noob::prepared_shaders::info& uniforms)
{
	auto a = std::make_shared<noob::prepared_shaders::info>(uniforms);
	shader_uniforms[name] = a;
}



std::weak_ptr<noob::actor> noob::stage::get_actor(const std::string& name) const
{
	auto search = actors.find(name);
	if (search == actors.end())
	{
		logger::log(fmt::format("[Stage] Cannot find requested actor: {0}", name));
		// TODO: Verify if this is proper form
		return {};
	}

	return search->second;
}


std::weak_ptr<noob::prepared_shaders::info> noob::stage::get_shader(const std::string& name) const
{
	auto search = shader_uniforms.find(name);

	if (search == shader_uniforms.end())
	{
		logger::log(fmt::format("[Stage] Cannot find requested shader: {0}", name));
		// TODO: Verify if this is proper form
		return {};
	}

	return search->second;
	
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

	return search->second;
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
