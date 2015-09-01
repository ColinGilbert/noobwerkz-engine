#include "Stage.hpp"

bool noob::stage::init()
{
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

	noob::basic_renderer::uniform_info dbg_shader;
	dbg_shader.colour = noob::vec4(0.0, 0.3, 0.3, 1.0);
	set_shader("basic-debug", dbg_shader);
	debug_shader = get_shader("basic-debug").lock();

	logger::log("[Stage] init complete.");
	return true;
}


void noob::stage::update(double dt)
{
	if (!paused)
	{
		dynamics_world.update(static_cast<rp3d::decimal>(dt));
		for (auto actor_it : actors)
		{
			actor_it.second->update();
			//actor_it.second->print_debug_info();
		}
	}
}


void noob::stage::draw() const
{
	for (auto a : actors)
	{
		draw(a.second);
		//debug_draw(a.second);
	}
	for (auto p : props)
	{
		draw(p.second.get());
	}
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::draw(noob::prop* p) const
{
	shaders.draw(p->model.get(), *(p->shading.get()), p->get_transform());
}


void noob::stage::draw(const std::shared_ptr<noob::actor>& a) const
{
	draw(a->get_prop());
	//shaders.draw(a->get_prop()->model.get(), *(a->get_prop()->shading.get()), a->get_prop()->get_transform());
}

/*
void noob::stage::debug_draw(noob::prop* p) const
{
}
*/

void noob::stage::debug_draw(const std::shared_ptr<noob::actor>& a) const
{
//	debug_draw(a->get_prop());
}


std::shared_ptr<noob::actor> noob::stage::make_actor(const std::string& name, const std::shared_ptr<noob::prop>& _prop, const std::shared_ptr<noob::skeletal_anim>& _skel_anim, float width, float height)
{
	// TODO: Optimize
	auto a = std::make_shared<noob::actor>();
	a->init(&dynamics_world, _prop, _skel_anim, width, height);
	actors[name] = a;
	return actors[name];

}


std::shared_ptr<noob::prop> noob::stage::make_prop(const std::string& _name, rp3d::RigidBody* _body, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::prepared_shaders::info>& _uniforms)
{
	auto p = std::make_shared<noob::prop>();
	p->init(_body, _model, _uniforms);
	props[_name] = p;
	return props[_name];
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

	return search->second;
}

rp3d::RigidBody* noob::stage::body(const noob::vec3& position, const noob::versor& orientation)
{
	return dynamics_world.createRigidBody(rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])));
}
