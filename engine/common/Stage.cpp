#include "Stage.hpp"
// #include "Shiny.h"

noob::stage::~stage() noexcept(true) 
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}

void noob::stage::init() noexcept(true) 
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	// For the ghost object to work correctly, we need to add a callback to our world.
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	draw_graph.reserve_nodes(8192);
	draw_graph.reserve_edges(8192);
	draw_graph.add_node();
	node_masks.push_back(0);

	noob::light temp;
	temp.rgb_falloff = noob::vec4(1.0, 1.0, 1.0, 0.6);
	temp.pos_radius = noob::vec4(0.0, 450.0, 0.0, 500.0);
	directional_light = temp;

	logger::log(noob::importance::INFO, "[Stage] Done init.");
}

void noob::stage::tear_down() noexcept(true) 
{
	for (size_t i = 0; i < bodies.count(); ++i)
	{
		remove_body(body_handle::make(i));
	}

	bodies.empty();

	// Joints are automatically cleaned up by Bullet once all bodies have been destroyed. I think...
	joints.empty();

	for (size_t i = 0; i < ghosts.count(); ++i)
	{
		remove_ghost(ghost_handle::make(i));
	}

	ghosts.empty();

	actors.empty();
	//actor_mq.clear();
	// actor_mq_count = 0;

	// sceneries.empty();

	delete dynamics_world;
	delete broadphase;
	delete solver;
	delete collision_dispatcher;
	delete collision_configuration;

	draw_graph.clear();
	// node_masks.empty();

	init();
}


void noob::stage::update(double dt) noexcept(true) 
{
	// PROFILE_FUNC();
	noob::time start_time = noob::clock::now();

	//if (nav_changed)
	//{
	// build_navmesh();
	// nav_changed = false;
	//}

	dynamics_world->stepSimulation(1.0/60.0, 10);

	// update_particle_systems();
	update_actors();
	noob::time end_time = noob::clock::now();

	update_duration = end_time - start_time;

	noob::globals& g = noob::globals::get_instance();
	g.profile_run.stage_physics_duration += update_duration;
}

void noob::stage::draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) const noexcept(true) 
{
	// PROFILE_FUNC();


}


void noob::stage::build_navmesh() noexcept(true)
{
}


noob::body_handle noob::stage::body(const noob::body_type b_type, const noob::shape_handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();

	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	

	body_handle bod_h = bodies.add(b);
	// noob::body* b_ptr = std::get<1>(bodies.get_ptr_mutable(bod_h));

	return bod_h;
}


noob::ghost_handle noob::stage::ghost(const noob::shape_handle shape_h, const noob::vec3& pos, const noob::versor& orient) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();

	noob::ghost temp_ghost;
	temp_ghost.init(dynamics_world, g.shapes.get(shape_h), pos, orient);

	noob::ghost_handle ghost_h = ghosts.add(temp_ghost);

	return ghost_h;
}


noob::joint_handle noob::stage::joint(const noob::body_handle a, const noob::vec3& local_point_on_a, const noob::body_handle b, const noob::vec3& local_point_on_b) noexcept(true) 
{
	noob::body bod_a = bodies.get(a);
	noob::body bod_b = bodies.get(b);

	noob::joint j;

	noob::mat4 local_a = noob::translate(noob::identity_mat4(), local_point_on_a);
	noob::mat4 local_b = noob::translate(noob::identity_mat4(), local_point_on_b);

	j.init(dynamics_world, bod_a, bod_b, local_a, local_b);

	noob::joint_handle h = joints.add(j);

	return h;
}


noob::actor_handle noob::stage::actor(const noob::actor_blueprints_handle bp_h, uint32_t team, const noob::vec3& pos, const noob::versor& orient) 
{
	noob::globals& g = noob::globals::get_instance();
	const noob::actor_blueprints bp = g.actor_blueprints.get(bp_h);

	noob::actor a;
	a.ghost = ghost(bp.bounds, pos, orient);
	a.bp_handle = bp_h;

	noob::actor_handle a_h = actors.add(a);

	noob::stage_item_variant var;
	var.type = noob::stage_item_type::ACTOR;
	var.index = a_h.index();

	// add_to_graph(var);

	noob::ghost temp_ghost = ghosts.get(a.ghost);
	temp_ghost.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
	temp_ghost.inner->setUserIndex_2(a_h.index());

	return a_h;
}

