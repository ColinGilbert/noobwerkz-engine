#include "Stage.hpp"


void noob::stage::init(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true) 
{
	update_viewport_params(dims, projection_mat);

	noob::vec3f eye_pos, eye_target, eye_up;
	eye_pos = noob::vec3f(300.0, 700.0, 100.0);
	eye_target = noob::vec3f(10.0, 50.0, 0.0);
	eye_up = noob::vec3f(0.0, 1.0, 0.0);

	view_matrix = noob::look_at(eye_pos, eye_target, eye_up);
	projection_matrix = projection_mat;

	noob::graphics& gfx = noob::get_graphics();
	// gfx.set_view_mat(view_matrix);
	// gfx.set_projection_mat(projection_matrix);

	world.init();



	main_light.colour = noob::vec4f(1.0, 1.0, 1.0, 0.0);
	main_light.direction = noob::vec3f(0.0, -0.90, 0.3);

	team_colours.push_back(noob::vec4f(1.0, 1.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4f(0.1, 0.3, 0.3, 1.0));
	team_colours.push_back(noob::vec4f(0.3, 0.1, 0.1, 1.0));
	team_colours.push_back(noob::vec4f(0.3, 0.1, 0.3, 1.0));
	team_colours.push_back(noob::vec4f(0.3, 0.3, 0.1, 1.0));
	team_colours.push_back(noob::vec4f(0.4, 0.4, 0.4, 1.0));
	team_colours.push_back(noob::vec4f(0.4, 0.4, 0.1, 1.0));
	team_colours.push_back(noob::vec4f(0.1, 0.4, 0.4, 1.0));
	team_colours.push_back(noob::vec4f(0.4, 0.4, 0.1, 1.0));
	team_colours.push_back(noob::vec4f(0.4, 0.1, 0.4, 1.0));
	team_colours.push_back(noob::vec4f(0.5, 0.7, 0.7, 1.0));
	team_colours.push_back(noob::vec4f(0.7, 0.5, 0.5, 1.0));
	team_colours.push_back(noob::vec4f(0.7, 0.5, 0.7, 1.0));
	team_colours.push_back(noob::vec4f(0.0, 1.0, 1.0, 1.0));

	logger::log(noob::importance::INFO, "[Stage] Done init.");
}


void noob::stage::tear_down() noexcept(true) 
{
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

	init(viewport_dims, projection_matrix);
}


void noob::stage::update(double dt) noexcept(true) 
{
	// PROFILE_FUNC();
	noob::time_point start_time = noob::clock::now();

	//if (nav_changed)
	//{
	// build_navmesh();
	// nav_changed = false;
	//}

	// TODO: Change?
	world.step(1.0/120.0);

	// update_particle_systems();
	update_actors();
	noob::time_point end_time = noob::clock::now();

	update_duration = end_time - start_time;

	noob::globals& g = noob::get_globals();
	g.profile_run.stage_physics_duration += update_duration;
}


void noob::stage::draw() noexcept(true) 
{
	// PROFILE_FUNC();
	if (ndof_happened == true)
	{
		view_matrix = noob::rotate_x_deg(view_matrix, ndof_rotation[0]);
		view_matrix = noob::rotate_y_deg(view_matrix, ndof_rotation[1]);
		view_matrix = noob::rotate_z_deg(view_matrix, ndof_rotation[2]);
		view_matrix = noob::translate(view_matrix, ndof_translation);
		ndof_happened = false;
	}

	noob::graphics& gfx = noob::get_graphics();
	noob::graphics::program_handle prog = gfx.get_instanced_shader();
	gfx.use_program(prog);

	gfx.set_view_mat(view_matrix);
	gfx.set_projection_mat(projection_matrix);

	// TODO: Change
	gfx.set_light_direction(main_light.direction);
	gfx.set_eye_position(noob::translation_from_mat4(view_matrix));
	gfx.upload_instanced_uniforms();

	for (uint32_t drawables_index = 0; drawables_index < drawables.size(); ++drawables_index)
	{
		const noob::stage::drawable_info_handle handle = noob::stage::drawable_info_handle::make(drawables_index);
		if (drawables[drawables_index].needs_colours)
		{
			upload_colours(handle);
			drawables[drawables_index].needs_colours = false;
		}

		upload_matrices(handle);
		const noob::instanced_model_handle modl_h = drawables[drawables_index].model;
		const uint32_t instance_count = drawables[drawables_index].count;

		gfx.draw_instanced(modl_h, instance_count);
	}

	if (terrain_started)
	{
		if (terrain_changed)
		{
			upload_terrain();
			terrain_changed = false;
		}

		gfx.draw_terrain(num_terrain_verts);
	}
}


void noob::stage::update_viewport_params(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true)
{
	viewport_dims = dims;
	projection_matrix = projection_mat;
}


void noob::stage::build_navmesh() noexcept(true)
{

}


void noob::stage::rebuild_graphics(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true)
{
	noob::graphics& gfx = noob::get_graphics();
	update_viewport_params(dims, projection_mat);
}


noob::actor_blueprints_handle noob::stage::add_actor_blueprints(const noob::actor_blueprints& arg) noexcept(true)
{
	noob::stage::actor_info info;
	info.bp = arg;
	info.count = 0;
	info.max = 0;
	actors_extra_info.push_back(info);

	return noob::actor_blueprints_handle::make(actors_extra_info.size() - 1);
}


noob::prop_blueprints_handle noob::stage::add_props_blueprints(const noob::prop_blueprints& arg) noexcept(true)
{
	noob::stage::prop_info info;
	info.bp = arg;
	info.count = 0;
	info.max = 0;
	props_extra_info.push_back(info);

	return noob::prop_blueprints_handle::make(props_extra_info.size() - 1);
}


void noob::stage::reserve_actors(const noob::actor_blueprints_handle blueprint_h, uint32_t num) noexcept(true)
{
	if (actors_extra_info.size() > blueprint_h.index())
	{
		noob::stage::actor_info info = actors_extra_info[blueprint_h.index()];
		const uint32_t old_max = info.max;
		const uint32_t new_max = info.max + num;
		reserve_models(info.bp.model, num);
		info.max = new_max;
		actors_extra_info[blueprint_h.index()] = info;
	}
}


void noob::stage::reserve_props(const noob::prop_blueprints_handle blueprint_h, uint32_t num) noexcept(true)
{
	assert(team_colours.size() > blueprint_h.index());
	noob::stage::prop_info info = props_extra_info[blueprint_h.index()];
	const uint32_t old_max = info.max;
	const uint32_t new_max = info.max + num;
	reserve_models(info.bp.model, num);
	info.max = new_max;
	props_extra_info[blueprint_h.index()] = info;
}


void noob::stage::set_team_colour(uint32_t Team, const noob::vec4f& Colour) noexcept(true)
{
	//if (team_colours.size() < Team)
	//{
	//		team_colours.reserve(Team);
	//	}
	assert(team_colours.size() > Team);
	team_colours[Team] = Colour;
}


noob::actor_handle noob::stage::create_actor(const noob::actor_blueprints_handle blueprint_h, uint32_t team, const noob::vec3f& pos, const noob::versorf& orient) noexcept(true)
{
	if (actors_extra_info.size() > blueprint_h.index())
	{
		noob::stage::actor_info info = actors_extra_info[blueprint_h.index()];

		if (info.count < info.max)
		{
			const noob::ghost_handle ghost_h = world.add_ghost(info.bp.bounds, pos, orient);

			noob::actor a;
			a.team = team;
			a.ghost = ghost_h;
			a.bp_handle = blueprint_h;

			actors.push_back(a);

			const noob::actor_handle actor_h = noob::actor_handle::make(actors.size() - 1); 
			noob::fast_hashtable::cell *results = models_to_drawable_instances.lookup(info.bp.model.index());
			const uint32_t instance_index = results->value;
			const uint32_t old_count = drawables[instance_index].count;

			drawables[instance_index].count++;
			drawables[instance_index].instances[old_count].which = noob::stage::drawable_instance::type::ACTOR;
			drawables[instance_index].instances[old_count].index = actor_h.index();

			// Setting index-to-self info:
			noob::ghost& temp_ghost = world.get_ghost(ghost_h);
			temp_ghost.set_user_index_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
			temp_ghost.set_user_index_2(actor_h.index());


			actors_extra_info[blueprint_h.index()].count++;


			logger::log(noob::importance::INFO, "[Stage] Actor added.");

			return actor_h;
		}
	}


			logger::log(noob::importance::INFO, "[Stage] Actor failed to add.");
	return noob::actor_handle::make_invalid();
}


noob::prop_handle noob::stage::create_prop(const noob::prop_blueprints_handle blueprint_h, uint32_t team, const noob::vec3f& pos, const noob::versorf& orient) noexcept(true)
{

	if (props_extra_info.size() > blueprint_h.index())
	{
		// logger::log(noob::importance::INFO, "[Stage] create_prop. Inside first conditional block.");
		noob::stage::prop_info info = props_extra_info[blueprint_h.index()];

		if (info.count < info.max)
		{
			// logger::log(noob::importance::INFO, "[Stage] create_prop. Inside second conditional block.");
			const noob::body_handle body_h = world.add_body(noob::body_type::DYNAMIC, info.bp.shape, info.bp.mass, pos, orient, info.bp.ccd);

			noob::prop p;
			p.body = body_h;
			p.colour = noob::colourfp_handle::make(team);
			p.bp = blueprint_h;

			props.push_back(p);

			const noob::prop_handle prop_h = noob::prop_handle::make(props.size() - 1);
			noob::fast_hashtable::cell *results = models_to_drawable_instances.lookup(info.bp.model.index());
			const uint32_t instance_index = results->value;
			const uint32_t old_count = drawables[instance_index].count;

			drawables[instance_index].count++;
			drawables[instance_index].instances[old_count].which = noob::stage::drawable_instance::type::PROP;
			drawables[instance_index].instances[old_count].index = prop_h.index();

			// Setting index-to-self info:
			noob::body& temp_bod = world.get_body(body_h);
			temp_bod.set_user_index_1(static_cast<uint32_t>(noob::stage::drawable_instance::type::PROP));
			temp_bod.set_user_index_2(prop_h.index());

			props_extra_info[blueprint_h.index()].count++;

			return prop_h;
		}
		else noob::logger::log(noob::importance::INFO, "[Stage] Prop info count exceeded");
	}
	return noob::prop_handle::make_invalid();
}


noob::scenery_handle noob::stage::create_scenery(const noob::shape_handle Shape, const noob::vec3f& Pos, const noob::versorf& Orient) noexcept(true)
{
	const noob::globals& g = noob::get_globals();
	const noob::body_handle bod_h = world.add_body(noob::body_type::STATIC, Shape, 0.0, Pos, Orient, false);

	noob::scenery sc;
	sc.body = bod_h;

	sceneries.push_back(sc);

	const noob::scenery_handle scenery_h = noob::scenery_handle::make(sceneries.size() - 1);
	noob::body b = world.get_body(bod_h);
	b.set_user_index_1(static_cast<uint32_t>(noob::stage_item_type::SCENERY));
	b.set_user_index_2(scenery_h.index());

	// logger::log(noob::importance::INFO, noob::concat("[Stage] Scenery added! Handle ", noob::to_string(scenery_h.index())) );

	terrain_changed = terrain_started = true;

	return scenery_h;
}


noob::assembly_handle noob::stage::create_assembly(const noob::vec3f& pos, const noob::versorf& orient, bool ccd, const std::vector<noob::prop_handle>& props_arg) noexcept(true)
{
	double mass_accum = 0.0;
	std::vector<noob::compound_shape::child_info> infos;
	for (size_t i = 0; i < props_arg.size(); ++i)
	{
		noob::compound_shape::child_info inf;
		noob::body bod = world.get_body(props[i].body);
		inf.shape = bod.get_shape();
		inf.mass = bod.get_mass();
		inf.pos = bod.get_position();
		inf.orient = bod.get_orientation();
		infos.push_back(inf);
		mass_accum += static_cast<double>(infos[i].mass);
		size_t stage_props_index = props_arg[i].index();

		props[stage_props_index].in_assembly = true;
		props[stage_props_index].child_num_for_assembly = i;
		// world.remove_body(props[stage_props_index].body);

		bod.toggle_active();
	}
	noob::assembly a;
	assemblies.push_back(a);
	const uint32_t i = assemblies.size() - 1;
	assemblies[i].init(world, static_cast<float>(mass_accum), pos, orient, ccd, props_arg, infos);

	assemblies[i].bod.set_user_index_1(static_cast<uint32_t>(noob::stage::drawable_instance::type::ASSEMBLY));
	assemblies[i].bod.set_user_index_2(i);

	logger::log(noob::importance::INFO, noob::concat("[Stage] Made assembly with ",  noob::to_string(props_arg.size()), " children and index ", noob::to_string(i)));

	return noob::assembly_handle::make(i);
}


noob::scenery& noob::stage::get_scenery(const noob::scenery_handle arg) noexcept(true)
{
	return sceneries[arg.index()];
}


noob::actor& noob::stage::get_actor(const noob::actor_handle arg) noexcept(true)
{
	return actors[arg.index()];
}


noob::prop& noob::stage::get_prop(const noob::prop_handle arg) noexcept(true)
{
	// logger::log(noob::importance::INFO, noob::concat("[Stage] props.size ", noob::to_string(props.size()), "handle = ", noob::to_string(arg.index())));
	assert(props.size() > arg.index());
	return props[arg.index()];
}


noob::body& noob::stage::get_body(const noob::body_handle arg) noexcept(true)
{
	return world.get_body(arg);
}


noob::body_handle noob::stage::get_body_handle(const noob::assembly_handle ah) const noexcept(true)
{
	return assemblies[ah.index()].get_body_handle();
}


noob::constraint& noob::stage::get_constraint(const noob::constraint_handle arg) noexcept(true)
{
	return world.get_constraint(arg);
}

noob::constraint_handle noob::stage::create_fixed_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& frame_in_a, const noob::mat4f& frame_in_b) noexcept(true)
{
	return world.add_fixed_constraint(a, b, frame_in_a, frame_in_b);
}


