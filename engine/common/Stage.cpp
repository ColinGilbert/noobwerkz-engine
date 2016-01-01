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
	// dynamics_world->setApplySpeculativeContactRestitution(true);
	// dynamics_world->setSynchronizeAllMotionStates(true);
	renderer.init();

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


	unit_sphere_shape = sphere(0.5);
	unit_cube_shape = box(1.0, 1.0, 1.0);
	unit_cylinder_shape = cylinder(0.5, 1.0);
	unit_cone_shape = cone(0.5, 1.0);

	unit_sphere_mesh = meshes.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_sphere_shape)));
	unit_cube_mesh =  meshes.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cube_shape)));
	unit_cylinder_mesh =  meshes.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cylinder_shape)));
	unit_cone_mesh =  meshes.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cone_shape)));

	shapes_to_meshes[unit_sphere_shape.get_inner()] = unit_sphere_mesh;
	shapes_to_meshes[unit_cube_shape.get_inner()] = unit_cube_mesh;
	shapes_to_meshes[unit_cylinder_shape.get_inner()] = unit_cylinder_mesh;
	shapes_to_meshes[unit_cone_shape.get_inner()] = unit_cone_mesh;

	logger::log("[Stage] making unit sphere model");
	unit_sphere_model = basic_model(unit_sphere_mesh);
	logger::log("[Stage] making unit cube model");
	unit_cube_model = basic_model(unit_cube_mesh);
	// logger::log("[Stage] making unit capsule model");
	logger::log("[Stage] making unit cylinder model");
	unit_cylinder_model = basic_model(unit_cylinder_mesh);
	logger::log("[Stage] making unit cone model");
	unit_cone_model = basic_model(unit_cone_mesh);

	meshes_to_models[unit_sphere_mesh.get_inner()] = unit_sphere_model;
	meshes_to_models[unit_cube_mesh.get_inner()] = unit_cube_model;
	meshes_to_models[unit_cylinder_mesh.get_inner()] = unit_cylinder_model;
	meshes_to_models[unit_cone_mesh.get_inner()] = unit_cone_model;

	//  Init basic default shader
	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
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
	dynamics_world->stepSimulation(1.0/60.0, 10);
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
		//logger::log("drawloop");
		// logger::log(bodies.get(b)->get_debug_string());
		noob::mat4 world_mat = noob::scale(noob::identity_mat4(), shapes.get(bodies_to_shapes[b.get_inner()])->get_scales());
		world_mat = bodies.get(b)->get_transform() * world_mat;
		noob::mat4 normal_mat = noob::transpose(noob::inverse((view_mat * world_mat)));
		renderer.draw(basic_models.get(m), shaders.get(s), world_mat, normal_mat);
	});

	if (show_origin)
	{
		// noob::mat4 world_mat = noob::scale(noob::identity_mat4(), shapes.get(bodies_to_shapes[b.get_inner()])->get_scales());
		// world_mat = bodies.get(b)->get_transform() * world_mat;
		// renderer.draw(basic_models.get(m), shaders.get(s), world_mat);
		//bgfx::setViewTransform(view_id, view_matrix, ortho);
		// bgfx::setViewRect(view_id, 0, 0, window_width, window_height);
		noob::mat4 normal_mat = noob::identity_mat4();
		renderer.draw(basic_models.get(unit_cube_model), shaders.get(debug_shader), noob::scale(noob::identity_mat4(), noob::vec3(10.0, 10.0, 10.0)), normal_mat);
	}
}


