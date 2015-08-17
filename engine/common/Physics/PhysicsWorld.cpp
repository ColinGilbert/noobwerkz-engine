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


void add(const noob::physics_shape& shape, const std::string& name)
{
	
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
