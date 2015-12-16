#include "Stage.hpp"


bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	renderer.init();

	auto temp_mesh (pool.register_component<noob::meshes::handle>("mesh"));
	mesh_tag.inner = temp_mesh;

	auto temp_path (pool.register_component<std::vector<noob::vec3>>("path"));
	path_tag.inner = temp_path;

	auto temp_shape (pool.register_component<noob::shapes::handle>("shape"));
	shape_tag.inner = temp_shape;

	auto temp_shape_type (pool.register_component<noob::shape::type>("shape-type"));
	shape_type_tag.inner = temp_shape_type;

	auto temp_body (pool.register_component<noob::body_controller>("movement-controller"));
	body_tag.inner = temp_body;

	auto temp_basic_model (pool.register_component<noob::basic_models::handle>("basic-model"));
	basic_model_tag.inner = temp_basic_model;

	auto temp_animated_model (pool.register_component<noob::animated_models::handle>("animated-model"));
	animated_model_tag.inner = temp_animated_model;

	auto temp_skeletal_anim (pool.register_component<noob::skeletal_anims::handle>("skeletal-anim"));
	skeletal_anim_tag.inner = temp_skeletal_anim;

	auto temp_basic_shader (pool.register_component<noob::basic_renderer::uniform_info>("basic-renderer-uniform"));
	basic_shader_tag.inner = temp_basic_shader;

	auto temp_triplanar_shader (pool.register_component<noob::triplanar_gradient_map_renderer::uniform_info>("triplanar-renderer-uniform"));
	triplanar_shader_tag.inner = temp_triplanar_shader;	


	unit_sphere_shape = sphere(0.5);
	unit_cube_shape = box(1.0, 1.0, 1.0);
	unit_capsule_shape = capsule(0.5, 1.0);
	unit_cylinder_shape = cylinder(0.5, 1.0);
	unit_cone_shape = cone(0.5, 1.0);

	unit_sphere_mesh = meshes_holder.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_sphere_shape)));
	unit_cube_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cube_shape)));
	unit_capsule_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_capsule_shape)));
	unit_cylinder_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cylinder_shape)));
	unit_cone_mesh =  meshes_holder.add(std::make_unique<noob::basic_mesh>(make_mesh(unit_cone_shape)));

	unit_sphere_model = basic_model(unit_sphere_mesh);
	unit_cube_model = basic_model(unit_cube_mesh);
	unit_capsule_model = basic_model(unit_capsule_mesh);
	unit_cylinder_model = basic_model(unit_cylinder_mesh);
	unit_cone_model = basic_model(unit_cone_mesh);

	shapes_to_meshes[unit_sphere_shape.get_inner()] = unit_sphere_mesh;
	shapes_to_meshes[unit_cube_shape.get_inner()] = unit_cube_mesh;
	shapes_to_meshes[unit_capsule_shape.get_inner()] = unit_capsule_mesh;
	shapes_to_meshes[unit_cylinder_shape.get_inner()] = unit_cylinder_mesh;
	shapes_to_meshes[unit_cone_shape.get_inner()] = unit_cone_mesh;
	
	meshes_to_models[unit_sphere_mesh.get_inner()] = unit_sphere_model;
	meshes_to_models[unit_cube_mesh.get_inner()] = unit_cube_model;
	meshes_to_models[unit_capsule_mesh.get_inner()] = unit_capsule_model;
	meshes_to_models[unit_cylinder_mesh.get_inner()] = unit_cylinder_model;
	meshes_to_models[unit_cone_mesh.get_inner()] = unit_cone_model;



	// noob::animated_models animated_models_holder;
	// noob::bodies bodies_holder;
	// noob::skeletal_anims skeletal_anims_holder;
	// noob::lights lights_holder;
	// noob::reflections reflections_holder;

	//  Init basic default shader
	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 1.0, 1.0, 1.0);
	set_shader(basic_shader_info, "debug"); 

	logger::log("[Stage] init complete.");
	return true;
}


void noob::stage::tear_down()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}


void noob::stage::update(double dt)
{
	dynamics_world->stepSimulation(1.0/60.0, 10);
}


void noob::stage::draw() const
{
	// TODO: Use culling to determine which items are visible, and then draw them.
}


noob::bodies::handle noob::stage::body(noob::shapes::handle h, float mass, const noob::vec3& pos, const noob::versor& orient)
{
	std::unique_ptr<noob::body_controller> b = std::make_unique<noob::body_controller>();
	b->init(dynamics_world, shapes_holder.get(h), mass, pos, orient);
	return bodies_holder.add(std::move(b));
}


noob::shapes::handle noob::stage::sphere(float r)
{
	auto search = sphere_shapes.find(r);
	if (search == sphere_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r);
		auto results = sphere_shapes.insert(std::make_pair(r, shapes_holder.add(std::move(temp))));
		return (results.first)->second;

	}
	return sphere_shapes[r];
}