noob::bodies_holder::handle noob::stage::body(noob::body_type b_type, noob::shapes_holder::handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
{
	std::unique_ptr<noob::body> b = std::make_unique<noob::body>();
	b->init(dynamics_world, b_type, shapes.get(shape_h), mass, pos, orient, ccd);
	auto bod_h = bodies.add(std::move(b));
	bodies_to_shapes.insert(std::make_pair(bod_h.get_inner(), shape_h));
	return bod_h;
}


unsigned int noob::stage::_body(noob::body_type b_type, unsigned int shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
{
	return body(b_type, shapes.make_handle(shape_h), mass, pos, orient, ccd).get_inner();
}


noob::shapes_holder::handle noob::stage::sphere(float r)
{
	auto search = sphere_shapes.find(r);
	if (search == sphere_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r);
		auto results = sphere_shapes.insert(std::make_pair(r, shapes.add(std::move(temp))));
		return (results.first)->second;

	}
	return sphere_shapes[r];
}


unsigned int noob::stage::_sphere(float r)
{
	return sphere(r).get_inner();
}


noob::shapes_holder::handle noob::stage::box(float x, float y, float z)
{
	auto search = box_shapes.find(std::make_tuple(x,y,z));
	if (search == box_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);
		auto results = box_shapes.insert(std::make_pair(std::make_tuple(x,y,z), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return box_shapes[std::make_tuple(x,y,z)];
}


unsigned int noob::stage::_box(float x, float y, float z)
{
	return box(x, y, z).get_inner();
}


noob::shapes_holder::handle noob::stage::cylinder(float r, float h)
{
	auto search = cylinder_shapes.find(std::make_tuple(r, h));
	if (search == cylinder_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);
		auto results = cylinder_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return cylinder_shapes[std::make_tuple(r, h)];
}


unsigned int noob::stage::_cylinder(float r, float h)
{
	return cylinder(r, h).get_inner();
}


noob::shapes_holder::handle noob::stage::cone(float r, float h)
{
	auto search = cone_shapes.find(std::make_tuple(r, h));
	if (search == cone_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);
		auto results = cone_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return cone_shapes[std::make_tuple(r, h)];
}


unsigned int noob::stage::_cone(float r, float h)
{
	return cone(r, h).get_inner();
}


/*
noob::shapes_holder::handle noob::stage::capsule(float r, float h)
{
	auto search = capsule_shapes.find(std::make_tuple(r, h));
	if (search == capsule_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->capsule(r, h);
		auto results = capsule_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return capsule_shapes[std::make_tuple(r, h)];
}
*/
/*
   noob::shapes_holder::handle noob::stage::plane(const noob::vec3& normal, float offset)
   {
   auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
   if (search == planes.end())
   {
   std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
   temp->plane(normal, offset);
   auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes.add(std::move(temp))));
   return (results.first)->second;
   }
   return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
   }
   */

noob::shapes_holder::handle noob::stage::hull(const std::vector<vec3> & _points)
{
	// TODO: Add to shapes_to_meshes
	std::unique_ptr<noob::shape> temp_shape = std::make_unique<noob::shape>();
	temp_shape->hull(_points);
	noob::shapes_holder::handle temp_shape_handle = shapes.add(std::move(temp_shape));

	std::unique_ptr<noob::basic_mesh> temp_mesh = std::make_unique<noob::basic_mesh>();

	*temp_mesh = noob::mesh_utils::hull(_points);
	// TODO: Hash temp_mesh to save memory
	noob::meshes_holder::handle temp_mesh_handle = meshes.add(std::move(temp_mesh));
	shapes_to_meshes[temp_shape_handle.get_inner()] = temp_mesh_handle;

	return temp_shape_handle;
}


unsigned int noob::stage::_hull(const std::vector<vec3>& _points)
{
	return hull(_points).get_inner();
}


noob::shapes_holder::handle noob::stage::static_trimesh(const noob::meshes_holder::handle _mesh)
{
	auto search = meshes_to_shapes.find(_mesh.get_inner());
	if (search == meshes_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp_shape_ptr = std::make_unique<noob::shape>();
		temp_shape_ptr->trimesh(meshes.get(_mesh));
		noob::shapes_holder::handle temp_shape = shapes.add(std::move(temp_shape_ptr));
		meshes_to_shapes.insert(std::make_pair(_mesh.get_inner(), temp_shape));
		shapes_to_meshes.insert(std::make_pair(temp_shape.get_inner(), _mesh));
		return temp_shape;
	}
	return search->second;
}


unsigned int noob::stage::_static_trimesh(unsigned int _m)
{
	return static_trimesh(meshes.make_handle(_m)).get_inner();
}


noob::meshes_holder::handle noob::stage::add_mesh(const noob::basic_mesh& m)
{
	std::unique_ptr<noob::basic_mesh> mesh_ptr = std::make_unique<noob::basic_mesh>();
	*mesh_ptr = noob::mesh_utils::copy(m);
	return meshes.add(std::move(mesh_ptr));	
}


unsigned int noob::stage::_add_mesh(const noob::basic_mesh& m)
{
 	return add_mesh(m).get_inner();
}


noob::basic_models_holder::handle noob::stage::basic_model(const noob::meshes_holder::handle _mesh)
{
	auto search = meshes_to_models.find(_mesh.get_inner());
	if (search == meshes_to_models.end())
	{		
		logger::log("[Stage] basic_model() - Mesh not found. Making model");
		std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
		temp->init(noob::mesh_utils::copy(*meshes.get(_mesh)));
		noob::basic_models_holder::handle h = basic_models.add(std::move(temp));
		meshes_to_models.insert(std::make_pair(_mesh.get_inner(), h));
		return h;
	}
	return search->second;
}


unsigned int noob::stage::_basic_model(unsigned int _m)
{
	return basic_model(meshes.make_handle(_m)).get_inner();
}


noob::animated_models_holder::handle noob::stage::animated_model(const std::string& filename)
{
	std::unique_ptr<noob::animated_model> temp = std::make_unique<noob::animated_model>();
	temp->init(filename);
	return animated_models.add(std::move(temp));
}


unsigned int noob::stage::_animated_model(const std::string& filename)
{
	return animated_model(filename).get_inner();
}


noob::skeletal_anims_holder::handle noob::stage::skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletal_anims.add(std::move(temp));
}


unsigned int noob::stage::_skeleton(const std::string& filename)
{
	return skeleton(filename).get_inner();
}


void noob::stage::set_light(const noob::light& l, const std::string& s)
{
	auto search = names_to_lights.find(s);
	if (search != names_to_lights.end())
	{
		lights.set(search->second, l);
	}
	else
	{
		names_to_lights[s] = lights.add(l);
	}
}


noob::light noob::stage::get_light(const std::string& s)
{
	noob::lights_holder::handle temp;
	if (names_to_lights.find(s) != names_to_lights.end())
	{
		temp = names_to_lights[s];
	}
	return lights.get(temp);
}


void noob::stage::set_reflection(const noob::reflection& r, const std::string& s)
{
	auto search = names_to_reflections.find(s);
	if (search != names_to_reflections.end())
	{
		reflections.set(search->second, r);
	}
	else
	{
		names_to_reflections[s] = reflections.add(r);
	}
}


noob::reflection noob::stage::get_reflection(const std::string& s)
{
	noob::reflections_holder::handle temp;
	if (names_to_reflections.find(s) != names_to_reflections.end())
	{
		temp = names_to_reflections[s];
	}
	return reflections.get(temp);
}


void noob::stage::set_shader(const noob::prepared_shaders::info& i, const std::string& s)
{
	auto search = names_to_shaders.find(s);
	if (search != names_to_shaders.end())
	{
		shaders.set(search->second, i);
	}
	else
	{
		names_to_shaders[s] = shaders.add(i);
	}
}


noob::shaders_holder::handle noob::stage::get_shader(const std::string& s)
{
	noob::shaders_holder::handle temp;
	if (names_to_shaders.find(s) != names_to_shaders.end())
	{
		return names_to_shaders.at(s);
	}
	return default_triplanar_shader;
}


unsigned int noob::stage::_get_shader(const std::string& s)
{
	return get_shader(s).get_inner();
}


noob::basic_mesh noob::stage::make_mesh(const noob::shapes_holder::handle h)
{
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			{
				// logger::log("[Stage] making sphere mesh");
				return noob::mesh_utils::sphere(static_cast<btSphereShape*>(s->inner_shape)->getRadius());
			}
		case(noob::shape::type::BOX):
			{
				// logger::log("[Stage] making box mesh");
				noob::vec3 half_extents = static_cast<btBoxShape*>(s->inner_shape)->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::box(half_extents.v[0]*2, half_extents.v[1]*2, half_extents.v[2]*2);
			}
		// case(noob::shape::type::CAPSULE):
		//	{
				// logger::log("[Stage] making capsule mesh");
		//		btCapsuleShape* temp = static_cast<btCapsuleShape*>(s->inner_shape);
		//		return noob::mesh_utils::capsule(temp->getRadius(), temp->getHalfHeight()*2);
		//	}
		case(noob::shape::type::CYLINDER):
			{
				// logger::log("[Stage] making cylinder mesh");
				btCylinderShape* temp = static_cast<btCylinderShape*>(s->inner_shape);// return std::make_tuple(unit_cylinder_model, s->scales);
				noob::vec3 half_extents = temp->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::cylinder(temp->getRadius(), half_extents.v[1]*2);
			}
		case(noob::shape::type::CONE):
			{
				// logger::log("[Stage] making cone mesh");
				btConeShape* temp = static_cast<btConeShape*>(s->inner_shape);
				return noob::mesh_utils::cone(temp->getRadius(), temp->getHeight());
			}
		case(noob::shape::type::HULL):
			{
				// logger::log("[Stage] making convex mesh");
				return noob::mesh_utils::copy(*meshes.get(shapes_to_meshes[h.get_inner()]));
			}
		case(noob::shape::type::TRIMESH):
			{
				// logger::log("[Stage] making trimesh");
				return noob::mesh_utils::copy(*meshes.get(shapes_to_meshes[h.get_inner()]));
			}
		default:
			logger::log("[Stage] - USER DATA WARNING - INVALID SHAPE TO MESH :(");
			break;
	};
	return noob::mesh_utils::copy(*meshes.get(unit_sphere_mesh));
}


noob::basic_mesh noob::stage::_make_mesh(unsigned int h)
{
	return make_mesh(shapes.make_handle(h));
}



std::tuple<noob::basic_models_holder::handle, noob::vec3> noob::stage::get_model(const noob::shapes_holder::handle h)
{
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			// logger::log("[Stage] sphere shape to model");
			return std::make_tuple(unit_sphere_model, s->scales);
		case(noob::shape::type::BOX):
			// logger::log("[Stage] box shape to model");
			return std::make_tuple(unit_cube_model, s->scales);
		//case(noob::shape::type::CAPSULE):
			// logger::log("[Stage] capsule shape to model");
		//	return std::make_tuple(unit_capsule_model, s->scales);
		case(noob::shape::type::CYLINDER):
			// logger::log("[Stage] cylinder shape to model");
			return std::make_tuple(unit_cylinder_model, s->scales);
		case(noob::shape::type::CONE):
			// logger::log("[Stage] cone shape to model");
			return std::make_tuple(unit_cone_model, s->scales);
		case(noob::shape::type::HULL):
		{
			auto mesh = make_mesh(h);
			auto mesh_h = add_mesh(mesh);
			auto model_h = basic_model(mesh_h);
			// logger::log("[Stage] convex hull to model");
			return std::make_tuple(model_h, noob::vec3(1.0, 1.0, 1.0));
		}
		case(noob::shape::type::TRIMESH):
		{
			auto mesh = make_mesh(h);
			auto mesh_h = add_mesh(mesh);
			auto model_h = basic_model(mesh_h);
			// logger::log("[Stage] convex hull to model");
			return std::make_tuple(model_h, noob::vec3(1.0, 1.0, 1.0));
		}
		default:
			logger::log("[Stage] - USER DATA WARNING - INVALID SHAPE TO MODEL :(");
			break;
	};
	return std::make_tuple(unit_sphere_model, noob::vec3(1.0, 1.0, 1.0));
}


