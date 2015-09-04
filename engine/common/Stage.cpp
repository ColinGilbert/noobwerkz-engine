#include "Stage.hpp"

bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	
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
	//static double accum = 0.0;
	//if (!paused)
	//{
	//	accum += dt;
	//	if (accum >= 1.0 / 60.0)
		{
			dynamics_world->stepSimulation(1.0/60.0, 10);
	//		accum -= 1.0/60;
	//	}
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
		draw(a.second->get_drawable(), a.second->get_transform());
	}
	for (auto p : props)
	{
		draw(p.second->get_drawable(), p.second->get_transform());
	}
	for (auto s : sceneries)
	{
		draw(s.second->get_drawable(), s.second->get_transform());
	}
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::draw(noob::drawable* d, const noob::mat4& transform) const
{
	shaders.draw(d->get_model(),*(d->get_shading()), transform);
}


std::shared_ptr<noob::actor> noob::stage::make_actor(const std::string& _name, const std::shared_ptr<noob::drawable>& _drawable, const std::shared_ptr<noob::skeletal_anim>& _anim, const noob::vec3& _position, const noob::versor& _orientation)
{
	auto a = std::make_shared<noob::actor>();
	a->init(dynamics_world, _drawable, _anim);
	a->set_position(_position);
	a->set_orientation(_orientation);
	actors[_name] = a;
	return a;
}


std::shared_ptr<noob::prop> noob::stage::make_prop(const std::string& _name, btRigidBody* _body, const std::shared_ptr<noob::drawable>& _drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	auto p = std::make_shared<noob::prop>();
	p->init(_body, _drawable);
	p->set_position(_position);
	p->set_orientation(_orientation);
	props[_name] = p;
	return p;

}


std::shared_ptr<noob::scenery> noob::stage::make_scenery(const std::string& _name, const std::shared_ptr<noob::drawable>& _drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::shared_ptr<noob::scenery> s = std::make_shared<noob::scenery>(noob::scenery(dynamics_world, _drawable, _position, _orientation));
	sceneries[_name] = s;
	return s;
}


std::shared_ptr<noob::drawable> noob::stage::make_drawable(const std::string& _name, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::light>& _light, const std::shared_ptr<noob::reflectance>& _reflectance, const std::shared_ptr<noob::prepared_shaders::info>& _shading, const noob::vec3& _scale) 
{
	std::shared_ptr<noob::drawable> d = std::make_shared<noob::drawable>(_model, _light, _reflectance, _shading, _scale);
	drawables[_name];
	return d;	
}


std::shared_ptr<noob::reflectance> noob::stage::reflectance(const std::string& _name, const noob::reflectance& _reflectance)
{
	std::shared_ptr<noob::reflectance> r = std::make_shared<noob::reflectance>(_reflectance);
	reflectances[_name] = r;
	return r;
}


std::weak_ptr<noob::reflectance> noob::stage::reflectance(const std::string& _name) const
{
	auto search = reflectances.find(_name);
	if (search == reflectances.end())
	{
		return {};
	}
	return reflectances[_name];
}


std::shared_ptr<noob::light> noob::stage::light(const std::string& _name, const noob::light& _light)
{
	std::shared_ptr<noob::light> l = std::make_shared<noob::light>(_light);
	lights[_name] = l;
	return l;

}


std::weak_ptr<noob::light> noob::stage::light(const std::string& _name) const
{
	auto search = lights.find(_name);
	if (search == lights.end())
	{
		return {};
	}
	return lights[_name];
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


std::weak_ptr<noob::drawable> noob::stage::get_drawable(const std::string& _name) const
{
	return drawables[_name];
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


btConvexHullShape* noob::stage::hull(const std::vector<noob::vec3>& points)
{
	btConvexHullShape* temp = new btConvexHullShape();
	for (noob::vec3 p : points)
	{
		temp->addPoint(btVector3(p.v[0], p.v[1], p.v[2]));
	}
	return temp;
}

/*
btCompoundShape* noob::stage::breakable_mesh(const noob::basic_mesh& _mesh)
{
	std::vector<noob::basic_mesh> convex_meshes;
	return breakable_mesh(_mesh.convex_decomposition());
}


btCompoundShape* noob::stage::breakable_mesh(const std::vector<noob::basic_mesh>& _meshes)
{
	btCompoundShape* result = new btCompoundShape();
	for (noob::basic_mesh m : _meshes)
	{
		btConvexHullShape* temp = hull(m.vertices); 
		btTransform t;
		t.setIdentity();
		result->addChildShape(t,temp);
	}
	return result; 
}
*/

/*
btBvhTriangleMeshShape*> noob::stage::static_mesh(const noob::basic_mesh&)
{
	btBvhTriangleMeshShape* temp = new btBvhTriangleMeshShape();
	for (noob::vec3 p : points)
	{
		temp->addPoint(btVector3(p.v[0], p.v[1], p.v[2]));
	}
	return temp;

}
*/
