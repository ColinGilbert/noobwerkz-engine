#include "PhysicsWorld.hpp"
#include "Logger.hpp"

noob::physics_world::~physics_world()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete dispatcher;
	delete broadphase;
}


void noob::physics_world::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -9.81, 0));
}


void noob::physics_world::step(double delta)
{
	double seconds = delta / 1000.0f;
	auto max_substeps = 2;
	double fixed_timestep = 1.0/60.0f;
	dynamics_world->stepSimulation(seconds, max_substeps, fixed_timestep);
	if (seconds > max_substeps * fixed_timestep)
	{
		logger::log(fmt::format("[GameWorld] Warning: Timestep of {0} seconds is greater than max acceptable {1}", seconds, max_substeps * fixed_timestep));
	}
}


void noob::physics_world::apply_global_force(const noob::vec3& force, const noob::physics_shape& shape, const noob::vec3& origin)
{
	
}


void noob::physics_world::add(const noob::physics_body& rigid_body, short collision_group, short collides_with)
{
	dynamics_world->addRigidBody(rigid_body.get_raw_ptr(), collision_group, collides_with);
}


void noob::physics_world::add(const noob::physics_constraint_generic& constraint)
{
	dynamics_world->addConstraint(constraint.get_raw_ptr());
}


std::shared_ptr<noob::physics_shape> noob::physics_world::sphere(float radius)
{
	if(spheres.find(radius) == spheres.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_sphere(radius);
		spheres[radius] = a;
		return a;
	}
	else return spheres[radius];
}

std::shared_ptr<noob::physics_shape> noob::physics_world::box(float width, float height, float depth)
{
	std::array<float, 3> dims;
	dims[0] = width;
	dims[1] = height;
	dims[2] = depth;
	if(boxes.find(dims) == boxes.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_box(width, height, depth);
		boxes[dims] = a;
		return a;
	}
	else return boxes[dims];
}


std::shared_ptr<noob::physics_shape> noob::physics_world::cylinder(float radius, float height)
{
	std::array<float, 2> dims;
	dims[0] = radius;
	dims[1] = height;
	if(cylinders.find(dims) == cylinders.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_cylinder(radius, height);
		cylinders[dims] = a;
		return a;
	}
	else return cylinders[dims];
}


std::shared_ptr<noob::physics_shape> noob::physics_world::capsule(float radius, float height)
{
	std::array<float, 2> dims;
	dims[0] = radius;
	dims[1] = height;
	if(capsules.find(dims) == capsules.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_capsule(radius, height);
		capsules[dims] = a;
		return a;
	}
	else return capsules[dims];

}


std::shared_ptr<noob::physics_shape> noob::physics_world::cone(float radius, float height)
{
	std::array<float, 2> dims;
	dims[0] = radius;
	dims[1] = height;
	if(cones.find(dims) == cones.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_cone(radius, height);
		cones[dims] = a;
		return a;
	}
	else return cones[dims];

}


std::shared_ptr<noob::physics_shape> noob::physics_world::plane(const noob::vec3& normal, float constant)
{
	std::array<float, 4> normalconstant;
	normalconstant[0] = normal.v[0];
	normalconstant[1] = normal.v[1];
	normalconstant[2] = normal.v[2];
	normalconstant[3] = constant;
	if(planes.find(normalconstant) == planes.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_static_plane(normal, constant);
		planes[normalconstant] = a;
		return a;
	}
	else return planes[normalconstant];

}


std::shared_ptr<noob::physics_shape> noob::physics_world::set_convex_hull(const std::vector<noob::vec3>& points, const std::string& name)
{
	if (convex_hulls.find(name) == convex_hulls.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_convex_hull(points);
		convex_hulls[name] = a;
		return a;
	}
	else return convex_hulls[name];
}


std::shared_ptr<noob::physics_shape> noob::physics_world::set_multisphere(const std::vector<noob::vec4>& radii_pos, const std::string& name)
{
	if (multispheres.find(name) == multispheres.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_multisphere(radii_pos);
		multispheres[name] = a;
		return a;
	}
	return multispheres[name];

}


std::shared_ptr<noob::physics_shape> noob::physics_world::set_compound_shape(const std::vector<std::shared_ptr<noob::physics_shape>>& shapes, const std::string& name)
{
	if (compound_shapes.find(name) == compound_shapes.end())
	{
		auto a = std::make_shared<noob::physics_shape>();
		a->set_compound(shapes);
		compound_shapes[name] = a;
		return a;
	}
	return compound_shapes[name];
}


std::shared_ptr<noob::physics_shape> noob::physics_world::set_trimesh(const std::vector<noob::vec3>& points, const std::vector<uint16_t>& indices, const std::string& name)
{
	if (trimeshes.find(name) == trimeshes.end())
	{
		//auto a = std::make_shared<noob::physics_shape>();
		auto t = std::make_tuple<std::shared_ptr<noob::physics_shape>, std::vector<noob::vec3>, std::vector<uint16_t>>(std::make_shared<noob::physics_shape>(),static_cast<std::vector<noob::vec3>>(points),static_cast<std::vector<uint16_t>>(indices));
		std::get<0>(t)->set_static_mesh(std::get<1>(t), std::get<2>(t));
		trimeshes[name] = t;
		return std::get<0>(t);
	}
	else return std::get<0>(trimeshes[name]);
}


std::weak_ptr<noob::physics_shape> noob::physics_world::get_convex_hull(const std::string& name)
{
	auto search = convex_hulls.find(name);
	if (search == convex_hulls.end())
	{
		return {};
	}
	else return search->second;
}


std::weak_ptr<noob::physics_shape> noob::physics_world::get_multisphere(const std::string& name)
{
	auto search = multispheres.find(name);
	if (search == multispheres.end())
	{
		return {};
	}
	else return search->second;
}


std::weak_ptr<noob::physics_shape> noob::physics_world::get_compound_shape(const std::string& name)
{
	auto search = compound_shapes.find(name);
	if (search == compound_shapes.end())
	{
		return {};
	}
	else return search->second;
}


std::weak_ptr<noob::physics_shape> noob::physics_world::get_trimesh(const std::string& name)
{
	auto search = trimeshes.find(name);
	if (search == trimeshes.end())
	{
		return {};
	}
	else return std::get<0>(search->second);
}


void noob::physics_world::remove(const noob::physics_body& body)
{
	dynamics_world->removeRigidBody(body.get_raw_ptr());
}


void noob::physics_world::remove(const noob::physics_constraint_generic& constraint)
{
	dynamics_world->removeConstraint(constraint.get_raw_ptr());
}


btDynamicsWorld* noob::physics_world::get_raw_ptr() const
{
	return dynamics_world;
}
