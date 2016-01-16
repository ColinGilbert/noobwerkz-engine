#include "Stage.hpp"


noob::stage::~stage()
{
	// delete dynamics_world;
	// delete solver;
	// delete collision_configuration;
	// delete collision_dispatcher;
	// delete broadphase;
}


void noob::stage::init()
{
	// broadphase = new btDbvtBroadphase();
	// collision_configuration = new btDefaultCollisionConfiguration();
	// collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	// solver = new btSequentialImpulseConstraintSolver();
	// dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	// dynamics_world->setGravity(btVector3(0, -10, 0));
	// dynamics_world->setApplySpeculativeContactRestitution(true);
	// dynamics_world->setSynchronizeAllMotionStates(true);


	auto temp_scaling (pool.register_component<noob::vec3>("scales"));
	scales_tag.inner = temp_scaling;

	auto temp_mesh (pool.register_component<noob::meshes_holder::handle>("mesh"));
	mesh_tag.inner = temp_mesh;

	auto temp_path (pool.register_component<std::vector<noob::vec3>>("path"));
	path_tag.inner = temp_path;

	auto temp_shape (pool.register_component<noob::shapes_holder::handle>("shape"));
	shape_tag.inner = temp_shape;

	auto temp_shape_type (pool.register_component<noob::shape::type>("shape-type"));
	shape_type_tag.inner = temp_shape_type;

	auto temp_body (pool.register_component<noob::bodies_holder::handle>("movement-controller"));
	body_tag.inner = temp_body;

	auto temp_basic_model (pool.register_component<noob::basic_models_holder::handle>("basic-model"));
	basic_model_tag.inner = temp_basic_model;

	auto temp_animated_model (pool.register_component<noob::animated_models_holder::handle>("animated-model"));
	animated_model_tag.inner = temp_animated_model;

	auto temp_skeletal_anim (pool.register_component<noob::skeletal_anims_holder::handle>("skeletal-anim"));
	skeletal_anim_tag.inner = temp_skeletal_anim;

	auto temp_shader (pool.register_component<noob::shaders_holder::handle>("shading"));
	shader_tag.inner = temp_shader;	

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
	// dynamics_world->stepSimulation(1.0/60.0, 10);
}


void noob::stage::draw(float window_width, float window_height) const
{

	bgfx::setViewTransform(0, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);
	bgfx::setViewTransform(1, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(1, 0, 0, window_width, window_height);


	// TODO: Use culling to determine which items are invisible.
	pool.for_each<noob::basic_models_holder::handle, noob::bodies_holder::handle, noob::shaders_holder::handle>(basic_model_tag.get(), body_tag.get(), shader_tag.get(), [this](es::storage::iterator, noob::basic_models_holder::handle& m, noob::bodies_holder::handle& b, noob::shaders_holder::handle& s) -> long unsigned int
	{
		// logger::log("drawloop");
		// logger::log(bodies.get(b)->get_debug_string());
		noob::mat4 world_mat = noob::scale(noob::identity_mat4(), globals::shapes.get(bodies_to_shapes[b.get_inner()])->get_scales());
		world_mat = bodies.get(b)->get_transform() * world_mat;
		noob::mat4 normal_mat = noob::transpose(noob::inverse((view_mat * world_mat)));
		globals::renderer.draw(globals::basic_models.get(m), globals::shaders.get(s), world_mat, normal_mat, basic_lights);
		return 1;
	});

	if (show_origin)
	{
		// noob::mat4 world_mat = noob::scale(noob::identity_mat4(), shapes.get(bodies_to_shapes[b.get_inner()])->get_scales());
		// world_mat = bodies.get(b)->get_transform() * world_mat;
		// renderer.draw(basic_models.get(m), shaders.get(s), world_mat);
		//bgfx::setViewTransform(view_id, view_matrix, ortho);
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
	//bodies_to_shapes.insert(std::make_pair(bod_h.get_inner(), shape_h));
	return bod_h;
}

unsigned int noob::stage::_body(noob::body_type b_type, unsigned int shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
{
	return body(b_type, globals::shapes.make_handle(shape_h), mass, pos, orient, ccd).get_inner();
}


es::entity noob::stage::actor(const noob::bodies_holder::handle _bod, noob::animated_models_holder::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, _bod);
	pool.set(temp, animated_model_tag.inner, _model);
	pool.set(temp, shader_tag.inner, globals::get_shader(shading));
	return temp;
}


unsigned int noob::stage::_actor(unsigned int _bod, unsigned int _model, const std::string& _shading)
{
	return static_cast<unsigned int>(actor(bodies.make_handle(_bod), globals::animated_models.make_handle(_model), _shading));
}


es::entity noob::stage::prop(const noob::bodies_holder::handle _bod, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, _bod);
	auto bs = bodies_to_shapes[_bod.get_inner()];
	pool.set(temp, basic_model_tag.inner, std::get<0>(globals::get_model(bs)));
	pool.set(temp, scales_tag.inner, globals::shapes.get(bodies_to_shapes[_bod.get_inner()])->get_scales());
	pool.set(temp, shader_tag.inner, globals::get_shader(shading));
	return temp;
}


unsigned int noob::stage::_prop(unsigned int _bod, const std::string& shading)
{
	return static_cast<unsigned int>(prop(bodies.make_handle(_bod), shading));

}

es::entity noob::stage::prop(const noob::bodies_holder::handle _bod, noob::basic_models_holder::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, bodies.get(_bod));
	pool.set(temp, basic_model_tag.inner, _model);
	pool.set(temp, scales_tag.inner, noob::vec3(1.0, 1.0, 1.0));
	pool.set(temp, shader_tag.inner, globals::get_shader(shading));
	return temp;
}


unsigned int noob::stage::_prop(unsigned int _bod, unsigned int _model, const std::string& shading)
{
	return static_cast<unsigned int>(prop(bodies.make_handle(_bod), globals::basic_models.make_handle(_model), shading));
}


es::entity noob::stage::scenery(const noob::meshes_holder::handle h, const noob::vec3& pos, const std::string& shading, const noob::versor& orient)
{
	noob::shapes_holder::handle s_handle = globals::static_trimesh(h);
	// shapes.get(s_handle)->set_margin(1.0);
	noob::bodies_holder::handle b_handle = body(noob::body_type::STATIC, s_handle, 0.0, pos, orient);
	noob::basic_models_holder::handle model_handle = std::get<0>(globals::get_model(s_handle));
	
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, b_handle);
	pool.set(temp, basic_model_tag.inner, model_handle);
	pool.set(temp, scales_tag.inner, std::get<1>(globals::get_model(s_handle)));//bodies.get(b_handle)->inner_shape->get_scales());
	pool.set(temp, shader_tag.inner, globals::get_shader(shading));
	return temp;
}


unsigned int noob::stage::_scenery(unsigned int _mesh, const noob::vec3& pos, const std::string& shading, const noob::versor& orient)
{
	return static_cast<unsigned int>(scenery(globals::meshes.make_handle(_mesh), pos, shading, orient));
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
