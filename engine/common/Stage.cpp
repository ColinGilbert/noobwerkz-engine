#include "Stage.hpp"

/*
noob::stage::~stage()
{
	// delete dynamics_world;
	// delete solver;
	// delete collision_configuration;
	// delete collision_dispatcher;
	// delete broadphase;
}
*/

void noob::stage::init()
{
	dynamics_world = NewtonCreate ();
	
	// broadphase = new btDbvtBroadphase();
	// collision_configuration = new btDefaultCollisionConfiguration();
	// collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	// solver = new btSequentialImpulseConstraintSolver();
	// dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	// dynamics_world->setGravity(btVector3(0, -10, 0));
	// dynamics_world->setApplySpeculativeContactRestitution(true);
	// dynamics_world->setSynchronizeAllMotionStates(true);
	renderer.init();

	auto temp_mesh (pool.register_component<noob::meshes::handle>("mesh"));
	mesh_tag.inner = temp_mesh;

	auto temp_path (pool.register_component<std::vector<noob::vec3>>("path"));
	path_tag.inner = temp_path;

	// auto temp_shape (pool.register_component<noob::shapes::handle>("shape"));
	// shape_tag.inner = temp_shape;

	auto temp_body (pool.register_component<noob::bodies::handle>("movement-controller"));
	body_tag.inner = temp_body;

	auto temp_basic_model (pool.register_component<noob::basic_models::handle>("basic-model"));
	basic_model_tag.inner = temp_basic_model;

	auto temp_animated_model (pool.register_component<noob::animated_models::handle>("animated-model"));
	animated_model_tag.inner = temp_animated_model;

	auto temp_skeletal_anim (pool.register_component<noob::skeletal_anims::handle>("skeletal-anim"));
	skeletal_anim_tag.inner = temp_skeletal_anim;

	auto temp_shader (pool.register_component<noob::shaders::handle>("shading"));
	shader_tag.inner = temp_shader;	


	unit_sphere_mesh = meshes_holder.add(std::make_unique<noob::basic_mesh>(noob::mesh_utils::sphere(0.5)));
	unit_cube_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(noob::mesh_utils::box(1.0, 1.0, 1.0)));
	unit_capsule_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(noob::mesh_utils::capsule(0.5, 1.0)));
	unit_cylinder_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(noob::mesh_utils::cylinder(0.5, 1.0)));
	unit_cone_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(noob::mesh_utils::cone(0.5, 1.0)));


	logger::log("[Stage] making unit sphere model");
	unit_sphere_model = basic_model(unit_sphere_mesh);
	logger::log("[Stage] making unit cube model");
	unit_cube_model = basic_model(unit_cube_mesh);
	logger::log("[Stage] making unit capsule model");
	unit_capsule_model = basic_model(unit_capsule_mesh);
	logger::log("[Stage] making unit cylinder model");
	unit_cylinder_model = basic_model(unit_cylinder_mesh);
	logger::log("[Stage] making unit cone model");
	unit_cone_model = basic_model(unit_cone_mesh);

	meshes_to_models[unit_sphere_mesh.get_inner()] = unit_sphere_model;
	meshes_to_models[unit_cube_mesh.get_inner()] = unit_cube_model;
	meshes_to_models[unit_capsule_mesh.get_inner()] = unit_capsule_model;
	meshes_to_models[unit_cylinder_mesh.get_inner()] = unit_cylinder_model;
	meshes_to_models[unit_cone_mesh.get_inner()] = unit_cone_model;

	//  Init basic default shader
	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 1.0, 1.0, 1.0);
	set_shader(basic_shader_info, "debug"); 
	debug_shader = get_shader("debug");

	// Init triplanar shader. For fun.
	noob::triplanar_gradient_map_renderer::uniform_info triplanar_info;
	std::array<noob::vec4,4> colours;
	triplanar_info.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[1] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[2] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[3] = noob::vec4(1.0, 1.0, 1.0, 1.0);

	triplanar_info.mapping_blends = noob::vec3(1.0, 1.0, 1.0);
	triplanar_info.scales = noob::vec3(1.0, 1.0, 1.0);
	triplanar_info.colour_positions = noob::vec2(0.3, 0.6);
	set_shader(triplanar_info, "default-triplanar");
	default_triplanar_shader = get_shader("default-triplanar");

	logger::log("[Stage] init complete.");
}


void noob::stage::tear_down()
{

}


void noob::stage::update(double dt)
{
	// dynamics_world->stepSimulation(1.0/60.0, 10);
}


void noob::stage::draw() const
{
	// TODO: Use culling to determine which items are invisible.
	pool.for_each<noob::basic_models::handle, noob::bodies::handle, noob::shaders::handle>(basic_model_tag.get(), body_tag.get(), shader_tag.get(), [this](es::storage::iterator, noob::basic_models::handle& m, noob::bodies::handle& b, noob::shaders::handle& s) -> long unsigned int
	{
		//logger::log("drawloop");
		// logger::log(bodies_holder.get(b)->get_debug_string());
		renderer.draw(basic_models_holder.get(m), shaders_holder.get(s), bodies_holder.get(b)->get_transform());
	});
}


