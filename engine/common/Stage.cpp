#include "Stage.hpp"

bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);

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

void noob::stage::tear_down()
{
	for (auto s : spheres)
	{
		delete s.second;
	}
	for (auto  s : boxes)
	{
		delete s.second;	
	}
	for (auto s : cylinders)
	{
		delete s.second;
	}
	for (auto s : cones)
	{
		delete s.second;
	}
	for (auto s : capsules)
	{
		delete s.second;
	}

	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}

void noob::stage::update(double dt)
{
	if (!paused)
	{
		//world.update();
		for (auto actor_it : actors)
		{
			actor_it.second->update();//dt);//, true, false, true, false, false);//true);
			//actor_it.second->print_debug_info();
		}
		//world.update(static_cast<rp3d::decimal>(dt));
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
	shaders.draw(p->model.get(), *(p->shading.get()), noob::scale(p->get_transform(), p->scale));
}


void noob::stage::draw(const std::shared_ptr<noob::actor>& a) const
{
	draw(a->get_prop());
	//shaders.draw(a->get_prop()->model.get(), *(a->get_prop()->shading.get()), a->get_prop()->get_transform());
}


void noob::stage::debug_draw(noob::prop* p) const
{
}


void noob::stage::debug_draw(const std::shared_ptr<noob::actor>& a) const
{
	debug_draw(a->get_prop());
}


std::shared_ptr<noob::actor> noob::stage::make_actor(const std::string& name, const std::shared_ptr<noob::model>& model, const std::shared_ptr<noob::skeletal_anim>& skel_anim, const std::shared_ptr<noob::prepared_shaders::info>& shader_uniform, const noob::mat4& transform, float mass, float width, float height, float max_speed)
{
	// TODO: Optimize
//	auto a = std::make_shared<noob::actor>();

//	a->init(&world, model, skel_anim, shader_uniform, transform, mass, width, height, max_speed);
//	a->destination_prop.init(&world, unit_sphere, debug_shader, transform);
//	a->destination_prop.add_sphere(1.0, 0.0);
//	a->destination_prop.body->setType(rp3d::KINEMATIC);
//	actors[name] = a;

//	return actors[name];

}


std::shared_ptr<noob::prop> noob::stage::make_prop(const std::string& name, const std::shared_ptr<noob::model>& drawable, const std::shared_ptr<noob::prepared_shaders::info>& uniforms, const noob::mat4& transform)
{
/*
	auto p = std::make_shared<noob::prop>();
	p->init(&world, drawable, uniforms, transform);
	props[name] = p;
	return props[name];
*/
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


btRigidBody* noob::stage::body(btCollisionShape* shape, float mass, const noob::vec3& pos, const noob::versor& orientation)
{
	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(btQuaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3]), btVector3(pos.v[0], pos.v[1], pos.v[2])));
	btVector3 inertia(0.0, 0.0, 0.0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass, motion_state, shape, inertia);
	btRigidBody* body = new btRigidBody(ci);
	dynamics_world->addRigidBody(body);
	return body;
}


btSphereShape* noob::stage::sphere(float r)
{
	auto search = spheres.find(r);
	if (search == spheres.end())
	{
		spheres[r] = new btSphereShape(r);
		return spheres[r];
	}
	else return spheres[r];
}


btBoxShape* noob::stage::box(float x, float y, float z)
{
	auto search = boxes.find(std::make_tuple(x,y,z));
	if (search == boxes.end())
	{
		auto results = boxes.insert(std::make_pair(std::make_tuple(x,y,z), new btBoxShape(btVector3(x, y, z))));
		return (results.first)->second;
	}
	else return boxes[std::make_tuple(x,y,z)];

}


btCylinderShape* noob::stage::cylinder(float r, float h)
{
	auto search = cylinders.find(std::make_tuple(r, h));
	if (search == cylinders.end())
	{
		auto results = cylinders.insert(std::make_pair(std::make_tuple(r, h), new btCylinderShape(btVector3(r, h / 2, r))));
		return (results.first)->second;
	}
	else return cylinders[std::make_tuple(r, h)];
}


btConeShape* noob::stage::cone(float r, float h)
{
	auto search = cones.find(std::make_tuple(r, h));
	if (search == cones.end())
	{
		auto results = cones.insert(std::make_pair(std::make_tuple(r, h), new btConeShape(r, h)));
		return (results.first)->second;
	}
	else return cones[std::make_tuple(r, h)];
}


btCapsuleShape* noob::stage::capsule(float r, float h)
{
	auto search = capsules.find(std::make_tuple(r, h));
	if (search == capsules.end())
	{
		auto results = capsules.insert(std::make_pair(std::make_tuple(r, h), new btCapsuleShape(r, h)));
		return (results.first)->second;
	}
	else return capsules[std::make_tuple(r, h)];
}

btStaticPlaneShape* noob::stage::plane(const noob::vec3& normal, float offset)
{
	auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
	if (search == planes.end())
	{
		auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), new btStaticPlaneShape(btVector3(normal.v[0], normal.v[1], normal.v[2]), offset)));
		return (results.first)->second;
	}
	else return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
}
