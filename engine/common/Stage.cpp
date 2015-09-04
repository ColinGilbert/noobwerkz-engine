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

	size_t sphere = add_model(noob::basic_mesh::sphere(0.5));
	size_t cube = add_model(noob::basic_mesh::cube(1.0, 1.0, 1.0));
	size_t cylinder = add_model(noob::basic_mesh::cylinder(1.0, 1.0));
	size_t cone = add_model(noob::basic_mesh::cone(1.0, 1.0));

	size_t skeleton = add_skeleton("seymour.skel.ozz");

	noob::basic_renderer::uniform_info dbg_shader;
	dbg_shader.colour = noob::vec4(0.0, 0.3, 0.3, 1.0);
	size_t debug_shader_id = add_shader(dbg_shader);

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
	/*for (auto actor_it : actors)
	  {
	  actor_it->update();//dt);//, true, false, true, false, false);//true);
	  }*/
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
	/*for (auto p : props)
	  {
	  draw(p->get_drawable(), p->get_transform());
	  }
	  for (auto s : sceneries)
	  {
	  draw(s->get_drawable(), s->get_transform());
	  }*/
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


void noob::stage::draw(noob::drawable* d, const noob::mat4& transform) const
{
	shaders.draw(d->get_model(),*(d->get_shading()), transform);
}


size_t noob::stage::add_model(const std::string& filename)
{
	return 0;
}


size_t noob::stage::add_model(const noob::basic_mesh&)
{
	return 0;
}


size_t noob::stage::add_drawable(size_t model, size_t light, size_t reflectance, size_t shading, const noob::vec3& _scale)
{
	return 0;
}


size_t noob::stage::add_skeleton(const std::string& filename)
{
	return 0;
}


size_t noob::stage::add_actor(size_t drawable, size_t skeletal_anim, const noob::vec3& _position, const noob::versor& _orientation)
{
	return 0;
}


size_t noob::stage::add_prop(btRigidBody*, size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	return 0;
}


size_t noob::stage::add_scenery(size_t drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	return 0;
}


size_t noob::stage::add_light(const noob::light& arg)
{
	return 0;
}


size_t noob::stage::add_reflectance(const noob::reflectance& arg)
{
	return 0;
}


size_t noob::stage::add_shader(const noob::prepared_shaders::info& arg)
{
	return 0;
}


void noob::stage::set_light(size_t index, const noob::light& arg)
{

}


void noob::stage::set_reflectance(size_t index, const noob::reflectance& arg)
{

}


void noob::stage::set_shader(size_t index, const noob::prepared_shaders::info& arg)
{

}



size_t noob::stage::get_model_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_drawable_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_skeleton_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_actor_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_prop_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_scenery_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_light_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_reflectance_id(const std::string&) const
{
	return 0;
}


size_t noob::stage::get_shader_id(const std::string&) const
{
	return 0;
}


noob::model* noob::stage::get_model(size_t) const
{
	return nullptr;
}


noob::drawable* noob::stage::get_drawable(size_t) const
{
	return nullptr;
}


noob::skeletal_anim* noob::stage::get_skeleton(size_t) const
{
	return nullptr;
}


noob::actor* noob::stage::get_actor(size_t) const
{
	return nullptr;
}


noob::prop* noob::stage::get_prop(size_t) const
{
	return nullptr;
}


noob::scenery* noob::stage::get_scenery(size_t) const
{
	return nullptr;
}


noob::light* noob::stage::get_light(size_t) const
{
	return nullptr;
}


noob::reflectance* noob::stage::get_reflectance(size_t) const
{
	return nullptr;
}


noob::prepared_shaders::info* noob::stage::get_shader(size_t) const
{
	return nullptr;
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