noob::constraint_handle noob::stage::create_point_constraint(const noob::body_handle a, const noob::body_handle b, const noob::vec3f& pivot_a, const noob::vec3f& pivot_b) noexcept(true)
{
	return world.add_point_constraint(a, b, pivot_a, pivot_b);
}



noob::constraint_handle noob::stage::create_hinge_constraint(const noob::body_handle a, const noob::vec3f& pivot, const noob::vec3f& axis) noexcept(true)
{
	return world.add_hinge_constraint(a, pivot, axis);
}



noob::constraint_handle noob::stage::create_conical_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& frame_in_a, const noob::mat4f& frame_in_b) noexcept(true)
{
	return world.add_conical_constraint(a, b, frame_in_a, frame_in_b);
}



noob::constraint_handle noob::stage::create_hinge_constraint(const noob::body_handle a, const noob::body_handle b, const noob::vec3f& pivot_in_a, const noob::vec3f& pivot_in_b, const noob::vec3f& axis_in_a, const noob::vec3f& axis_in_b) noexcept(true)
{
	return world.add_hinge_constraint(a, b, pivot_in_a, pivot_in_b, axis_in_a, axis_in_b);
}


noob::constraint_handle noob::stage::create_slide_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	return world.add_slide_constraint(a, b, local_a, local_b);
}


