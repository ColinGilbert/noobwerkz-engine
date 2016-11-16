#include "Physics.hpp"
#include "Globals.hpp"
#include "MathFuncs.hpp"

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
	set_gravity(noob::vec3(0.0, -10.0, 0.0));
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}


void noob::physics::step(noob::duration_fp arg) noexcept(true)
{
	dynamics_world->stepSimulation(arg.count() / noob::billion);
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


void noob::physics::set_gravity(const noob::vec3& arg) noexcept(true)
{
	dynamics_world->setGravity(btVector3(arg[0], arg[1], arg[2]));
}


noob::vec3 noob::physics::get_gravity() const noexcept(true)
{
	return noob::vec3_from_bullet(dynamics_world->getGravity());
}


noob::physics::body_handle noob::physics::add_body(noob::body_type b_type, const noob::physics::shape_handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();

	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	
	bodies.push_back(b);
	
	return noob::physics::body_handle::make(bodies.size() - 1);
}


noob::physics::body_handle noob::physics::add_body(noob::body_type type_arg, const noob::physics::shape_handle shape_h, const noob::body::info& info_arg) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();

	noob::body b;
	b.init(dynamics_world, type_arg, g.shapes.get(shape_h), info_arg);
	bodies.push_back(b);
	
	return noob::physics::body_handle::make(bodies.size() - 1);
}


noob::physics::ghost_handle noob::physics::add_ghost(const noob::physics::shape_handle shape_h, const noob::vec3& pos, const noob::versor& orient) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();

	noob::ghost temp;
	temp.init(dynamics_world, g.shapes.get(shape_h), pos, orient);
	ghosts.push_back(temp);
	
	return noob::physics::ghost_handle::make(ghosts.size() - 1);
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