noob::shapes::handle noob::stage::box(float x, float y, float z)
{
	auto search = box_shapes.find(std::make_tuple(x,y,z));
	if (search == box_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);
		auto results = box_shapes.insert(std::make_pair(std::make_tuple(x,y,z), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return box_shapes[std::make_tuple(x,y,z)];
}


noob::shapes::handle noob::stage::cylinder(float r, float h)
{
	auto search = cylinder_shapes.find(std::make_tuple(r, h));
	if (search == cylinder_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);
		auto results = cylinder_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return cylinder_shapes[std::make_tuple(r, h)];
}


noob::shapes::handle noob::stage::cone(float r, float h)
{
	auto search = cone_shapes.find(std::make_tuple(r, h));
	if (search == cone_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);
		auto results = cone_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return cone_shapes[std::make_tuple(r, h)];
}


noob::shapes::handle noob::stage::capsule(float r, float h)
{
	auto search = capsule_shapes.find(std::make_tuple(r, h));
	if (search == capsule_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->capsule(r, h);
		auto results = capsule_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return capsule_shapes[std::make_tuple(r, h)];
}

/*
   noob::shapes::handle noob::stage::plane(const noob::vec3& normal, float offset)
   {
   auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
   if (search == planes.end())
   {
   std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
   temp->plane(normal, offset);
   auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes_holder.add(std::move(temp))));
   return (results.first)->second;
   }
   return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
   }
   */

noob::shapes::handle noob::stage::hull(const std::vector<vec3> & _points)
{
	// TODO: Add to shapes_to_meshes
	std::unique_ptr<noob::shape> temp_shape = std::make_unique<noob::shape>();
	temp_shape->hull(_points);
	noob::shapes::handle temp_shape_handle = shapes_holder.add(std::move(temp_shape));

	std::unique_ptr<noob::basic_mesh> temp_mesh = std::make_unique<noob::basic_mesh>();

	*temp_mesh = noob::mesh_utils::hull(_points);
	// TODO: Hash temp_mesh to save memory
	noob::meshes::handle temp_mesh_handle = meshes_holder.add(std::move(temp_mesh));
	shapes_to_meshes[temp_shape_handle.get_inner()] = temp_mesh_handle;

	//meshes_to_models[temp_mesh_handle.get_inner()] = 

	return temp_shape_handle;
}


noob::shapes::handle noob::stage::static_trimesh(const noob::meshes::handle _mesh)
{
	auto search = meshes_to_shapes.find(_mesh.get_inner());
	if (search == meshes_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp_shape_ptr = std::make_unique<noob::shape>();
		temp_shape_ptr->trimesh(meshes_holder.get(_mesh));
		noob::shapes::handle temp_shape = shapes_holder.add(std::move(temp_shape_ptr));
		meshes_to_shapes.insert(std::make_pair(_mesh.get_inner(), temp_shape));
		shapes_to_meshes.insert(std::make_pair(temp_shape.get_inner(), _mesh));
		return temp_shape;
	}
	return search->second;
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
		std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
		temp->init(*meshes_holder.get(_mesh));
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
	auto search = light_names.find(s);
	if (search != light_names.end())
	{
		lights_holder.set(search->second, l);
	}
	else
	{
		light_names[s] = lights_holder.add(l);
	}
}


noob::light noob::stage::get_light(const std::string& s)
{
	noob::lights::handle temp;
	if (light_names.find(s) != light_names.end())
	{
		temp = light_names[s];
	}
	return lights_holder.get(temp);
}


void noob::stage::set_reflection(const noob::reflection& r, const std::string& s)
{
	auto search = reflection_names.find(s);
	if (search != reflection_names.end())
	{
		reflections_holder.set(search->second, r);
	}
	else
	{
		reflection_names[s] = reflections_holder.add(r);
	}
}


noob::reflection noob::stage::get_reflection(const std::string& s)
{
	noob::reflections::handle temp;
	if (reflection_names.find(s) != reflection_names.end())
	{
		temp = reflection_names[s];
	}
	return reflections_holder.get(temp);
}


void noob::stage::set_shader(const noob::prepared_shaders::info& i, const std::string& s)
{
	auto search = shader_names.find(s);
	if (search != shader_names.end())
	{
		shaders_holder.set(search->second, i);
	}
	else
	{
		shader_names[s] = shaders_holder.add(i);
	}
}


noob::prepared_shaders::info noob::stage::get_shader(const std::string& s)
{
	noob::shaders::handle temp;
	if (shader_names.find(s) != shader_names.end())
	{
		temp = shader_names[s];
	}
	return shaders_holder.get(temp);
}


es::entity noob::stage::actor(const noob::bodies::handle _bod, noob::animated_models::handle _model)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, _bod);
	pool.set(temp, animated_model_tag.inner, _model);
	return temp;
}


es::entity noob::stage::prop(const noob::bodies::handle _bod, noob::basic_models::handle _model)
{
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, bodies_holder.get(_bod));
	pool.set(temp, basic_model_tag.inner, basic_models_holder.get(_model));

	return temp;
}