// noob::constraint_handle noob::stage::create_conical_constraint() noexcept(true);
// noob::constraint_handle noob::stage::create_gear_constraint() noexcept(true);


noob::constraint_handle noob::stage::create_generic_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	return world.add_generic_constraint(a, b, local_a, local_b);
}


void noob::stage::set_actor_position(const noob::actor_handle Actor, const noob::vec3f& Pos) noexcept(true)
{
	noob::ghost& pryzrak = world.get_ghost(actors[Actor.index()].ghost);
	pryzrak.set_position(Pos);
}


void noob::stage::set_actor_orientation(const noob::actor_handle Actor, const noob::versorf& Orient) noexcept(true)
{
	noob::ghost& pryzrak = world.get_ghost(actors[Actor.index()].ghost);
	pryzrak.set_orientation(Orient);
}


void noob::stage::set_actor_velocity(const noob::actor_handle Actor, const noob::vec3f& Vel) noexcept(true)
{
	actors[Actor.index()].velocity = Vel;
}


void noob::stage::set_actor_target(const noob::actor_handle Actor, const noob::vec3f& Target) noexcept(true)
{
	actors[Actor.index()].target_pos = Target;
}


void noob::stage::set_actor_self_controlled(const noob::actor_handle Actor, bool Control) noexcept(true)
{
	actors[Actor.index()].self_control = Control;
}


