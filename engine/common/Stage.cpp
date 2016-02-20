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


noob::bodies_holder::handle noob::stage::body(noob::body_type b_type, noob::shapes_holder::handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
{
	std::unique_ptr<noob::body> b = std::make_unique<noob::body>();
	b->init(dynamics_world, b_type, globals::shapes.get(shape_h), mass, pos, orient, ccd);
	auto bod_h = bodies.add(std::move(b));
	bodies_to_shapes.insert(std::make_pair(bod_h.get_inner(), shape_h));
	return bod_h;
}


void noob::stage::actor(const noob::bodies_holder::handle _bod, noob::animated_models_holder::handle _model, const std::string& shading)
{

}



void noob::stage::prop(const noob::bodies_holder::handle _bod, noob::basic_models_holder::handle _model, const std::string& shading)
{

}


void noob::stage::scenery(const noob::meshes_holder::handle h, const noob::vec3& pos, const std::string& shading, const noob::versor& orient)
{
	noob::shapes_holder::handle s_handle = globals::static_trimesh(h);
	// shapes.get(s_handle)->set_margin(1.0);
	noob::bodies_holder::handle b_handle = body(noob::body_type::STATIC, s_handle, 0.0, pos, orient);
	noob::basic_models_holder::handle model_handle = std::get<0>(globals::get_model(s_handle));

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
