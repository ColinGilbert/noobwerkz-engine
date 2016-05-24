#include "Stage.hpp"

#include <fstream>

noob::stage::~stage()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}


void noob::stage::init(const noob::globals* g)
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	draw_graph.reserveNode(NUM_RESERVED_NODES);
	draw_graph.reserveArc(NUM_RESERVED_ARCS);

	root_node = draw_graph.addNode();

	//for (size_t i = 0; i < 6; ++i)
	//{
		//noob::lights_holder::handle;
		//lights[i] =
	//}

	globals = g;

	// noob::bodies_holder::handle temp = body(noob::body_type::ghost, globals->unit_sphere_shape, 0.0, noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0), false);

	// fmt::MemoryWriter ww;
	// ww << "[Stage] init first body. Handle = " << temp.get_inner();
	// logger::log(ww.str());

	logger::log("[Stage] Done init.");
}


// TODO: Implement
void noob::stage::tear_down()
{
	/*
	// TODO: Split this mess into separate funnctions
	// bodies.empty();
	bodies_to_nodes.empty();

	// bodies_mapping.clear();
	// basic_models_mapping.clear();
	// shaders_mapping.clear();

	bodies_to_shapes.empty();
	bodies_to_nodes.empty();
	basic_models_to_nodes.empty();
	shaders_to_nodes.empty();

	draw_graph.clear();

	root_node = draw_graph.addNode();
	*/
	// delete dynamics_world;
	// delete solver;
	// delete collision_configuration;
	// delete collision_dispatcher;
	// delete broadphase;

	// broadphase = new btDbvtBroadphase();
	// collision_configuration = new btDefaultCollisionConfiguration();
	// collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	// solver = new btSequentialImpulseConstraintSolver();
	// dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	// dynamics_world->setGravity(btVector3(0, -10, 0));

	//noob::bodies_holder::handle temp = body(noob::body_type::STATIC, globals.unit_sphere_shape, 0.0, noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0), false);

	// bodies_mapping(draw_graph);
	// basic_models_mapping(draw_graph);
	// shaders_mapping(draw_graph);
}


void noob::stage::update(double dt)
{
	dynamics_world->stepSimulation(1.0/60.0, 10);
}