void noob::stage::set_actor_team(const noob::actor_handle Actor, uint32_t Team) noexcept(true)
{
	actors[Actor.index()].team = Team;
}


noob::vec3f noob::stage::get_actor_position(const noob::actor_handle Actor) noexcept(true)
{
	return world.get_ghost(actors[Actor.index()].ghost).get_position();
}


noob::versorf noob::stage::get_actor_orientation(const noob::actor_handle Actor) noexcept(true)
{
	return world.get_ghost(actors[Actor.index()].ghost).get_orientation();	
}


noob::vec3f noob::stage::get_actor_velocity(const noob::actor_handle Actor) noexcept(true)
{
	return actors[Actor.index()].velocity;
}


noob::vec3f noob::stage::get_actor_target(const noob::actor_handle Actor) noexcept(true)
{
	return actors[Actor.index()].target_pos;
}


bool noob::stage::get_actor_self_controlled(const noob::actor_handle Actor) noexcept(true)
{
	return actors[Actor.index()].self_control;
}


uint32_t noob::stage::get_actor_team(const noob::actor_handle Actor) noexcept(true)
{
	return actors[Actor.index()].team;
}


size_t noob::stage::get_intersecting(const noob::actor_handle ah, std::vector<noob::contact_point>& Results) const noexcept(true)
{
	noob::actor a = actors[ah.index()];
	return world.get_intersecting(a.ghost, Results);
}


