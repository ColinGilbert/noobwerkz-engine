#include "Stage.hpp"

#include <fstream>

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

	draw_graph.reserveNode(NUM_RESERVED_NODES);
	draw_graph.reserveArc(NUM_RESERVED_ARCS);

	root_node = draw_graph.addNode();

	logger::log("[Stage] Done init.");
}

void noob::stage::tear_down() noexcept(true) 
{
	draw_graph.clear();

	for (size_t i = 0; i < bodies.count(); ++i)
	{
		remove_body(body_handle::make(i));
	}

	bodies.empty();
	joints.empty();
	ghosts.empty();

	bodies_to_nodes.clear();
	basic_models_to_nodes.clear();

	delete dynamics_world;
	delete broadphase;
	delete solver;
	delete collision_dispatcher;
	delete collision_configuration;

	init();
}


void noob::stage::update(double dt) noexcept(true) 
{
	dynamics_world->stepSimulation(1.0/60.0, 10);
}


void noob::stage::draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) const noexcept(true) 
{
	noob::mat4 view_mat(noob::look_at(eye_pos, eye_target, eye_up));
	bgfx::setViewTransform(0, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);
	bgfx::setViewTransform(1, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(1, 0, 0, window_width, window_height);

	bgfx::setUniform(noob::graphics::ambient.handle, &ambient_light.v[0]);

	// static const noob::graphics::uniform invalid_uniform, colour_0, colour_1, colour_2, colour_3, blend_0, blend_1, tex_scales, normal_mat, normal_mat_modelspace, eye_pos, eye_pos_normalized, ambient, light_pos_radius, light_rgb_falloff, specular_shine, diffuse, emissive, fog, rough_albedo_fresnel;
	std::array<noob::light, MAX_LIGHTS> temp_lights;

	noob::globals& g = noob::globals::get_instance();

	for (size_t i = 0; i < MAX_LIGHTS; ++i)
	{
		temp_lights[i] = g.lights.get(lights[i]);
	}

	for (lemon::ListDigraph::OutArcIt model_it(draw_graph, root_node); model_it != lemon::INVALID; ++model_it)
	{
		lemon::ListDigraph::Node model_node = draw_graph.target(model_it);
		size_t model_h = basic_models_mapping[model_node];

		for (lemon::ListDigraph::OutArcIt shading_it(draw_graph, model_node); shading_it != lemon::INVALID; ++shading_it)
		{
			lemon::ListDigraph::Node shading_node = draw_graph.target(shading_it);
			noob::globals::shader_results shader_h = shaders_mapping[shading_node];

			for (lemon::ListDigraph::OutArcIt body_it(draw_graph, shading_node); body_it != lemon::INVALID; ++body_it)
			{
				lemon::ListDigraph::Node body_node = draw_graph.target(body_it);
				size_t body_h = bodies_mapping[body_node];
				noob::vec3 scales = noob::vec3_from_array(scales_mapping[body_node]);

				noob::mat4 world_mat = noob::identity_mat4();

				// Prior to this hack, we fed the results of body.get_orientation directly into the rotate() function.
				// This caused all objects drawn flipped about the x-axis 180 degrees
				noob::versor original_quat = bodies.get(body_handle::make(body_h)).get_orientation();
				noob::versor temp_quat(original_quat.q[3], original_quat.q[2], original_quat.q[1], original_quat.q[0]); 

				world_mat = noob::rotate(world_mat, temp_quat);//bodies.get(body_h).get_orientation());
				world_mat = noob::scale(world_mat, noob::vec3_from_array(scales_mapping[body_node]));												
				world_mat = noob::translate(world_mat, bodies.get(body_handle::make(body_h)).get_position());												
				noob::mat4 normal_mat = noob::transpose(noob::inverse((world_mat * view_mat)));

				noob::reflectance temp_reflect;
				temp_reflect = g.reflectances.get(reflectance_handle::make(reflectances_mapping[body_node]));

				// std::array<noob::light, 4> temp_lights;
				// std::array<size_t, 4> lights_h = lights_mapping[body_node];

				// for(size_t i = 0; i < 3; ++i)
				// {
				// 	temp_lights[i] = globals->lights.get(lights_h[i]);
				// }

				switch(shader_h.type)
				{
					case(noob::shader_type::BASIC):
						{
							g.basic_drawer.draw(g.basic_models.get(model_handle::make(model_h)), world_mat, normal_mat, eye_pos, g.basic_shaders.get(basic_shader_handle::make(shader_h.handle)), temp_reflect, temp_lights, 0);
							break;
						}
					case(noob::shader_type::TRIPLANAR):
						{
							g.triplanar_drawer.draw(g.basic_models.get(model_handle::make(model_h)), world_mat, normal_mat, eye_pos, g.triplanar_shaders.get(triplanar_shader_handle::make(shader_h.handle)), temp_reflect, temp_lights, 0);
							break;
						}
					default:
						{
							fmt::MemoryWriter ww;
							ww << "[Stage] Attempting to draw with a shader that doesn't exist. WHYY??";
							logger::log(ww.str());
							break;
						}
				}
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


noob::body_handle noob::stage::add_body(const noob::body_type b_type, const noob::shape_handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();
	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	
	body_handle bod_h = bodies.add(b);
	b = bodies.get(bod_h);
	b.inner->setUserIndex(bod_h.get_inner());
	return bod_h;
}


noob::ghost_handle noob::stage::add_ghost(const noob::shape_handle shape_h, const noob::vec3& pos, const noob::versor& orient) noexcept(true) 
{
	if (!ghosts_initialized)
	{
		// For the ghost object to work correctly, we need to add a callback to our world.
		dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		ghosts_initialized = true;
	}
	noob::globals& g = noob::globals::get_instance();
	noob::ghost temp_ghost;
	temp_ghost.init(g.shapes.get(shape_h), pos, orient);
	dynamics_world->addCollisionObject(temp_ghost.inner);
	noob::ghost_handle ghost_h = ghosts.add(temp_ghost);
	temp_ghost = ghosts.get(ghost_h);
	temp_ghost.inner->setUserIndex(ghost_h.get_inner());
	return ghost_h;
}


noob::joint_handle noob::stage::joint(const noob::body_handle a, const noob::vec3& local_point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true) 
{
	noob::joint_handle h;


	return h;
}


void noob::stage::actor(const noob::body_handle body_h, const noob::animated_model_handle model_h, const noob::globals::shader_results shader_h) noexcept(true) 
{

}


void noob::stage::actor(const noob::body_handle body_h, const noob::scaled_model model_info, const noob::globals::shader_results shader_h) noexcept(true) 
{
	auto body_results = bodies_to_nodes.lookup(body_h.get_inner());

	// Filter out duplicate bodies by checking existence in the table
	if (bodies_to_nodes.is_valid(body_results))
	{
		logger::log("[Stage] Warning: Attempting to create actor with duplicate body. Aborted.");
		return;
	}

	lemon::ListDigraph::Node model_node;

	noob::fast_hashtable::cell* model_results = basic_models_to_nodes.lookup(model_info.model_h.get_inner());

	if (basic_models_to_nodes.is_valid(model_results))
	{
		model_node = draw_graph.nodeFromId(model_results->value);
	}

	else
	{
		model_node = draw_graph.addNode();

		basic_models_mapping[model_node] = model_info.model_h.get_inner();

		draw_graph.addArc(root_node, model_node);

		auto temp_cell = basic_models_to_nodes.insert(model_info.model_h.get_inner());
		temp_cell->value = draw_graph.id(model_node);
	}

	bool shader_found = false;
	lemon::ListDigraph::Node shader_node;

	for (lemon::ListDigraph::OutArcIt shader_it(draw_graph, model_node); shader_it != lemon::INVALID; ++shader_it)
	{
		lemon::ListDigraph::Node temp_shader_node = draw_graph.target(shader_it);
		noob::globals::shader_results test_value = shaders_mapping[temp_shader_node];
		if (shader_h == test_value)
		{
			shader_found = true;
			shader_node = temp_shader_node;
			break;
		}
	}

	if (!shader_found)
	{
		shader_node = draw_graph.addNode();
		shaders_mapping[shader_node] = shader_h;
		draw_graph.addArc(model_node, shader_node);
	}

	lemon::ListDigraph::Node body_node = draw_graph.addNode();
	bodies_mapping[body_node] = body_h.get_inner();

	auto temp_body_cell = bodies_to_nodes.insert(body_h.get_inner());
	temp_body_cell->value = draw_graph.id(body_node);

	scales_mapping[body_node] = model_info.scales.v;
	reflectances_mapping[body_node] = model_info.reflect_h.get_inner();
	draw_graph.addArc(shader_node, body_node);

	noob::globals& g = noob::globals::get_instance();
	noob::light_handle light_h = g.default_light;
	lights_mapping[body_node] = {light_h.get_inner(), light_h.get_inner(), light_h.get_inner(), light_h.get_inner()};

	// fmt::MemoryWriter ww;
	// ww << "[Stage] Created actor with model " << model_info.model_h.get_inner() << ", shader " << shader_h.get_inner() << ", reflectance " << model_info.reflect_h.get_inner() << " and body " << body_h.get_inner() << ".";
	// logger::log(ww.str());
}


/*
   void noob::stage::actor(const noob::shape_handle shape_h , float mass, const noob::vec3& pos, const noob::versor& orient, const noob::scaled_model model_info, const noob::globals::shader_results shader_h)
   {
   noob::body_handle body_h = body(noob::body_type::DYNAMIC, shape_h, mass, pos, orient);
   actor(body_h, model_info, shader_h);
   }
   */


void noob::stage::actor(const noob::shape_handle shape_h , float mass, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results shader_h, const noob::reflectance_handle reflect_arg) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();
	noob::shape s = g.shapes.get(shape_h);
	if (s.get_type() != noob::shape::type::TRIMESH)
	{
		noob::body_handle body_h = add_body(noob::body_type::DYNAMIC, shape_h, mass, pos, orient);
		noob::scaled_model model_info = g.model_by_shape(shape_h);
		model_info.reflect_h = reflect_arg;
		actor(body_h, model_info, shader_h);
	}
	else
	{
		logger::log("[Stage] Attempting to create actor with static mesh.");
	}
}


void noob::stage::scenery(const noob::basic_mesh& m, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results shader_h, const noob::reflectance_handle reflect_arg, const std::string& name) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();
	noob::shape_handle shape_h = g.static_trimesh_shape(m, name);
	noob::body_handle body_h = add_body(noob::body_type::STATIC, shape_h, 0.0, pos, orient);
	noob::scaled_model model_info;
	model_info = g.model_from_mesh(m, name);
	model_info.reflect_h = reflect_arg;
	actor(body_h, model_info, shader_h);
}


void noob::stage::set_light(unsigned int i, const noob::light_handle h) noexcept(true) 
{
	if (i < MAX_LIGHTS)
	{
		lights[i] = h;
	}	
}


void noob::stage::set_directional_light(const noob::directional_light& l) noexcept(true) 
{
	directional_light = l;
}


noob::light_handle noob::stage::get_light(unsigned int i) const noexcept(true) 
{
	noob::light_handle l;

	if (i < MAX_LIGHTS)
	{
		l = lights[i];
	}

	return l;
}

void noob::stage::remove_body(noob::body_handle h) noexcept(true) 
{
	if (bodies.exists(h) && h.get_inner() != 0)
	{
		noob::body b = bodies.get(h);
		if (b.physics_valid)
		{
			dynamics_world->removeRigidBody(b.inner);
			delete b.inner;
		}
	}	
}

noob::stage::ghost_intersection_results noob::stage::get_intersections(const noob::ghost_handle ghost_h) const noexcept(true) 
{
	noob::ghost temp_ghost = ghosts.get(ghost_h);

	btManifoldArray manifold_array;

	btBroadphasePairArray& pairArray = temp_ghost.inner->getOverlappingPairCache()->getOverlappingPairArray();

	noob::stage::ghost_intersection_results results;
	results.ghost = ghost_handle::make(temp_ghost.inner->getUserIndex());

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

			// Avoid duplicates
			if (manifold->getBody0() == temp_ghost.inner)
			{
				const btCollisionObject* bt_obj = manifold->getBody1();
				// Sanity check
				const int index = bt_obj->getUserIndex();
				if (index > -1)
				{
					if (static_cast<noob::body_descriptor*>(bt_obj->getUserPointer())->is_physical() == true)
					{
						results.bodies.push_back(body_handle::make(index));
					}
					else
					{
						results.ghosts.push_back(ghost_handle::make(index));
					}
				}
				// btScalar direction = is_first_body ? btScalar(-1.0) : btScalar(1.0);

				// for (size_t p = 0; p < manifold->getNumContacts(); ++p)
				// {
				// const btManifoldPoint& pt = manifold->getContactPoint(p);

				// if (pt.getDistance() < 0.0f)
				// {
				// const btVector3& pt_a = pt.getPositionWorldOnA();
				// const btVector3& pt_b = pt.getPositionWorldOnB();
				// const btVector3& normal_on_b = pt.m_normalWorldOnB;



				// Handle collisions here. Thanks Bullet Wiki :)


				// }
				// }
			}
		}
	}

	return results;
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
