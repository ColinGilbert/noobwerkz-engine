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

void noob::stage::init(float window_width, float window_height, const noob::mat4& view_mat, const noob::mat4& projection_mat) noexcept(true) 
{
	update_viewport_params(window_width, window_height, view_mat, projection_mat);

	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	// For the ghost object to work correctly, we need to add a callback to our world.
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

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
	// actor_mq.clear();
	// actor_mq_count = 0;

	// sceneries.empty();

	delete dynamics_world;
	delete broadphase;
	delete solver;
	delete collision_dispatcher;
	delete collision_configuration;

	// draw_graph.clear();
	// node_masks.empty();

	init(viewport_width, viewport_height, view_matrix, projection_matrix);
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


void noob::stage::draw() const noexcept(true) 
{
	// PROFILE_FUNC();


}


void noob::stage::update_viewport_params(float window_width, float window_height, const noob::mat4& view_mat, const noob::mat4& projection_mat) noexcept(true)
{
	viewport_width = window_width;
	viewport_height = viewport_height;
	view_matrix = view_mat;
	projection_matrix = projection_mat;
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

noob::actor_blueprints_handle noob::stage::add_actor_blueprints(const noob::actor_blueprints& arg) noexcept(true)
{
	noob::stage::actor_info info;
	info.bp = arg;
	info.count = 0;
	info.max = 0;
	actor_factories.push_back(info);
	
	return noob::actor_blueprints_handle::make(actor_factories.size() - 1);
}


void noob::stage::reserve_actors(noob::actor_blueprints_handle bp_h, uint32_t num) noexcept(true)
{
	if (actor_factories.size() > bp_h.index())
	{
		noob::stage::actor_info info = actor_factories[bp_h.index()];
		const uint32_t old_max = info.max;
		const uint32_t new_max = info.max + num;
		reserve_models(info.bp.model, num);
		info.max = new_max;
		actor_factories[bp_h.index()] = info;
	}
}


noob::actor_handle noob::stage::actor(noob::actor_blueprints_handle bp_h, uint32_t team, const noob::vec3& pos, const noob::versor& orient) noexcept(true)
{
	if (actor_factories.size() > bp_h.index())
	{
		noob::stage::actor_info info = actor_factories[bp_h.index()];

		if (info.count < info.max)
		{
			noob::actor a;
			a.team = team;
			a.ghost = ghost(info.bp.bounds, pos, orient);
			a.bp_handle = bp_h;
			noob::actor_handle a_h = actors.add(a);

			noob::fast_hashtable::cell* results = models_to_instances.lookup(info.bp.model.index());
			const uint32_t index = results->value;
			const uint32_t old_count = drawables[index].count;
			drawables[index].count++;
			drawables[index].instances[old_count].index1 = a.ghost.index();

			// Setting index-to-self info:
			noob::ghost temp_ghost = ghosts.get(a.ghost);

			temp_ghost.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
			temp_ghost.inner->setUserIndex_2(a_h.index());

			actor_factories[bp_h.index()].count++;
			
			return a_h;
		}
		else
		{
			return noob::actor_handle::make_invalid();
		}
	}
	else
	{
		return noob::actor_handle::make_invalid();
	}
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
				logger::log(noob::importance::ERROR, "[Stage] Invalid objects found during collision");
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


void noob::stage::upload_colours(drawable_info_handle index_arg, const std::vector<std::tuple<uint32_t, noob::vec4>>& colours_arg) noexcept(true)
{
	const noob::stage::drawable_info info = drawables[index_arg.index()];

	noob::graphics& gfx = noob::graphics::get_instance();

	noob::gpu_write_buffer buf = gfx.map_buffer(info.handle, noob::model::instanced_data_type::COLOUR);

	if (buf.valid() == false)
	{
		logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced colour buffer for model ", noob::to_string(index_arg.index())));	
		return;
	}

	for (std::tuple<uint32_t, noob::vec4> c : colours_arg)
	{
		uint32_t current = 0;

		while (current < std::get<0>(c))
		{
			bool valid = buf.push_back(std::get<1>(c));
			++current;
			if (!valid)
			{
				logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu colours buffer for model ", noob::to_string(index_arg.index())));
				gfx.unmap_buffer();

				return;
			}
		}
	}
	gfx.unmap_buffer();

}


void noob::stage::upload_matrices(drawable_info_handle arg) noexcept(true)
{
	const noob::stage::drawable_info master_info = drawables[arg.index()];

	assert(master_info.count <= master_info.instances.size());

	noob::graphics& gfx = noob::graphics::get_instance();

	noob::gpu_write_buffer buf = gfx.map_buffer(master_info.handle, noob::model::instanced_data_type::MATRICES);

	if (buf.valid() == false)
	{
		logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced matrices buffer for model ", noob::to_string(arg.index())));	
		return;
	}

	uint32_t current = 0;
	const noob::mat4 viewproj_mat = projection_matrix * view_matrix;

	while (current < master_info.count)
	{
		const noob::stage::drawable_instance info = master_info.instances[current];
		const noob::actor a = actors.get(noob::actor_handle::make(info.index1));
		const noob::ghost gst = ghosts.get(a.ghost);
		const noob::mat4 model_mat = gst.get_transform();
		const noob::mat4 mvp_mat =  viewproj_mat * model_mat;
		const noob::mat4 normal_mat = noob::transpose(noob::inverse((model_mat * view_matrix)));

		bool valid = buf.push_back(mvp_mat);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu matrices buffer for model ", noob::to_string(arg.index()), " MVP"));
			gfx.unmap_buffer();			
			return;
		}

		valid = buf.push_back(normal_mat);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu matrices buffer for model ", noob::to_string(arg.index()), " normal"));
			gfx.unmap_buffer();
			return;
		}

		++current;
	}

	gfx.unmap_buffer();
}

void noob::stage::reserve_models(noob::model_handle h, uint32_t num) noexcept(true)
{
	noob::fast_hashtable::cell* results = models_to_instances.lookup(h.index());

	// If we haven't gotten a model with that handle yet...
	if (!models_to_instances.is_valid(results))
	{
		results = models_to_instances.insert(h.index());

		noob::stage::drawable_info info;
		info.handle = h;
		info.count = 0;
		info.needs_colours = true;
		drawables.push_back(info);

		const uint32_t index  = drawables.size() - 1;

		drawables[index].instances.resize(num);

		results->value = index;

		noob::graphics& gfx = noob::graphics::get_instance();
		gfx.reset_instances(h, num);
	}
	else
	{
		// To prevent repeated pointer dereferences
		const uint32_t index = results->value;
		const uint32_t old_max = drawables[index].instances.size();
		const uint32_t max_new = old_max + num;

		drawables[index].instances.resize(max_new);

		noob::graphics& gfx = noob::graphics::get_instance();
		gfx.reset_instances(h, num);
	}
}
