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

	renderer.init();
	
	draw_graph.reserveNode(NUM_RESERVED_NODES);
	draw_graph.reserveArc(NUM_RESERVED_ARCS);

	root_node = draw_graph.addNode();
	// auto temp = body(noob::body_type::STATIC, globals.unit_sphere_shape(), 0.0, const noob::vec3& pos, const noob::versor& orient, bool ccd);
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

	for (lemon::ListDigraph::OutArcIt model_it(draw_graph, root_node); model_it != lemon::INVALID; ++model_it)
	{
		lemon::ListDigraph::Node model_node = draw_graph.target(model_it);
		noob::basic_models_holder::handle model_h = basic_models_mapping[model_node];
		if (lemon::countOutArcs(draw_graph, model_node) > 0)
		{	
		for (lemon::ListDigraph::OutArcIt shading_it(draw_graph, model_it); shading_it != lemon::INVALID; ++shading_it)
		{
			lemon::ListDigraph::Node shading_node = draw_graph.target(shading_it);
			noob::shaders_holder::handle shader_h = shaders_mapping[shading_node];
			if (lemon::countOutArcs(draw_graph, shading_node) > 0)
			{
				for (lemon::ListDigraph::OutArcIt body_it(draw_graph, shading_it); body_it != lemon::INVALID; ++body_it)
				{
					lemon::ListDigraph::Node body_node = draw_graph.target(body_it);
					noob::bodies_holder::handle body_h = bodies_mapping[body_node];
					noob::mat4 model_mat = bodies.get(body_h)->get_transform();
					renderer.draw(noob::globals::basic_models.get(model_h), noob::globals::shaders.get(shader_h), model_mat, basic_lights);
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


void noob::stage::actor(const noob::bodies_holder::handle body_h, const noob::globals::scaled_model& model_info, const noob::shaders_holder::handle shader_h)
{
	lemon::ListDigraph::Node model_node, shader_node, body_node;

	auto model_results = basic_models_to_nodes.find(model_info.model_h.get_inner());
	if (model_results != basic_models_to_nodes.end())
	{
		model_node = model_results->second;
	}
	else
	{
		model_node = draw_graph.addNode();
		basic_models_mapping[model_node] = model_info.model_h;
		draw_graph.addArc(root_node, model_node);
		basic_models_to_nodes.insert(std::make_pair(model_info.model_h.get_inner(), model_node));
	}

	auto shader_results = shaders_to_nodes.find(shader_h.get_inner());
	if (shader_results != shaders_to_nodes.end())
	{
		shader_node = shader_results->second;
	}
	else
	{
		shader_node = draw_graph.addNode();
		shaders_mapping[shader_node] = shader_h;
		draw_graph.addArc(model_node, shader_node);
		shaders_to_nodes.insert(std::make_pair(shader_h.get_inner(), shader_node));
	}

	body_node = draw_graph.addNode();
	bodies_mapping[body_node] = body_h;
	draw_graph.addArc(shader_node, body_node);
	bodies_to_nodes.insert(std::make_pair(body_h.get_inner(), body_node));
}


void noob::stage::scenery(const noob::basic_mesh& m, const noob::vec3& pos, const noob::versor& orient, const noob::shaders_holder::handle shader_h, const std::string& name)
{
	noob::shapes_holder::handle shape_h = globals::static_trimesh(m, name);
	noob::bodies_holder::handle body_h = body(noob::body_type::STATIC, shape_h, 0.0, pos, orient);
	noob::globals::scaled_model model_info = globals::model_by_shape(shape_h);
	actor(body_h, model_info, shader_h);
}


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