std::tuple<unsigned int, noob::vec3> noob::stage::_get_model(unsigned int h)
{
	auto a = get_model(shapes.make_handle(h));
	return std::make_tuple(std::get<0>(a).get_inner(), std::get<1>(a));//_model(shapes.make_handle(h));
}


es::entity noob::stage::actor(float radius, float height, noob::animated_models_holder::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	// pool.set(temp, body_tag.inner, _bod);
	pool.set(temp, animated_model_tag.inner, _model);
	pool.set(temp, shader_tag.inner, get_shader(shading));
	
	return temp;
}


es::entity noob::stage::prop(const noob::bodies_holder::handle _bod, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, _bod);
	auto bs = bodies_to_shapes[_bod.get_inner()];
	pool.set(temp, basic_model_tag.inner, std::get<0>(get_model(bs)));
	pool.set(temp, scales_tag.inner, shapes.get(bodies_to_shapes[_bod.get_inner()])->get_scales());
	pool.set(temp, shader_tag.inner, get_shader(shading));
	return temp;
}



es::entity noob::stage::prop(const noob::bodies_holder::handle _bod, noob::basic_models_holder::handle _model, const std::string& shading)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, bodies.get(_bod));
	pool.set(temp, basic_model_tag.inner, _model);
	pool.set(temp, scales_tag.inner, noob::vec3(1.0, 1.0, 1.0));
	pool.set(temp, shader_tag.inner, get_shader(shading));
	return temp;
}


es::entity noob::stage::scenery(const noob::meshes_holder::handle h, const noob::vec3& pos, const std::string& shading, const noob::versor& orient)
{
	noob::shapes_holder::handle s_handle = static_trimesh(h);
	// shapes.get(s_handle)->set_margin(1.0);
	noob::bodies_holder::handle b_handle = body(noob::body_type::STATIC, s_handle, 0.0, pos, orient);
	noob::basic_models_holder::handle model_handle = std::get<0>(get_model(s_handle));
	
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, b_handle);
	pool.set(temp, basic_model_tag.inner, model_handle);
	pool.set(temp, scales_tag.inner, std::get<1>(get_model(s_handle)));//bodies.get(b_handle)->inner_shape->get_scales());
	pool.set(temp, shader_tag.inner, get_shader(shading));
	return temp;
}
