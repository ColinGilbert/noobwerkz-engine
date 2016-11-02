#include "Stage.hpp"
// #include "Shiny.h"

noob::stage::~stage() noexcept(true) 
{
	// delete dynamics_world;
	// delete solver;
	// delete collision_configuration;
	// delete collision_dispatcher;
	// delete broadphase;
}


void noob::stage::init(uint32_t window_width, uint32_t window_height, const noob::mat4& projection_mat) noexcept(true) 
{
	update_viewport_params(window_width, window_height, projection_mat);

	noob::vec3 eye_pos, eye_target, eye_up;
	eye_pos = noob::vec3(0.0, 0.0, -10.0);
	eye_target = noob::vec3(0.0, 0.0, 0.0);
	eye_up = noob::vec3(0.0, 1.0, 0.0);
	//const noob::vec3 eye_forward = eye_pos - eye_target;
	//eye_up = noob::cross(eye_forward, noob::vec3(0.0, 1.0, 0.0));
	//eye_up = noob::cross(eye_up, eye_forward);
	view_matrix = noob::look_at(eye_pos, eye_target, eye_up);
	projection_matrix = projection_mat;
	
	// broadphase = new btDbvtBroadphase();
	// collision_configuration = new btDefaultCollisionConfiguration();
	// collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	// solver = new btSequentialImpulseConstraintSolver();
	// dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	// dynamics_world->setGravity(btVector3(0, -10, 0));

	// dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	main_light.colour = noob::vec4(1.0, 1.0, 1.0, 0.3);
	main_light.direction = noob::vec4(-0.3333, -1.0, -0.3333, 1.0);

	team_colours.push_back(noob::vec4(1.0, 1.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 0.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 1.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 0.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 1.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 1.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 0.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 0.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.2, 0.5, 0.2, 1.0));
	logger::log(noob::importance::INFO, "[Stage] Done init.");
}


void noob::stage::tear_down() noexcept(true) 
{
	for (size_t i = 0; i < bodies.count(); ++i)
	{
		// remove_body(body_handle::make(i));
	}

	bodies.empty();

	// Joints are automatically cleaned up by Bullet once all bodies have been destroyed. I think...
	joints.empty();

	for (size_t i = 0; i < ghosts.count(); ++i)
	{
		// remove_ghost(ghost_handle::make(i));
	}

	ghosts.empty();

	actors.empty();
	// actor_mq.clear();
	// actor_mq_count = 0;

	// sceneries.empty();

	// delete dynamics_world;
	// delete broadphase;
	// delete solver;
	// delete collision_dispatcher;
	// delete collision_configuration;

	// draw_graph.clear();
	// node_masks.empty();

	init(viewport_width, viewport_height, projection_matrix);
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

	// dynamics_world->stepSimulation(1.0/60.0, 10);

	// update_particle_systems();
	update_actors();
	noob::time end_time = noob::clock::now();

	update_duration = end_time - start_time;

	noob::globals& g = noob::globals::get_instance();
	g.profile_run.stage_physics_duration += update_duration;
}


void noob::stage::draw() noexcept(true) 
{
	// PROFILE_FUNC();

	noob::graphics& gfx = noob::graphics::get_instance();
	noob::graphics::program_handle prog = gfx.get_default_instanced();
	gfx.use_program(prog);

	for (uint32_t drawables_index = 0; drawables_index < drawables.size(); ++drawables_index)
	{
		const noob::stage::drawable_info_handle handle = noob::stage::drawable_info_handle::make(drawables_index);
		if (drawables[drawables_index].needs_colours)
		{
			upload_colours(handle);
			drawables[drawables_index].needs_colours = false;
		}

		upload_matrices(handle);

		const noob::model_handle modl = drawables[drawables_index].model;
		const uint32_t instance_count = drawables[drawables_index].count;

		// noob::logger::log(noob::importance::INFO, noob::concat("Drawing model ", noob::to_string(modl.index()), " ", noob::to_string(instance_count), " times"));

		gfx.draw(modl, instance_count);

		// noob::logger::log(noob::importance::INFO,  noob::concat("[Stage] Drawing model ", noob::to_string(modl.index()), " ", noob::to_string(instance_count), " times."));//gfx.draw(modl, instance_count);

	}
}


void noob::stage::update_viewport_params(uint32_t window_width, uint32_t window_height, const noob::mat4& projection_mat) noexcept(true)
{
	viewport_width = window_width;
	viewport_height = viewport_height;
	// view_matrix = view_mat;
	projection_matrix = projection_mat;
}


void noob::stage::build_navmesh() noexcept(true)
{

}

/*
noob::body_handle noob::stage::body(const noob::body_type b_type, const noob::shape_handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();

	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	

	body_handle bod_h = bodies.add(b);

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
*/

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
			// a.ghost = ghost(info.bp.bounds, pos, orient);
			a.bp_handle = bp_h;
			a.position = pos;
			a.orientation = orient;
			const noob::actor_handle a_h = actors.add(a);

			noob::fast_hashtable::cell* results = models_to_instances.lookup(info.bp.model.index());

			const uint32_t index = results->value;
			const uint32_t old_count = drawables[index].count;

			drawables[index].count++;
			drawables[index].instances[old_count].actor = a_h;

			// Setting index-to-self info:
			// noob::ghost temp_ghost = ghosts.get(a.ghost);

			// temp_ghost.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
			// temp_ghost.inner->setUserIndex_2(a_h.index());

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
/*
	noob::ghost temp_ghost = ghosts.get(ghost_h);

	btManifoldArray manifold_array;

	btBroadphasePairArray& pair_array = temp_ghost.inner->getOverlappingPairCache()->getOverlappingPairArray();
*/
	std::vector<noob::contact_point> results;