noob::bodies::handle noob::stage::sphere_body(float mass, float r, noob::vec3& pos)
{
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();	
	b->sphere(mass, r, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}

noob::bodies::handle noob::stage::box_body(float mass, float x, float y, float z, noob::vec3& pos)
{
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();
	b->box(mass, x, y, z, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}

/*
noob::bodies::handle noob::stage::capsule(float mass, float r, float h, noob::vec3& pos)
{
	std::unique_ptr<noob:body_controller> b;
	b->capsule(mass, r, h, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}
*/


noob::bodies::handle noob::stage::cone_body(float mass, float r, float h, noob::vec3& pos)
{
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();
	b->cone(mass, r, h, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}


noob::bodies::handle noob::stage::cylinder_body(float mass, float r, float h, noob::vec3& pos)
{
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();
	b->cylinder(mass, r, h, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}


noob::bodies::handle noob::stage::static_mesh_body(const noob::meshes::handle h, const noob::vec3& pos)
{
	noob::basic_mesh* m = meshes_holder.get(h);
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();
	b->static_mesh(*m, pos, dynamics_world);
	return bodies_holder.add(std::move(b));
}


noob::meshes::handle noob::stage::add_mesh(const noob::basic_mesh& m)
{
	std::unique_ptr<noob::basic_mesh> mesh_ptr = std::make_unique<noob::basic_mesh>();
	*mesh_ptr = noob::mesh_utils::copy(m);
	return meshes_holder.add(std::move(mesh_ptr));	
}


noob::basic_models::handle noob::stage::basic_model(const noob::meshes::handle _mesh)
{
	auto search = meshes_to_models.find(_mesh.get_inner());
	if (search == meshes_to_models.end())
	{		
		logger::log("[Stage] basic_model() - Mesh not found. Making model");
		std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
		temp->init(noob::mesh_utils::copy(*meshes_holder.get(_mesh)));
		noob::basic_models::handle h = basic_models_holder.add(std::move(temp));
		meshes_to_models.insert(std::make_pair(_mesh.get_inner(), h));
		return h;
	}
	return search->second;
}


noob::animated_models::handle noob::stage::animated_model(const std::string& filename)
{
	std::unique_ptr<noob::animated_model> temp = std::make_unique<noob::animated_model>();
	temp->init(filename);
	return animated_models_holder.add(std::move(temp));
}


noob::skeletal_anims::handle noob::stage::skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletal_anims_holder.add(std::move(temp));
}


void noob::stage::set_light(const noob::light& l, const std::string& s)
{
	auto search = names_to_lights.find(s);
	if (search != names_to_lights.end())
	{
		lights_holder.set(search->second, l);
	}
	else
	{
		names_to_lights[s] = lights_holder.add(l);
	}
}


noob::light noob::stage::get_light(const std::string& s)
{
	noob::lights::handle temp;
	if (names_to_lights.find(s) != names_to_lights.end())
	{
		temp = names_to_lights[s];
	}
	return lights_holder.get(temp);
}


void noob::stage::set_reflection(const noob::reflection& r, const std::string& s)
{
	auto search = names_to_reflections.find(s);
	if (search != names_to_reflections.end())
	{
		reflections_holder.set(search->second, r);
	}
	else
	{
		names_to_reflections[s] = reflections_holder.add(r);
	}
}


noob::reflection noob::stage::get_reflection(const std::string& s)
{
	noob::reflections::handle temp;
	if (names_to_reflections.find(s) != names_to_reflections.end())
	{
		temp = names_to_reflections[s];
	}
	return reflections_holder.get(temp);
}


void noob::stage::set_shader(const noob::prepared_shaders::info& i, const std::string& s)
{
	auto search = names_to_shaders.find(s);
	if (search != names_to_shaders.end())
	{
		shaders_holder.set(search->second, i);
	}
	else
	{
		names_to_shaders[s] = shaders_holder.add(i);
	}
}


noob::shaders::handle noob::stage::get_shader(const std::string& s)
{
	noob::shaders::handle temp;
	if (names_to_shaders.find(s) != names_to_shaders.end())
	{
		return names_to_shaders.at(s);
	}
	return default_triplanar_shader;
}


es::entity noob::stage::actor(const noob::bodies::handle _bod, noob::animated_models::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, _bod);
	pool.set(temp, animated_model_tag.inner, _model);
	pool.set(temp, shader_tag.inner, get_shader(shading));
	
	return temp;
}


es::entity noob::stage::prop(const noob::bodies::handle _bod, noob::basic_models::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, bodies_holder.get(_bod));
	pool.set(temp, basic_model_tag.inner, _model);
	pool.set(temp, shader_tag.inner, get_shader(shading));
	return temp;
}


es::entity noob::stage::scenery(const noob::meshes::handle h, const noob::vec3& pos, const std::string& shading, const noob::versor& orient)
{
	noob::bodies::handle b_handle = static_mesh_body(h, pos);	
	noob::basic_models::handle m_handle = basic_model(h);

	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, b_handle);
	pool.set(temp, basic_model_tag.inner, m_handle);
	pool.set(temp, shader_tag.inner, get_shader(shading));
	return temp;
}
