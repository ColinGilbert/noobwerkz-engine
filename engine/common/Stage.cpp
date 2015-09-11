#include "Stage.hpp"

bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	renderer.init();
/*
	set_model_name(make_model(noob::basic_mesh::sphere(0.5)), "unit-sphere");
	set_model_name(make_model(noob::basic_mesh::cube(1.0, 1.0, 1.0)), "unit-cube");
	set_model_name(make_model(noob::basic_mesh::cylinder(0.5, 1.0)), "unit-cylinder");
	set_model_name(make_model(noob::basic_mesh::cone(0.5, 1.0)), "unit-cone");

	set_skeleton_name(make_skeleton("seymour.skel.ozz"), "human");

	set_light_name(make_light(noob::light()), "default");
	set_reflectance_name(make_reflectance(noob::reflectance()), "default");

	noob::basic_rendererr::uniform_info dbg_shader;
	dbg_shader.colour = noob::vec4(0.0, 0.3, 0.3, 1.0);	
	set_shader_name(make_shader(dbg_shader), "debug");
*/
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
	dynamics_world->stepSimulation(1.0/60.0, 10);
	//for (size_t i = 0; i < actors.size(); ++i)
	// {
	// 	noob::actor* a = actors[i].get();
	// 	a->update();
	// }
}


void noob::stage::draw() const
{
/*
	for (size_t i = 0; i < actors.size(); ++i)
	{
		noob::actor* a = actors[i].get();
		draw(a->drawable, a->get_transform());
	}
	for (size_t i = 0; i < props.size(); ++i)
	{
		noob::prop* p = props[i].get();
		draw(p->drawable, p->get_transform());
	}
	for (size_t i = 0; i < sceneries.size(); ++i)
	{
		noob::scenery* s = sceneries[i].get();
		draw(s->drawable, s->get_transform());
	}
*/
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


// void noob::stage::draw(noob::drawable* d, const noob::mat4& transform) const
// {
// 	renderer.draw(d->get_model_ptr(),*(d->get_shading_ptr()), transform);
// }

/*
size_t noob::stage::make_model(const std::string& filename)
{
	models.push_back(std::make_unique<noob::model>(filename));
	return models.size() - 1;
}


size_t noob::stage::make_model(const noob::basic_mesh& _mesh)
{
	models.push_back(std::make_unique<noob::model>(_mesh));
	return models.size() - 1;
}


size_t noob::stage::make_drawable(size_t model, size_t light, size_t reflectance, size_t shading, const noob::vec3& _scale)
{
	drawables.push_back(std::make_unique<noob::drawable>(get_model_ptr(model), get_light_ptr(light), get_reflectance_ptr(reflectance), get_shader_ptr(shading), _scale));
	return drawables.size() - 1;
}


size_t noob::stage::make_skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> s = std::make_unique<noob::skeletal_anim>();
	s->init(filename);
	skeletons.push_back(std::move(s));
	return skeletons.size() - 1;
}


size_t noob::stage::make_actor(size_t drawable, size_t skeletal_anim, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::actor> a = std::make_unique<noob::actor>();
	a->init(dynamics_world, get_drawable_ptr(drawable), get_skeleton_ptr(skeletal_anim));
	a->set_position(_position);
	a->set_orientation(_orientation);
	actors.push_back(std::move(a));
	return actors.size() - 1;
}


size_t noob::stage::make_prop(btRigidBody* body, size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::prop> p = std::make_unique<noob::prop>();
	p->init(body, get_drawable_ptr(drawable));
	p->set_position(_position);
	p->set_orientation(_orientation);
	props.push_back(std::move(p));
	return props.size() - 1;
}


size_t noob::stage::make_scenery(size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::scenery> s = std::make_unique<noob::scenery>();
	s->init(dynamics_world, get_drawable_ptr(drawable), _position, _orientation);
	sceneries.push_back(std::move(s));
	return sceneries.size() - 1;
}


size_t noob::stage::make_light(const noob::light& arg)
{
	lights.push_back(arg);
	return lights.size() - 1;
}


size_t noob::stage::make_reflectance(const noob::reflectance& arg)
{
	reflectances.push_back(arg);
	return reflectances.size() - 1;
}


size_t noob::stage::make_shader(const noob::prepared_shaders::info& arg)
{
	shader_uniforms.push_back(arg);
	return shader_uniforms.size() - 1;
}
*/

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
