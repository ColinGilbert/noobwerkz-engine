#include "Physics.hpp"


noob::physics::~physics() noexcept(true)
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}


void noob::physics::init() noexcept(true)
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	set_gravity(noob::vec3f(0.0, -10.0, 0.0));
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}


void noob::physics::step(float arg) noexcept(true)
{
	dynamics_world->stepSimulation(arg);
}


void noob::physics::clear() noexcept(true)
{
	for (size_t i = 0; i < bodies.size(); ++i)
	{
		remove_body(noob::body_handle::make(i));
	}

	bodies.reserve(0);

	// Joints are automatically cleaned up by Bullet once all bodies have been destroyed. I think...
	joints.empty();

	for (size_t i = 0; i < ghosts.size(); ++i)
	{
		remove_ghost(noob::ghost_handle::make(i));
	}

	ghosts.reserve(0);
}


void noob::physics::set_gravity(const noob::vec3f& arg) noexcept(true)
{
	dynamics_world->setGravity(btVector3(arg[0], arg[1], arg[2]));
}


noob::vec3f noob::physics::get_gravity() const noexcept(true)
{
	return noob::vec3f_from_bullet(dynamics_world->getGravity());
}


noob::body_handle noob::physics::add_body(noob::body_type b_type, const noob::shape_handle shape_h, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd) noexcept(true)
{
	noob::globals& g = noob::get_globals();

	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	
	bodies.push_back(b);
	
	return noob::body_handle::make(bodies.size() - 1);
}


noob::body_handle noob::physics::add_body(noob::body_type type_arg, const noob::shape_handle shape_h, const noob::body::info& info_arg) noexcept(true)
{
	noob::globals& g = noob::get_globals();

	noob::body b;
	b.init(dynamics_world, type_arg, g.shapes.get(shape_h), info_arg);
	bodies.push_back(b);
	
	return noob::body_handle::make(bodies.size() - 1);
}


noob::ghost_handle noob::physics::add_ghost(const noob::shape_handle shape_h, const noob::vec3f& pos, const noob::versorf& orient) noexcept(true)
{
	noob::globals& g = noob::get_globals();

	noob::ghost temp;
	temp.init(dynamics_world, g.shapes.get(shape_h), pos, orient);
	ghosts.push_back(temp);
	
	return noob::ghost_handle::make(ghosts.size() - 1);
}


noob::body& noob::physics::get_body(noob::body_handle h) noexcept(true)
{
	return bodies[h.index()];
}


noob::ghost& noob::physics::get_ghost(noob::ghost_handle h) noexcept(true)
{
	return ghosts[h.index()];
}


std::vector<noob::contact_point> noob::physics::get_intersecting(const noob::ghost_handle ghost_h) const noexcept(true) 
{
	noob::ghost temp_ghost = ghosts[ghost_h.index()];

	btManifoldArray manifold_array;

	btBroadphasePairArray& pair_array = temp_ghost.inner->getOverlappingPairCache()->getOverlappingPairArray();

	std::vector<noob::contact_point> results;

	size_t num_pairs = pair_array.size();

	for (size_t i = 0; i < num_pairs; ++i)
	{
		manifold_array.clear();

		const btBroadphasePair& pair = pair_array[i];

		btBroadphasePair* collision_pair = dynamics_world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

		if (!collision_pair)
		{
			continue;
		}

		if (collision_pair->m_algorithm)
		{
			collision_pair->m_algorithm->getAllContactManifolds(manifold_array);
		}

		for (size_t j = 0; j < manifold_array.size(); ++j)
		{
			btPersistentManifold* manifold = manifold_array[j];
			const btCollisionObject* bt_obj = manifold->getBody0();
			// Sanity check
			const uint32_t index = static_cast<uint32_t>(bt_obj->getUserIndex2());
			if (index != std::numeric_limits<uint32_t>::max())
			{
				// btScalar direction = is_first_body ? btScalar(-1.0) : btScalar(1.0);
				for (size_t p = 0; p < manifold->getNumContacts(); ++p)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.0f)
					{
						noob::contact_point cp;

						cp.item_type = static_cast<noob::stage_item_type>(bt_obj->getUserIndex());
						cp.index = index;
						cp.pos_a = vec3f_from_bullet(pt.getPositionWorldOnA());
						cp.pos_b = vec3f_from_bullet(pt.getPositionWorldOnB());
						cp.normal_on_b = vec3f_from_bullet(pt.m_normalWorldOnB);

						results.push_back(cp);
					}
				}
			}
			else
			{
				logger::log(noob::importance::ERROR, "[Stage] Invalid objects found during collision");
			}
		}
	}

	return results;
}


void noob::physics::remove_body(noob::body_handle h) noexcept(true)
{
	noob::body temp = bodies[h.index()];
	if (temp.physics_valid)
	{
		dynamics_world->removeRigidBody(temp.inner);
		delete temp.inner;
		temp.physics_valid = false;
	}
}


void noob::physics::remove_ghost(noob::ghost_handle h) noexcept(true) 
{
	noob::ghost temp = ghosts[h.index()];
	dynamics_world->removeCollisionObject(temp.inner);
	delete temp.inner;
}