es::entity noob::stage::scenery(const noob::meshes::handle h, const noob::vec3& pos, const noob::versor& orient)
{
	noob::shapes::handle s_handle = static_trimesh(h);
	noob::bodies::handle b_handle = body(s_handle, 0.0, pos, orient);
	noob::basic_models::handle model_handle = std::get<0>(get_model(s_handle));
	es::entity temp (pool.new_entity());
	pool.set(temp, body_tag.inner, bodies_holder.get(b_handle));
	pool.set(temp, basic_model_tag.inner, basic_models_holder.get(model_handle));

	return temp;
}


noob::basic_mesh noob::stage::make_mesh(const noob::shapes::handle h)
{
	noob::shape* s = shapes_holder.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			{
				return noob::mesh_utils::sphere(static_cast<btSphereShape*>(s->inner_shape)->getRadius());
				break;
			}
		case(noob::shape::type::BOX):
			{
				noob::vec3 half_extents = static_cast<btBoxShape*>(s->inner_shape)->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::box(half_extents.v[0]*2, half_extents.v[1]*2, half_extents.v[2]*2);
				break;
			}
		case(noob::shape::type::CAPSULE):
			{
				btCapsuleShape* temp = static_cast<btCapsuleShape*>(s->inner_shape);
				return noob::mesh_utils::capsule(temp->getRadius(), temp->getHalfHeight()*2);
				break;
			}
		case(noob::shape::type::CYLINDER):
			{
				btCylinderShape* temp = static_cast<btCylinderShape*>(s->inner_shape);// return std::make_tuple(unit_cylinder_model, s->scales);
				noob::vec3 half_extents = temp->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::cylinder(temp->getRadius(), half_extents.v[1]*2);
				break;
			}
		case(noob::shape::type::CONE):
			{
				btConeShape* temp = static_cast<btConeShape*>(s->inner_shape);
				return noob::mesh_utils::cone(temp->getRadius(), temp->getHeight());
				break;
			}
		case(noob::shape::type::CONVEX):
			{
				return noob::mesh_utils::copy(*meshes_holder.get(shapes_to_meshes[h.get_inner()]));
				break;
			}
		case(noob::shape::type::TRIMESH):
			{
				return noob::mesh_utils::copy(*meshes_holder.get(shapes_to_meshes[h.get_inner()]));
				break;
			}
		default:
			logger::log("[Stage] - USER DATA WARNING - INVALID SHAPE TO MESH :(");
			break;
	};
	return noob::mesh_utils::copy(*meshes_holder.get(unit_sphere_mesh));
}


std::tuple<noob::basic_models::handle, noob::vec3> noob::stage::get_model(const noob::shapes::handle h)
{
	noob::shape* s = shapes_holder.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			return std::make_tuple(unit_sphere_model, s->scales);
			logger::log("[Stage] sphere shape to model");
			break;
		case(noob::shape::type::BOX):
			return std::make_tuple(unit_cube_model, s->scales);
			logger::log("[Stage] box shape to model");
			break;
		case(noob::shape::type::CAPSULE):
			return std::make_tuple(unit_capsule_model, s->scales);
			logger::log("[Stage] capsule shape to model");
			break;
		case(noob::shape::type::CYLINDER):
			return std::make_tuple(unit_cylinder_model, s->scales);
			logger::log("[Stage] cylinder shape to model");
			break;
		case(noob::shape::type::CONE):
			return std::make_tuple(unit_cone_model, s->scales);
			logger::log("[Stage] cone shape to model");
			break;
		case(noob::shape::type::CONVEX):
			return std::make_tuple(meshes_to_models[shapes_to_meshes[h.get_inner()].get_inner()], noob::vec3(1.0, 1.0, 1.0));
			logger::log("[Stage] convex shape to model");
			break;
		case(noob::shape::type::TRIMESH):
			return std::make_tuple(meshes_to_models[shapes_to_meshes[h.get_inner()].get_inner()], noob::vec3(1.0, 1.0, 1.0));
			logger::log("[Stage] trimesh shape to model");
			break;
		default:
			logger::log("[Stage] - USER DATA WARNING - INVALID SHAPE TO MODEL :(");
			break;
	};
	return std::make_tuple(unit_sphere_model, noob::vec3(1.0, 1.0, 1.0));
}