void noob::stage::update_actors() noexcept(true)
{
	for (uint32_t i = 0; i < actors.size(); ++i)
	{
		actor_dither(noob::actor_handle::make(i));
	}
}


void noob::stage::actor_dither(const noob::actor_handle ah) noexcept(true)
{
	noob::actor a = actors[ah.index()];
	// static uint64_t value = 0;
	if (a.self_control)
	{
		size_t num_contacts = get_intersecting(ah, contacts);
		for (auto cp : contacts)
		{

			// Here, we implement behaviours.
		}
	}
	// TODO: Implement physics-based code
}


void noob::stage::upload_colours(const drawable_info_handle arg) const noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::instanced_model_handle model_h = drawables[arg.index()].model;

	noob::graphics& gfx = noob::get_graphics();

	noob::gpu_write_buffer buf = gfx.map_colours_buffer(model_h, 0, count);

	if (buf.valid() == false)
	{
		logger::log(noob::importance::WARNING, noob::concat("[Stage] Could not map instanced colour buffer for model ", noob::to_string(arg.index()), " - Number of instances = ", noob::to_string(count)));	
		return;
	}

	uint32_t current = 0;
	while (current < count)
	{
		noob::vec4f colour;
		if (drawables[arg.index()].instances[current].which == noob::stage::drawable_instance::type::ACTOR) 
		{

			const noob::actor_handle a_h = noob::actor_handle::make(drawables[arg.index()].instances[current].index);
			const noob::actor a = actors[a_h.index()];
			colour = team_colours[a.team];
		}
		else // Its a prop!
		{
			const noob::prop_handle h = noob::prop_handle::make(drawables[arg.index()].instances[current].index);
			const noob::prop p = props[h.index()];
			colour = team_colours[p.colour.index()];
		}

		bool valid = buf.push_back(colour);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu colours buffer for model ", noob::to_string(arg.index())));
			gfx.unmap_buffer();			
			return;
		}

		++current;
	}

	// logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(current), " colours uploaded"));

	gfx.unmap_buffer();
}


void noob::stage::upload_matrices(const drawable_info_handle arg) noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::instanced_model_handle model_h = drawables[arg.index()].model;

	noob::graphics& gfx = noob::get_graphics();
	noob::gpu_write_buffer buf = gfx.map_matrices_buffer(model_h, 0, count); // Mapping count + 1 is wierd but it seems to fix a problem when dealing with multiple models
 
	if (!buf.valid())
	{
		logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced matrices buffer for model ", noob::to_string(arg.index()), " - count: ", noob::to_string(count)));	
		return;
	}

	// const noob::mat4f viewproj_mat = projection_matrix * view_matrix;
	uint32_t current = 0;
	while (current < count)
	{
		const noob::stage::drawable_instance info = drawables[arg.index()].instances[current];
		bool valid = false;
		noob::mat4f model_matrix; 
		if (info.which == noob::stage::drawable_instance::type::ACTOR) // Its an actor
		{
			const noob::actor a = actors[info.index];
			const noob::ghost& gst = world.get_ghost(a.ghost);
			model_matrix = gst.get_transform();
		}
		else // Its a prop!
		{
			const noob::prop p = props[info.index];
			if (p.in_assembly)
			{

				model_matrix = assemblies[p.assembly.index()].get_child_transform(p.child_num_for_assembly);
			}
			else
			{
				const noob::body& bod = world.get_body(p.body);
				model_matrix = bod.get_transform();
			}


			// logger::log(noob::importance::INFO, noob::concat(bod.get_debug_string()));
		}

		valid = buf.push_back(model_matrix);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu (model) matrices buffer for model ", noob::to_string(arg.index()), "."));
			gfx.unmap_buffer();			
			return;
		}
		++current;
	}

	gfx.unmap_buffer();
}


