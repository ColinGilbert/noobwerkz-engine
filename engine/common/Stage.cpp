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

	set_model_name(add_model(noob::basic_mesh::sphere(0.5)), "unit-sphere");
	set_model_name(add_model(noob::basic_mesh::cube(1.0, 1.0, 1.0)), "unit-cube");
	set_model_name(add_model(noob::basic_mesh::cylinder(0.5, 1.0)), "unit-cylinder");
	set_model_name(add_model(noob::basic_mesh::cone(0.5, 1.0)), "unit-cone");

	set_skeleton_name(add_skeleton("seymour.skel.ozz"), "human");
	
	noob::basic_renderer::uniform_info dbg_shader;
	dbg_shader.colour = noob::vec4(0.0, 0.3, 0.3, 1.0);
	set_light_name(add_light(noob::light()), "default");
	set_reflectance_name(add_reflectance(noob::reflectance()), "default");
	
	set_shader_name(add_shader(dbg_shader),"debug");

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
	for (size_t i = 0; i < actors.size(); ++i)
	{
		noob::actor* a = actors[i].get();
		// a->update();//	draw(a->drawable, a->get_transform());
	}

}


void noob::stage::draw() const
{
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

	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::draw(noob::drawable* d, const noob::mat4& transform) const
{
	shaders.draw(d->get_model(),*(d->get_shading()), transform);
}


size_t noob::stage::add_model(const std::string& filename)
{
	models.push_back(std::make_unique<noob::model>(filename));
	return models.size() - 1;
}


size_t noob::stage::add_model(const noob::basic_mesh& _mesh)
{
	models.push_back(std::make_unique<noob::model>(_mesh));
	return models.size() - 1;
}


size_t noob::stage::add_drawable(size_t model, size_t light, size_t reflectance, size_t shading, const noob::vec3& _scale)
{
	drawables.push_back(std::make_unique<noob::drawable>(get_model(model), get_light(light), get_reflectance(reflectance), get_shader(shading), _scale));
	return drawables.size() - 1;
}


size_t noob::stage::add_skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> s = std::make_unique<noob::skeletal_anim>();
	s->init(filename);
	skeletons.push_back(std::move(s));
	return skeletons.size() - 1;
}


size_t noob::stage::add_actor(size_t drawable, size_t skeletal_anim, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::actor> a = std::make_unique<noob::actor>();
	a->init(dynamics_world, get_drawable(drawable), get_skeleton(skeletal_anim));
	a->set_position(_position);
	a->set_orientation(_orientation);
	actors.push_back(std::move(a));
	return actors.size() - 1;
}


size_t noob::stage::add_prop(btRigidBody* body, size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::prop> p = std::make_unique<noob::prop>();
	p->init(body, get_drawable(drawable));
	p->set_position(_position);
	p->set_orientation(_orientation);
	props.push_back(std::move(p));
	return props.size() - 1;
}


size_t noob::stage::add_scenery(size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	std::unique_ptr<noob::scenery> s = std::make_unique<noob::scenery>();
	s->init(dynamics_world, get_drawable(drawable), _position, _orientation);
	sceneries.push_back(std::move(s));
	return sceneries.size() - 1;
}


size_t noob::stage::add_light(const noob::light& arg)
{
	lights.push_back(arg);
	return lights.size() - 1;
}


size_t noob::stage::add_reflectance(const noob::reflectance& arg)
{
	reflectances.push_back(arg);
	return reflectances.size() - 1;
}


size_t noob::stage::add_shader(const noob::prepared_shaders::info& arg)
{
	shader_uniforms.push_back(arg);
	return shader_uniforms.size() - 1;
}


void noob::stage::set_light(size_t index, const noob::light& arg)
{
	if (index < lights.size())
	{
		lights[index] = arg;
	}
}


void noob::stage::set_reflectance(size_t index, const noob::reflectance& arg)
{
	if (index < reflectances.size())
	{
		reflectances[index] = arg;
	}
}


void noob::stage::set_shader(size_t index, const noob::prepared_shaders::info& arg)
{
	if (index < shader_uniforms.size())
	{
		shader_uniforms[index] = arg;
	}
}


void noob::stage::set_model_name(size_t index, const std::string& name)
{
	if (model_exists(index))
	{
		model_names[name] = index;
	}
}


void noob::stage::set_drawable_name(size_t index, const std::string& name)
{
	if (drawable_exists(index))
	{
		drawable_names[name] = index;
	}
}


void noob::stage::set_skeleton_name(size_t index, const std::string& name)
{
	if (skeleton_exists(index))
	{
		skeleton_names[name] = index;
	}
}


void noob::stage::set_actor_name(size_t index, const std::string& name)
{
	if (actor_exists(index))
	{
		actor_names[name] = index;
	}
}


