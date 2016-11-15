#include "Physics.hpp"

#include <functional>

noob::physics::~physics() noexcept(true)
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}

void noob::physics::init(const noob::vec3& gravity, noob::duration timestep) noexcept(true)
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

void noob::physics::clear() noexcept(true)
{
	for (size_t i = 0; i < bodies.size(); ++i)
	{
		remove_body(noob::physics::body_handle::make(i));
	}

	bodies.reserve(0);

	// Joints are automatically cleaned up by Bullet once all bodies have been destroyed. I think...
	joints.empty();

	for (size_t i = 0; i < ghosts.size(); ++i)
	{
		remove_ghost(noob::physics::ghost_handle::make(i));
	}

	ghosts.reserve(0);

}

void noob::physics::set_gravity(const noob::vec3&) noexcept(true)
{

}

void noob::physics::set_timestep(const noob::duration) noexcept(true)
{

}

noob::vec3 noob::physics::get_gravity() const noexcept(true)
{

}

noob::duration noob::physics::get_timestep() const noexcept(true)
{

}

noob::physics::body_handle noob::physics::add_body(noob::body_type, const noob::physics::shape_handle, float mass, const noob::vec3& position, const noob::versor& orientation, bool ccd) noexcept(true)
{

}

noob::physics::body_handle noob::physics::add_body(noob::body_type, const noob::physics::shape_handle, const noob::body::info&) noexcept(true)
{

}

noob::physics::ghost_handle noob::physics::add_ghost(const noob::physics::shape_handle, const noob::vec3&, const noob::versor&) noexcept(true)
{

}

noob::body& noob::physics::get_body(noob::physics::body_handle h) noexcept(true)
{
	return bodies[h.index()];
}

noob::ghost& noob::physics::get_ghost(noob::physics::ghost_handle h) noexcept(true)
{
	return ghosts[h.index()];
}

void noob::physics::remove_body(noob::physics::body_handle h) noexcept(true)
{
	noob::body temp = bodies[h.index()];
	if (temp.physics_valid)
	{
		dynamics_world->removeRigidBody(temp.inner);
		delete temp.inner;
		temp.physics_valid = false;
	}
}

void noob::physics::remove_ghost(noob::physics::ghost_handle h) noexcept(true) 
{
	noob::ghost temp = ghosts[h.index()];
	dynamics_world->removeCollisionObject(temp.inner);
	delete temp.inner;
}