void noob::stage::upload_terrain() noexcept(true)
{
	if (terrain_started)
	{
		std::vector<noob::vec4f> tmp_vertices;
		for (uint32_t sceneries_iter = 0; sceneries_iter < sceneries.size(); ++sceneries_iter)
		{
			const noob::scenery sc = sceneries[sceneries_iter];
			const noob::body& bod = world.get_body(sc.body);
			const noob::mat4f model_mat = bod.get_transform();
			const noob::globals& g = noob::get_globals();
			const noob::shape tmp_shape = g.shapes.get(bod.get_shape());
			const noob::mesh_3d tmp_mesh = tmp_shape.get_mesh();
			// Add up triangles independently
			for (uint32_t ii = 0; ii < tmp_mesh.indices.size(); ++ii)
			{
				const uint32_t vert_index = tmp_mesh.indices[ii];
				const auto tmp_vert = tmp_mesh.vertices[vert_index];
				tmp_vertices.push_back(model_mat * noob::vec4f(tmp_vert.position, 1.0));
				tmp_vertices.push_back(noob::vec4f(tmp_vert.normal, 0.0));
				tmp_vertices.push_back(tmp_vert.colour);
			}
		}

		num_terrain_verts = tmp_vertices.size() / 3;

		noob::graphics& gfx = noob::get_graphics();
		noob::gpu_write_buffer buf = gfx.map_terrain_buffer(0, tmp_vertices.size());

		if (!buf.valid())
		{
			logger::log(noob::importance::ERROR, "[Stage] Could not map instanced matrices buffer for terrain.");
			gfx.unmap_buffer();		
			return;
		}

		for (uint32_t i = 0; i < tmp_vertices.size(); ++i)
		{
			buf.push_back(tmp_vertices[i]);
		}

		logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(num_terrain_verts), " terrain vertices uploaded."));

		gfx.unmap_buffer();
	}
}


void noob::stage::reserve_models(const noob::instanced_model_handle Handle, uint32_t Num) noexcept(true)
{
	noob::fast_hashtable::cell *results = models_to_drawable_instances.lookup(Handle.index());

	// If we haven't gotten a model with that handle yet...
	if (!models_to_drawable_instances.is_valid(results))
	{
		results = models_to_drawable_instances.insert(Handle.index());

		noob::stage::drawable_info info;
		info.model = Handle;
		info.count = 0;
		info.needs_colours = true;

		drawables.push_back(info);

		const uint32_t results_index  = drawables.size() - 1;
		drawables[results_index].instances.resize(Num);

		results->value = results_index;

		noob::graphics& gfx = noob::get_graphics();
		gfx.resize_buffers(Handle, Num);
	}
	else
	{
		const uint32_t index = results->value;
		const uint32_t old_max = drawables[index].instances.size();
		const uint32_t max_new = std::max(old_max, Num);

		drawables[index].instances.resize(max_new);
		// drawables[index].needs_colours = true;
		noob::graphics& gfx = noob::get_graphics();
		gfx.resize_buffers(Handle, Num);
	}
}


void noob::stage::accept_ndof_data(const noob::ndof::data& info) noexcept(true)
{
	if (info.valid == true)
	{
		ndof_happened = true;

		const float damping = 360.0;

		ndof_rotation[0] = static_cast<float>(-info.rx) / static_cast<float>(damping);
		ndof_rotation[1] = static_cast<float>(-info.ry) / static_cast<float>(damping);
		ndof_rotation[2] = static_cast<float>(-info.rz) / static_cast<float>(damping);

		ndof_translation[0] =  static_cast<float>(-info.x) / static_cast<float>(damping);
		ndof_translation[1] = static_cast<float>(-info.y) / static_cast<float>(damping);
		ndof_translation[2] = static_cast<float>(-info.z) / static_cast<float>(damping);
	}
}