void noob::stage::set_prop_name(size_t index, const std::string& name)
{
	if (prop_exists(index))
	{
		prop_names[name] = index;
	}
}


void noob::stage::set_scenery_name(size_t index, const std::string& name)
{
	if (scenery_exists(index))
	{
		scenery_names[name] = index;
	}
}


void noob::stage::set_light_name(size_t index, const std::string& name)
{
	if (light_exists(index))
	{
		light_names[name] = index;
	}
}


void noob::stage::set_reflectance_name(size_t index, const std::string& name)
{
	if (reflectance_exists(index))
	{
		reflectance_names[name] = index;
	}
}


void noob::stage::set_shader_name(size_t index, const std::string& name)
{
	if (shader_exists(index))
	{
		shading_names[name] = index;
	}
}


size_t noob::stage::get_model_id(const std::string& name) const
{
	auto search = model_names.find(name);
	if (search == model_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_drawable_id(const std::string& name) const
{
	auto search = drawable_names.find(name);
	if (search == drawable_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_skeleton_id(const std::string& name) const
{
	auto search = skeleton_names.find(name);
	if (search == skeleton_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_actor_id(const std::string& name) const
{
	auto search = actor_names.find(name);
	if (search == actor_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_prop_id(const std::string& name) const
{
	auto search = prop_names.find(name);
	if (search == prop_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_scenery_id(const std::string& name) const
{
	auto search = scenery_names.find(name);
	if (search == scenery_names.end())
	{
		return 0;
	}
	return search->second;

}


size_t noob::stage::get_light_id(const std::string& name) const
{
	auto search = light_names.find(name);
	if (search == light_names.end())
	{
		return 0;
	}
	return search->second;
}

size_t noob::stage::get_reflectance_id(const std::string& name) const
{
	auto search = reflectance_names.find(name);
	if (search == reflectance_names.end())
	{
		return 0;
	}
	return search->second;
}


size_t noob::stage::get_shader_id(const std::string& name) const
{
	auto search = shading_names.find(name);
	if (search == shading_names.end())
	{
		return 0;
	}
	return search->second;
}


noob::model* noob::stage::get_model(size_t _index) const
{
	if (!model_exists(_index))
	{
		return models[0].get();
	}
	return models[_index].get();
}


noob::drawable* noob::stage::get_drawable(size_t _index) const
{
	if (!drawable_exists(_index))
	{
		return drawables[0].get();
	}
	return drawables[_index].get();

}


noob::skeletal_anim* noob::stage::get_skeleton(size_t _index) const
{
	if (!skeleton_exists(_index))
	{
		return skeletons[0].get();
	}
	return skeletons[_index].get();

}


noob::actor* noob::stage::get_actor(size_t _index) const
{
	if (!actor_exists(_index))
	{
		return actors[0].get();
	}
	return actors[_index].get();
}


noob::prop* noob::stage::get_prop(size_t _index) const
{
	if (!prop_exists(_index))
	{
		return props[0].get();
	}
	return props[_index].get();

}


noob::scenery* noob::stage::get_scenery(size_t _index) const
{
	if (!scenery_exists(_index))
	{
		return sceneries[0].get();
	}
	return sceneries[_index].get();
}


const noob::light* noob::stage::get_light(size_t _index) const
{
	if (!light_exists(_index))
	{
		return &lights[0];
	}
	return &lights[_index];
}


const noob::reflectance* noob::stage::get_reflectance(size_t _index) const
{
	if (!reflectance_exists(_index))
	{
		return &reflectances[0];
	}
	return &reflectances[_index];
}


const noob::prepared_shaders::info* noob::stage::get_shader(size_t _index) const
{
	if (!shader_exists(_index))
	{
		return &shader_uniforms[0];
	}
	return &shader_uniforms[_index];
}


bool noob::stage::model_exists(size_t index) const
{
	return (index <= models.size());
}


bool noob::stage::drawable_exists(size_t index) const
{
	return (index <= drawables.size());
}


bool noob::stage::skeleton_exists(size_t index) const
{
	return (index <= skeletons.size());
}


bool noob::stage::actor_exists(size_t index) const
{
	return (index <= actors.size());
}


bool noob::stage::prop_exists(size_t index) const
{
	return (index <= props.size());
}


bool noob::stage::scenery_exists(size_t index) const
{
	return (index <= sceneries.size());
}


bool noob::stage::light_exists(size_t index) const
{
	return (index <= lights.size());
}


bool noob::stage::reflectance_exists(size_t index) const 
{
	return (index <= reflectances.size());
}


bool noob::stage::shader_exists(size_t index) const
{
	return (index <= shader_uniforms.size());
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