/*
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
*/
	return results;
}


std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::actor_handle ah) const noexcept(true)
{
	noob::actor a = actors.get(ah);
	return get_intersecting(a.ghost);
}


/*
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
*/

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


void noob::stage::upload_colours(drawable_info_handle arg) const noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::model_handle model_h = drawables[arg.index()].model;

	noob::graphics& gfx = noob::graphics::get_instance();

	// noob::gpu_write_buffer buf = gfx.map_buffer(model_h, noob::model::instanced_data_type::COLOUR, 0, count);
	/*
	   if (buf.valid() == false)
	   {
	   logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced colour buffer for model ", noob::to_string(arg.index())));	
	   return;
	   }
	   */
	std::vector<noob::vec4> colours;
	uint32_t current = 0;
	while (current < count)
	{
		const noob::actor_handle a_h = drawables[arg.index()].instances[current].actor;
		const noob::actor a = actors.get(a_h);
		const noob::vec4 colour = team_colours[a.team];
		colours.push_back(colour);
		/*
		   if (!valid)
		   {
		   logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu colours buffer for model ", noob::to_string(arg.index())));
		   gfx.unmap_buffer();			
		   return;
		   }
		   */
		++current;
	}

	gfx.push_colours(model_h, 0, colours);

	// logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(current), " colours uploaded"));

	// gfx.unmap_buffer();
}


void noob::stage::upload_matrices(drawable_info_handle arg) const noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::model_handle model_h = drawables[arg.index()].model;


	// noob::gpu_write_buffer buf = gfx.map_buffer(model_h, noob::model::instanced_data_type::MATRICES, 0, count);
	/*
	   if (buf.valid() == false)
	   {
	   logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced matrices buffer for model ", noob::to_string(arg.index())));	
	   return;
	   }
	   */
	const noob::mat4 viewproj_mat = projection_matrix * view_matrix;
	std::vector<noob::mat4> matrices;
	uint32_t current = 0;
	while (current < count)
	{
		const noob::stage::drawable_instance info = drawables[arg.index()].instances[current];
		const noob::actor a = actors.get(info.actor);

		// const noob::ghost gst = ghosts.get(a.ghost);
		// const noob::mat4 model_mat = gst.get_transform();

		noob::mat4 xform = noob::identity_mat4();
		xform = noob::rotate(xform, a.orientation);
		xform = noob::translate(xform, a.position);

		const noob::mat4 mvp_mat = viewproj_mat * xform;
		matrices.push_back(mvp_mat);
		const noob::mat4 normal_mat = noob::transpose(noob::inverse(xform * view_matrix));
		matrices.push_back(normal_mat);



		/*
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
		   */
		++current;
	}

	noob::graphics& gfx = noob::graphics::get_instance();

	gfx.push_matrices(model_h, 0, matrices);

	// logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(current), "*2 matrices uploaded"));

	// gfx.unmap_buffer();
}


void noob::stage::reserve_models(noob::model_handle h, uint32_t num) noexcept(true)
{
	noob::fast_hashtable::cell* results = models_to_instances.lookup(h.index());

	// If we haven't gotten a model with that handle yet...
	if (!models_to_instances.is_valid(results))
	{
		results = models_to_instances.insert(h.index());

		noob::stage::drawable_info info;
		info.model = h;
		info.count = 0;
		info.needs_colours = true;

		drawables.push_back(info);

		const uint32_t results_index  = drawables.size() - 1;

		drawables[results_index].instances.resize(num);

		results->value = results_index;

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

/*
   std::string noob::stage::print_drawables_info() const noexcept(true)
   {

   }
   */


void noob::stage::accept_ndof_data(const noob::ndof::data& info) noexcept(true)
{
	if (info.movement == true)
	{
		// logger::log(noob::importance::INFO, noob::concat("[Stage] NDOF data: T(", noob::to_string(info.translation[0]), ", ", noob::to_string(info.translation[1]) ,", ", noob::to_string(info.translation[2]), ") R(", noob::to_string(info.rotation[0]), ", ", noob::to_string(info.rotation[1]), ", ", noob::to_string(info.rotation[2]), ")"));
		float damping = 360.0;
		view_matrix = noob::rotate_x_deg(view_matrix, -info.rotation[0] / damping);
		view_matrix = noob::rotate_y_deg(view_matrix, -info.rotation[1] / damping);
		view_matrix = noob::rotate_z_deg(view_matrix, -info.rotation[2] / damping);
		view_matrix = noob::translate(view_matrix, noob::vec3(-info.translation[0] / damping, -info.translation[1] / damping, -info.translation[2] / damping));
	}

}
