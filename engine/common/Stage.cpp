#include "Stage.hpp"
// #include "Shiny.h"


void noob::stage::init(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true) 
{
	update_viewport_params(width, height, projection_mat);

	noob::vec3 eye_pos, eye_target, eye_up;
	eye_pos = noob::vec3(0.0, 0.0, -40.0);
	eye_target = noob::vec3(0.0, 0.0, 10.0);
	eye_up = noob::vec3(0.0, 1.0, 0.0);
	//const noob::vec3 eye_forward = eye_pos - eye_target;
	//eye_up = noob::cross(eye_forward, noob::vec3(0.0, 1.0, 0.0));
	//eye_up = noob::cross(eye_up, eye_forward);
	view_matrix = noob::look_at(eye_pos, eye_target, eye_up);
	projection_matrix = projection_mat;

	noob::graphics& gfx = noob::get_graphics();
	gfx.set_view_mat(view_matrix);// = noob::get_graphics();
	gfx.set_projection_mat(projection_matrix);

	world.init();

	main_light.colour = noob::vec4(1.0, 1.0, 1.0, 0.3);
	main_light.direction = noob::vec4(-0.3333, -1.0, -0.3333, 1.0);

	team_colours.push_back(noob::vec4(1.0, 1.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 0.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 1.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 0.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 1.0, 0.0, 1.0));
	team_colours.push_back(noob::vec4(0.0, 1.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(1.0, 0.0, 1.0, 1.0));
	team_colours.push_back(noob::vec4(0.2, 0.2, 0.2, 1.0));
	team_colours.push_back(noob::vec4(0.2, 0.5, 0.2, 1.0));

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

	init(viewport_width, viewport_height, projection_matrix);
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


	world.step(1.0/60.0);

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

	noob::graphics& gfx = noob::get_graphics();
	noob::graphics::program_handle prog = gfx.get_instanced_shader();
	gfx.use_program(prog);

	gfx.set_view_mat(view_matrix);
	gfx.set_projection_mat(projection_matrix);
	
	gfx.set_light_direction(noob::normalize(noob::vec3(0.5, -1.0, 0.33)));

	for (uint32_t drawables_index = 0; drawables_index < drawables.size(); ++drawables_index)
	{
		const noob::stage::drawable_info_handle handle = noob::stage::drawable_info_handle::make(drawables_index);
		if (drawables[drawables_index].needs_colours)
		{
			upload_colours(handle);
			drawables[drawables_index].needs_colours = false;
		}

		upload_matrices(handle);

		const noob::model_handle modl_h = drawables[drawables_index].model;
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


void noob::stage::update_viewport_params(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true)
{
	viewport_width = width;
	viewport_height = height;
	projection_matrix = projection_mat;
}


void noob::stage::build_navmesh() noexcept(true)
{

}


void noob::stage::rebuild_graphics(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true)
{
	noob::graphics& gfx = noob::get_graphics();
	update_viewport_params(width, height, projection_mat);
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

			const noob::ghost_handle ghost_h = world.add_ghost(info.bp.bounds, pos, orient);

			noob::actor a;
			a.team = team;
			a.ghost = ghost_h;
			a.bp_handle = bp_h;

			const noob::actor_handle a_h = actors.add(a);

			noob::fast_hashtable::cell* results = models_to_instances.lookup(info.bp.model.index());

			const uint32_t index = results->value;
			const uint32_t old_count = drawables[index].count;

			drawables[index].count++;
			drawables[index].instances[old_count].actor = a_h;

			// Setting index-to-self info:
			noob::ghost& temp_ghost = world.get_ghost(ghost_h);

			temp_ghost.set_user_index_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
			temp_ghost.set_user_index_2(a_h.index());

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


noob::scenery_handle noob::stage::scenery(const noob::shape_handle Shape, const noob::vec3& Pos, const noob::versor& Orient) noexcept(true)
{
	const noob::globals& g = noob::get_globals();
	const noob::body_handle bod_h = world.add_body(noob::body_type::STATIC, Shape, 0.0, Pos, Orient, false);

	noob::scenery sc;
	sc.body = bod_h;

	const noob::scenery_handle scenery_h = sceneries.add(sc);

	noob::body b = world.get_body(bod_h);
	b.set_user_index_1(static_cast<uint32_t>(noob::stage_item_type::SCENERY));
	b.set_user_index_2(scenery_h.index());

	logger::log(noob::importance::INFO, noob::concat("[Stage] Scenery added! Handle ", noob::to_string(scenery_h.index())) );

	terrain_changed = terrain_started = true;

	return scenery_h;
}


std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::actor_handle ah) const noexcept(true)
{
	noob::actor a = actors.get(ah);
	return world.get_intersecting(a.ghost);
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
	/*
	   noob::actor a = actors.get(ah);

	   if (a.alive)
	   {
	   std::vector<noob::contact_point> cps = get_intersecting(ah);
	   if (cps.size() == 0)
	   {
	   noob::vec3 gravity = world.get_gravity();// * a.gravity_coeff;
	   noob::ghost& gst = world.get_ghost(a.ghost);
	   noob::vec3 temp_pos = gst.get_position();
	   temp_pos += gravity;
	   gst.set_position(temp_pos);
	   }
	   }
	   */
}


void noob::stage::upload_colours(drawable_info_handle arg) const noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::model_handle model_h = drawables[arg.index()].model;

	noob::graphics& gfx = noob::get_graphics();

	noob::gpu_write_buffer buf = gfx.map_instanced_data_buffer(model_h, noob::model::instanced_data_type::COLOUR, 0, count);

	if (buf.valid() == false)
	{
		logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced colour buffer for model ", noob::to_string(arg.index())));	
		return;
	}

	uint32_t current = 0;
	while (current < count)
	{
		const noob::actor_handle a_h = drawables[arg.index()].instances[current].actor;
		const noob::actor a = actors.get(a_h);
		const noob::vec4 colour = team_colours[a.team];
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


void noob::stage::upload_matrices(drawable_info_handle arg) noexcept(true)
{
	const uint32_t count = drawables[arg.index()].count;

	const uint32_t theoretical_max = drawables[arg.index()].instances.size();
	assert(count <= theoretical_max);

	const noob::model_handle model_h = drawables[arg.index()].model;
	noob::graphics& gfx = noob::get_graphics();
	noob::gpu_write_buffer buf = gfx.map_instanced_data_buffer(model_h, noob::model::instanced_data_type::MATRICES, 0, count);
	if (buf.valid() == false)
	{
		logger::log(noob::importance::ERROR, noob::concat("[Stage] Could not map instanced matrices buffer for model ", noob::to_string(arg.index())));	
		return;
	}

	const noob::mat4 viewproj_mat = projection_matrix * view_matrix;
	uint32_t current = 0;
	while (current < count)
	{
		const noob::stage::drawable_instance info = drawables[arg.index()].instances[current];
		const noob::actor a = actors.get(info.actor);

		const noob::ghost& gst = world.get_ghost(a.ghost);
		const noob::mat4 model_mat = gst.get_transform();

		bool valid = buf.push_back(model_mat);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu matrices buffer for model ", noob::to_string(arg.index()), " MVP"));
			gfx.unmap_buffer();			
			return;
		}

		const noob::mat4 mvp_mat = viewproj_mat * model_mat;
		valid = buf.push_back(mvp_mat);

		if (!valid)
		{
			logger::log(noob::importance::WARNING, noob::concat("[Stage] Tried to overflow gpu matrices buffer for model ", noob::to_string(arg.index()), " normal"));
			gfx.unmap_buffer();
			return;
		}

		++current;
	}

	// logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(current), "*2 matrices uploaded"));

	gfx.unmap_buffer();
}


void noob::stage::upload_terrain() noexcept(true)
{
	if (terrain_started)
	{
		std::vector<noob::vec4> tmp_verts;
		for (uint32_t i = 0; i < sceneries.count(); ++i)
		{
			const noob::scenery sc = sceneries.get(noob::scenery_handle::make(i));
			const noob::body& bod = world.get_body(sc.body);
			const noob::mat4 model_mat = bod.get_transform();
			noob::globals& g = noob::get_globals();
			const noob::shape tmp_shp = g.shapes.get(noob::shape_handle::make(bod.get_shape_index()));
			const noob::basic_mesh tmp_msh = tmp_shp.get_mesh();
			// Add up triangles independently
			for (uint32_t i = 0; i < tmp_msh.indices.size(); ++i)
			{
				tmp_verts.push_back(model_mat * noob::vec4(tmp_msh.vertices[i], 1.0));
				tmp_verts.push_back(noob::vec4(tmp_msh.normals[i], 0.0));
				tmp_verts.push_back(tmp_msh.colours[i]);
			}
		}

		num_terrain_verts = tmp_verts.size() / 3;

		noob::graphics& gfx = noob::get_graphics();
		noob::gpu_write_buffer buf = gfx.map_terrain_buffer(0, tmp_verts.size());

		if (!buf.valid())
		{
			logger::log(noob::importance::ERROR, "[Stage] Could not map instanced matrices buffer for terrain");
			gfx.unmap_buffer();		
			return;
		}

		for (uint32_t i = 0; i < tmp_verts.size(); ++i)
		{
			buf.push_back(tmp_verts[i]);
		}
		logger::log(noob::importance::INFO, noob::concat("[Stage] ", noob::to_string(num_terrain_verts), " terrain vertices uploaded"));

		gfx.unmap_buffer();
	}
}








void noob::stage::reserve_models(noob::model_handle Handle, uint32_t Num) noexcept(true)
{
	noob::fast_hashtable::cell* results = models_to_instances.lookup(Handle.index());

	// If we haven't gotten a model with that handle yet...
	if (!models_to_instances.is_valid(results))
	{
		results = models_to_instances.insert(Handle.index());

		noob::stage::drawable_info info;
		info.model = Handle;
		info.count = 0;
		info.needs_colours = true;

		drawables.push_back(info);

		const uint32_t results_index  = drawables.size() - 1;

		drawables[results_index].instances.resize(Num);

		results->value = results_index;

		noob::graphics& gfx = noob::get_graphics();
		gfx.resize_instanced_data_buffers(Handle, Num);
	}
	else
	{
		// To prevent repeated pointer dereferences
		const uint32_t index = results->value;
		const uint32_t old_max = drawables[index].instances.size();
		const uint32_t max_new = std::max(old_max, Num);

		drawables[index].instances.resize(max_new);

		noob::graphics& gfx = noob::get_graphics();
		gfx.resize_instanced_data_buffers(Handle, Num);
	}
}


void noob::stage::accept_ndof_data(const noob::ndof::data& info) noexcept(true)
{
	if (info.movement == true)
	{
		float damping = 360.0;
		view_matrix = noob::rotate_x_deg(view_matrix, -info.rotation[0] / damping);
		view_matrix = noob::rotate_y_deg(view_matrix, -info.rotation[1] / damping);
		view_matrix = noob::rotate_z_deg(view_matrix, -info.rotation[2] / damping);
		view_matrix = noob::translate(view_matrix, noob::vec3(-info.translation[0] / damping, -info.translation[1] / damping, -info.translation[2] / damping));
	}

}
