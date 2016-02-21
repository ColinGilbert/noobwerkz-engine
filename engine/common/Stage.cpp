#include "Stage.hpp"


noob::stage::~stage()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}


void noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	root_node = draw_graph.addNode();

	logger::log("[Stage] Done init.");
}


// TODO: Implement
void noob::stage::tear_down()
{
	bodies.empty();
	bodies_to_shapes.empty();
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
	b->init(dynamics_world, b_type, globals::shapes.get(shape_h), mass, pos, orient, ccd);
	auto bod_h = bodies.add(std::move(b));
	bodies_to_shapes.insert(std::make_pair(bod_h.get_inner(), shape_h));
	return bod_h;
}


void noob::stage::actor(const noob::bodies_holder::handle body_h, const noob::animated_models_holder::handle model_h, const noob::shaders_holder::handle shader_h)
{

}


void noob::stage::prop(const noob::bodies_holder::handle body_h, const noob::basic_models_holder::handle model_h,  const noob::shaders_holder::handle shader_h)
{

}


void noob::stage::scenery(const noob::meshes_holder::handle mesh_h, const noob::shaders_holder::handle shader_h, const noob::vec3& pos, const noob::versor& orient)
{
	noob::shapes_holder::handle shape_h = globals::static_trimesh(mesh_h);
	noob::bodies_holder::handle body_h = body(noob::body_type::STATIC, shape_h, 0.0, pos, orient);
	noob::basic_models_holder::handle model_h = std::get<0>(globals::get_model(shape_h));
	
	bool model_found, shader_found;
	model_found = shader_found = false;

	lemon::SmartDigraph::Node model_node, shader_node;

	auto model_results = basic_models_to_nodes.find(model_h.get_inner());
	if (model_results != basic_models_to_nodes.end())
	{
		model_found = true;
		model_node = model_results->second;
	}

	auto shader_results = shaders_to_nodes.find(shader_h.get_inner());
	if (shader_results != shaders_to_nodes.end())
	{
		shader_found = true;
		shader_node = shader_results->second;
	}
	
	if (!model_found)
	{
		model_node = draw_graph.addNode();
		basic_models_mapping[model_node] = model_h;
		draw_graph.addArc(root_node, model_node);
		basic_models_to_nodes.insert(std::make_pair(model_h.get_inner(), model_node));
	}

	if (!shader_found)
	{
		shader_node = draw_graph.addNode();
		shaders_mapping[shader_node] = shader_h;
		draw_graph.addArc(model_node, shader_node);
		shaders_to_nodes.insert(std::make_pair(shader_h.get_inner(), shader_node));
	}
	
	lemon::SmartDigraph::Node body_node = draw_graph.addNode();
	bodies_mapping[body_node] = body_h;
	draw_graph.addArc(shader_node, body_node);
	bodies_to_nodes.insert(std::make_pair(body_h.get_inner(), body_node));
}

// noob::shaders_holder::handle shader_h = globals::get_shader(shading_name);
/*
	bool model_found = false;
	for (lemon::SmartDigraph::OutArcIt model_it(draw_graph, root_node); model_it != lemon::INVALID; ++model_it)
	{
		if (basic_models_mapping[draw_graph.target(model_it)] == model_h)
		{
			model_found = true;
		}
		if (model_found)
		{
			bool shading_found = false;
			for (lemon::SmartDigraph::OutArcIt shading_it(draw_graph, model_it); shading_it != lemon::INVALID; ++shading_it)
			{
				lemon::SmartDigraph::Node n = draw_graph.target(shading_it)
				if (shaders_mapping[n] == shader_h)
				{
					shading_found = true;
				}
				// If the model + shading was found, add the new node containing the body underneath. 
				if (shading_found)
				{
					lemon::SmartDigraph::Node body_node = draw_graph.addNode();
					bodies_mapping[body_node] = body_h;
					draw_graph.addArc(n, body_node);
					return;
				}
			}
		// If shader hasn't been found for the model, add it, then add the body, and return immediately.
		return;
		}
	}
*/
// At this point in execution, we can safely assume that even the model itself hasn't been found, so we add everything.
// }


void noob::stage::set_basic_light(unsigned int i, const noob::vec4& light)
{
	if (i > basic_lights.size()) basic_lights[basic_lights.size()-1] = light;
	else basic_lights[i] = light;
}


noob::vec4 noob::stage::get_basic_light(unsigned int i) const
{
	if (i > 1) return basic_lights[1];
	else return basic_lights[i];
}