void noob::stage::draw(float window_width, float window_height) const
{

	bgfx::setViewTransform(0, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);
	bgfx::setViewTransform(1, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(1, 0, 0, window_width, window_height);

	bgfx::setUniform(noob::graphics::ambient.handle, &ambient_light.v[0]);
	bgfx::setUniform(noob::graphics::eye_pos.handle, &eye_pos.v[0]);	
	bgfx::setUniform(noob::graphics::eye_pos_normalized.handle, &(noob::normalize(eye_pos)).v[0]);




	// static const noob::graphics::uniform invalid_uniform, colour_0, colour_1, colour_2, colour_3, blend_0, blend_1, tex_scales, normal_mat, normal_mat_modelspace, eye_pos, eye_pos_normalized, ambient, light_pos_radius, light_rgb_falloff, specular_shine, diffuse, emissive, fog, rough_albedo_fresnel;
	std::array<noob::light, MAX_LIGHTS> temp_lights;

	for (size_t i = 0; i < MAX_LIGHTS; ++i)
	{
		temp_lights[i] = globals->lights.get(lights[i]);
	}

	for (lemon::ListDigraph::OutArcIt model_it(draw_graph, root_node); model_it != lemon::INVALID; ++model_it)
	{
		lemon::ListDigraph::Node model_node = draw_graph.target(model_it);
		size_t model_h = basic_models_mapping[model_node];

		for (lemon::ListDigraph::OutArcIt shading_it(draw_graph, model_node); shading_it != lemon::INVALID; ++shading_it)
		{
			lemon::ListDigraph::Node shading_node = draw_graph.target(shading_it);
			size_t shader_h = shaders_mapping[shading_node];

			for (lemon::ListDigraph::OutArcIt body_it(draw_graph, shading_node); body_it != lemon::INVALID; ++body_it)
			{
				lemon::ListDigraph::Node body_node = draw_graph.target(body_it);
				size_t body_h = bodies_mapping[body_node];
				noob::vec3 scales = noob::vec3(scales_mapping[body_node]);

				noob::body* body_ptr = bodies.get(bodies.make_handle(body_h));
				// noob::mat4 world_mat = noob::identity_mat4();
				// world_mat = noob::scale(world_mat, scales); //noob::scale(world_mat, bodies.get(bodies.make_handle(body_h))->get_transform();
				noob::mat4 world_mat = noob::identity_mat4();
				world_mat = noob::scale(world_mat, noob::vec3(scales_mapping[body_node]));												
				world_mat = noob::rotate(world_mat, bodies.get(body_h)->get_orientation());
				world_mat = noob::translate(world_mat, bodies.get(body_h)->get_position());												
				noob::mat4 normal_mat = noob::transpose(noob::inverse((world_mat * view_mat)));

				noob::reflectance temp_reflect;
				temp_reflect = globals->reflectances.get(reflectances_mapping[body_node]);


				// std::array<noob::light, 4> temp_lights;
				// std::array<size_t, 4> lights_h = lights_mapping[body_node];

				// for(size_t i = 0; i < 3; ++i)
				// {
				// 	temp_lights[i] = globals->lights.get(lights_h[i]);
				// }
				
				renderer.draw(globals->basic_models.get(model_h), globals->shaders.get(shader_h), world_mat, normal_mat, eye_pos, temp_reflect, temp_lights, 0);
			}
		}
	}

		if (show_origin)
		{
			// noob::mat4 world_mat = noob::scale(noob::identity_mat4(), shapes.get(bodies_to_shapes[b.get_inner()])->get_scales());
			// world_mat = bodies.get(b)->get_transform() * world_mat;
			// renderer.draw(basic_models.get(m), shaders.get(s), world_mat);
			// bgfx::setViewTransform(view_id, view_matrix, ortho);
			// bgfx::setViewRect(view_id, 0, 0, window_width, window_height);
			noob::mat4 normal_mat = noob::identity_mat4();

			// renderer.draw(basic_models.get(unit_cube_model), shaders.get(debug_shader), noob::scale(noob::identity_mat4(), noob::vec3(10.0, 10.0, 10.0)), normal_mat, basic_lights);
		}
	}


	noob::bodies_holder::handle noob::stage::body(const noob::body_type b_type, const noob::shapes_holder::handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
	{
		std::unique_ptr<noob::body> b = std::make_unique<noob::body>();
		b->init(dynamics_world, b_type, globals->shapes.get(shape_h), mass, pos, orient, ccd);	
		body_handle bod_h = bodies.add(std::move(b));
		noob::body* temp = bodies.get(bod_h);
		temp->inner_body->setUserIndex(bod_h.get_inner());
		
		// bodies_to_shapes.insert(std::make_pair(bod_h.get_inner(), shape_h));
		return bod_h;
	}


	noob::joint_handle noob::stage::joint(const noob::body_handle a, const noob::vec3& local_point_on_a, const noob::body_handle b, const noob::vec3& point_on_b)
	{
		noob::joint_handle h;
		

		return h;
	}


	void noob::stage::actor(const noob::bodies_holder::handle body_h, const noob::animated_models_holder::handle model_h, const noob::globals::shader_results shader_h)
	{

	}


	void noob::stage::actor(const noob::bodies_holder::handle body_h, const noob::scaled_model model_info, const noob::globals::shader_results shader_h)
	{
		auto body_results = bodies_to_nodes.find(body_h.get_inner());
		if (body_results != bodies_to_nodes.end())
		{
			logger::log("[Stage] Warning: Attempting to use duplicate body. Aborted.");
			return;
		}

		lemon::ListDigraph::Node model_node;

		auto model_results = basic_models_to_nodes.find(model_info.model_h.get_inner());
		if (model_results != basic_models_to_nodes.end())
		{
			model_node = model_results->second;
		}

		else
		{
			model_node = draw_graph.addNode();

			basic_models_mapping[model_node] = model_info.model_h.get_inner();

			draw_graph.addArc(root_node, model_node);

			basic_models_to_nodes.insert(std::make_pair(model_info.model_h.get_inner(), model_node));
		}

		bool shader_found = false;
		lemon::ListDigraph::Node shader_node;
		for (lemon::ListDigraph::OutArcIt shader_it(draw_graph, model_node); shader_it != lemon::INVALID; ++shader_it)
		{
			lemon::ListDigraph::Node temp_shader_node = draw_graph.target(shader_it);
			size_t test_value = shaders_mapping[temp_shader_node];
			if (shader_h.get_inner() == test_value)
			{
				shader_found = true;
				shader_node = temp_shader_node;
				break;
			}
		}
		if (!shader_found)
		{
			shader_node = draw_graph.addNode();
			shaders_mapping[shader_node] = shader_h.get_inner();
			draw_graph.addArc(model_node, shader_node);
		}

		lemon::ListDigraph::Node body_node = draw_graph.addNode();
		bodies_mapping[body_node] = body_h.get_inner();
		scales_mapping[body_node] = model_info.scales.v;
		reflectances_mapping[body_node] = model_info.reflect_h.get_inner();
		draw_graph.addArc(shader_node, body_node);

		noob::lights_holder::handle light_h = globals->default_light;
		lights_mapping[body_node] = {light_h.get_inner(), light_h.get_inner(), light_h.get_inner(), light_h.get_inner()};

		fmt::MemoryWriter ww;
		ww << "[Stage] Created actor with model " << model_info.model_h.get_inner() << ", shader " << shader_h.get_inner() << ", reflectance " << model_info.reflect_h.get_inner() << " and body " << body_h.get_inner() << ".";
		logger::log(ww.str());
	}


/*
	void noob::stage::actor(const noob::shapes_holder::handle shape_h , float mass, const noob::vec3& pos, const noob::versor& orient, const noob::scaled_model model_info, const noob::globals::shader_results shader_h)
	{
		noob::bodies_holder::handle body_h = body(noob::body_type::DYNAMIC, shape_h, mass, pos, orient);
		actor(body_h, model_info, shader_h);
	}
*/


	void noob::stage::actor(const noob::shapes_holder::handle shape_h , float mass, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results shader_h, const noob::reflectances_holder::handle reflect_arg)
	{
		noob::shape* s = globals->shapes.get(shape_h);
		if (s->get_type() != noob::shape::type::TRIMESH)
		{
			noob::bodies_holder::handle body_h = body(noob::body_type::DYNAMIC, shape_h, mass, pos, orient);
			noob::scaled_model model_info = globals->model_by_shape(shape_h);
			model_info.reflect_h = reflect_arg;
			actor(body_h, model_info, shader_h);
		}
		else logger::log("[Stage] Attempting to create actor with static mesh.");
	}


	void noob::stage::scenery(const noob::basic_mesh& m, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results shader_h, const noob::reflectances_holder::handle reflect_arg, const std::string& name)
	{
		noob::shapes_holder::handle shape_h = globals->static_trimesh_shape(m, name);
		noob::bodies_holder::handle body_h = body(noob::body_type::STATIC, shape_h, 0.0, pos, orient);
		noob::scaled_model model_info;
		model_info = globals->model_from_mesh(m, name);
		model_info.reflect_h = reflect_arg;
		actor(body_h, model_info, shader_h);
	}

	
	void noob::stage::set_light(unsigned int i, const noob::lights_holder::handle h)
	{
		if (i < MAX_LIGHTS)
		{
			lights[i] = h;
		}	
	}

	
	noob::lights_holder::handle noob::stage::get_light(unsigned int i) const
	{
		noob::lights_holder::handle l;
	
		if (i < MAX_LIGHTS)
		{
			l = lights[i];
		}
		
		return l;
	}

/*
	void noob::stage::write_graph(const std::string& filename) const
	{
		logger::log("About to write graph");
		boost::filesystem::path p("temp/");
		if (boost::filesystem::exists(p))
		{
			std::string full_path_str = p.generic_string() + filename;
			lemon::digraphWriter(draw_graph, full_path_str).nodeMap("model", basic_models_mapping).nodeMap("body", bodies_mapping).nodeMap("shader", shaders_mapping).node("root", root_node).run();
		}
		else logger::log("[Stage] Could not write graph snapshot - temp directory not found.");
	}
*/