/*
noob::scenery_handle noob::stage::scenery(const noob::shape_handle shape_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();

	const noob::body_handle bod_h = body(noob::body_type::STATIC, shape_arg, 0.0, pos_arg, orient_arg, false);

	noob::scenery sc;
	sc.body = bod_h;
	sc.reflect = reflect_arg;
	noob::scenery_handle scenery_h = sceneries.add(sc);

	noob::stage_item_variant var;
	var.type = noob::stage_item_type::SCENERY;
	var.index = scenery_h.index();

	add_to_graph(var);

	noob::body b = bodies.get(bod_h);
	b.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::SCENERY));
	b.inner->setUserIndex_2(scenery_h.index());

	logger::log(noob::importance::INFO, noob::concat("[Stage] Scenery added! Handle ", noob::to_string(scenery_h.index())) );

	return scenery_h;
}
*/

std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::ghost_handle ghost_h) const noexcept(true) 
{
	noob::ghost temp_ghost = ghosts.get(ghost_h);

	btManifoldArray manifold_array;

	btBroadphasePairArray& pairArray = temp_ghost.inner->getOverlappingPairCache()->getOverlappingPairArray();

	std::vector<noob::contact_point> results;

	size_t num_pairs = pairArray.size();

	for (size_t i = 0; i < num_pairs; ++i)
	{
		manifold_array.clear();

		const btBroadphasePair& pair = pairArray[i];

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
			const uint32_t index = bt_obj->getUserIndex_2();
			if (index != std::numeric_limits<uint32_t>::max())
			{
				// btScalar direction = is_first_body ? btScalar(-1.0) : btScalar(1.0);
				for (size_t p = 0; p < manifold->getNumContacts(); ++p)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.0f)
					{
						noob::contact_point cp;

						cp.item_type = static_cast<noob::stage_item_type>(bt_obj->getUserIndex_1());
						cp.index = index;
						cp.pos_a = vec3_from_bullet(pt.getPositionWorldOnA());
						cp.pos_b = vec3_from_bullet(pt.getPositionWorldOnB());
						cp.normal_on_b = vec3_from_bullet(pt.m_normalWorldOnB);

						results.push_back(cp);
					}
				}
			}
			else
			{
				logger::log(noob::importance::ERROR, "[Stage] DATA ERROR: Invalid objects found during collision");
			}
		}
	}

	return results;
}


std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::actor_handle ah) const noexcept(true)
{
	noob::actor a = actors.get(ah);
	return get_intersecting(a.ghost);
}


void noob::stage::remove_body(noob::body_handle h) noexcept(true) 
{
	noob::body b = bodies.get(h);
	if (b.physics_valid)
	{
		dynamics_world->removeRigidBody(b.inner);
		delete b.inner;
	}
}


void noob::stage::remove_ghost(noob::ghost_handle h) noexcept(true) 
{
	noob::ghost b = ghosts.get(h);
	dynamics_world->removeCollisionObject(b.inner);
	delete b.inner;
}


void noob::stage::update_actors() noexcept(true)
{
	for (uint32_t i = 0; i < actors.count(); ++i)
	{
		//noob::actor a = actors.get_unsafe(noob::actor_handle::make(i));
		actor_dither(noob::actor_handle::make(i));
	}
}


void noob::stage::actor_dither(noob::actor_handle ah) noexcept(true)
{
	noob::actor a = actors.get(ah);

	if (a.alive)
	{
		std::vector<noob::contact_point> cps = get_intersecting(ah);
		//if (cps.size() == 0)
		//{
		//	noob::vec3 gravity = vec3_from_bullet(dynamics_world->getGravity()) * a.gravity_coeff;
		//	noob::ghost gh = ghosts.get(a.ghost);
		//	noob::vec3 temp_pos = gh.get_position();

		//	temp_pos += gravity;
		//	gh.set_position(temp_pos);
		// }
	}
}

noob::node_handle noob::stage::add_to_graph(const noob::stage_item_variant arg) noexcept(true)
{
/*
	enum class stage_item_type : uint32_t
	{
		ACTOR = 0, SCENERY = 1, TRIGGER = 2, PARTICLES = 3, LIGHT = 4
	};

	struct stage_item_variant
	{
		stage_item_type type;
		uint32_t index;
	};
*/

	switch(arg.type)
	{
		case(noob::stage_item_type::ACTOR):
		{
			break;
		}
		case(noob::stage_item_type::SCENERY):
		{
		}
	}
	return noob::node_handle::make(0);
}
