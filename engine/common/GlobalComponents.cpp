#include "GlobalComponents.hpp"

noob::shaders_holder::handle noob::globals::debug_shader;
noob::shaders_holder::handle noob::globals::default_triplanar_shader;
noob::shaders_holder::handle noob::globals::uv_shader;

noob::shapes_holder::handle noob::globals::unit_sphere_shape;
noob::shapes_holder::handle noob::globals::unit_cube_shape;
noob::shapes_holder::handle noob::globals::unit_capsule_shape;
noob::shapes_holder::handle noob::globals::unit_cylinder_shape;
noob::shapes_holder::handle noob::globals::unit_cone_shape;

noob::meshes_holder::handle noob::globals::unit_sphere_mesh;
noob::meshes_holder::handle noob::globals::unit_cube_mesh;
noob::meshes_holder::handle noob::globals::unit_capsule_mesh;
noob::meshes_holder::handle noob::globals::unit_cylinder_mesh;
noob::meshes_holder::handle noob::globals::unit_cone_mesh;

noob::basic_models_holder::handle noob::globals::unit_sphere_model;
noob::basic_models_holder::handle noob::globals::unit_cube_model;
noob::basic_models_holder::handle noob::globals::unit_capsule_model;
noob::basic_models_holder::handle noob::globals::unit_cylinder_model;
noob::basic_models_holder::handle noob::globals::unit_cone_model;

noob::prepared_shaders noob::globals::renderer;

noob::meshes_holder noob::globals::meshes;
noob::basic_models_holder noob::globals::basic_models;
noob::animated_models_holder noob::globals::animated_models;
noob::shapes_holder noob::globals::shapes;
noob::skeletal_anims_holder noob::globals::skeletal_anims;
noob::lights_holder noob::globals::lights;
noob::reflections_holder noob::globals::reflections;
noob::shaders_holder noob::globals::shaders;

// TODO: Test other data structures.
// Benchmark, benchmark, benchmark!!! Multiplatform, too!!!
std::map<float, noob::shapes_holder::handle>  noob::globals::sphere_shapes;
std::map<std::tuple<float, float, float>, noob::shapes_holder::handle>  noob::globals::box_shapes;
std::map<std::tuple<float, float>, noob::shapes_holder::handle>  noob::globals::cylinder_shapes;
std::map<std::tuple<float, float>, noob::shapes_holder::handle>  noob::globals::cone_shapes;
std::map<std::tuple<float, float>, noob::shapes_holder::handle>  noob::globals::capsule_shapes;
// std::map<std::tuple<float,float,float,float>, shapes_holder::handle> planes;
std::map<std::vector<std::array<float, 3>>, noob::shapes_holder::handle>  noob::globals::hull_shapes;
std::map<std::vector<std::array<float, 3>>, noob::shapes_holder::handle>  noob::globals::trimesh_shapes;

std::unordered_map<size_t, noob::meshes_holder::handle> noob::globals::shapes_to_meshes;
std::unordered_map<size_t, noob::basic_models_holder::handle> noob::globals::meshes_to_models;
std::unordered_map<size_t, noob::shapes_holder::handle> noob::globals::meshes_to_shapes;

std::unordered_map<std::string, noob::shaders_holder::handle> noob::globals::names_to_shaders;
std::unordered_map<std::string, noob::lights_holder::handle> noob::globals::names_to_lights;
std::unordered_map<std::string, noob::reflections_holder::handle> noob::globals::names_to_reflections;


void noob::globals::init()
{
	renderer.init();
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

	logger::log("[Globals] Making unit sphere model");
	unit_sphere_model = basic_model(unit_sphere_mesh);
	logger::log("[Globals] Making unit cube model");
	unit_cube_model = basic_model(unit_cube_mesh);
	logger::log("[Globals] Making unit cylinder model");
	unit_cylinder_model = basic_model(unit_cylinder_mesh);
	logger::log("[Globals] Making unit cone model");
	unit_cone_model = basic_model(unit_cone_mesh);

	meshes_to_models[unit_sphere_mesh.get_inner()] = unit_sphere_model;
	meshes_to_models[unit_cube_mesh.get_inner()] = unit_cube_model;
	meshes_to_models[unit_cylinder_mesh.get_inner()] = unit_cylinder_model;
	meshes_to_models[unit_cone_mesh.get_inner()] = unit_cone_model;

	//  Init basic default shader
	noob::basic_renderer::uniform basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	set_shader(basic_shader_info, "debug"); 
	debug_shader = get_shader("debug");

	// Init triplanar shader. For fun.
	noob::triplanar_gradient_map_renderer::uniform triplanar_info;
	std::array<noob::vec4,4> colours;
	triplanar_info.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[1] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[2] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[3] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.blend = noob::vec4(1.0, 1.0, 1.0, 0.0);
	triplanar_info.scales = noob::vec4(1.0, 1.0, 1.0, 0.0);
	triplanar_info.colour_positions = noob::vec4(0.3, 0.6, 0.0, 0.0);
	set_shader(triplanar_info, "default-triplanar");
	default_triplanar_shader = get_shader("default-triplanar");
}


noob::basic_mesh noob::globals::make_mesh(const noob::shapes_holder::handle h)
{
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			{
				// logger::log("[Globals] making sphere mesh");
				return noob::mesh_utils::sphere(static_cast<btSphereShape*>(s->inner_shape)->getRadius());
			}
		case(noob::shape::type::BOX):
			{
				// logger::log("[Globals] making box mesh");
				noob::vec3 half_extents = static_cast<btBoxShape*>(s->inner_shape)->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::box(half_extents.v[0]*2, half_extents.v[1]*2, half_extents.v[2]*2);
			}
			// case(noob::shape::type::CAPSULE):
			//	{
			// logger::log("[Globals] making capsule mesh");
			//		btCapsuleShape* temp = static_cast<btCapsuleShape*>(s->inner_shape);
			//		return noob::mesh_utils::capsule(temp->getRadius(), temp->getHalfHeight()*2);
			//	}
		case(noob::shape::type::CYLINDER):
			{
				// logger::log("[Globals] making cylinder mesh");
				btCylinderShape* temp = static_cast<btCylinderShape*>(s->inner_shape);// return std::make_tuple(unit_cylinder_model, s->scales);
				noob::vec3 half_extents = temp->getHalfExtentsWithoutMargin();
				return noob::mesh_utils::cylinder(temp->getRadius(), half_extents.v[1]*2);
			}
		case(noob::shape::type::CONE):
			{
				// logger::log("[Globals] making cone mesh");
				btConeShape* temp = static_cast<btConeShape*>(s->inner_shape);
				return noob::mesh_utils::cone(temp->getRadius(), temp->getHeight());
			}
		case(noob::shape::type::HULL):
			{
				// logger::log("[Globals] making convex mesh");
				return *meshes.get(shapes_to_meshes[h.get_inner()]);
			}
		case(noob::shape::type::TRIMESH):
			{
				// logger::log("[Globals] making trimesh");
				return *meshes.get(shapes_to_meshes[h.get_inner()]);
			}
		default:
			logger::log("[Globals] - USER DATA WARNING - INVALID SHAPE TO MESH :(");
			break;
	};
	return *meshes.get(unit_sphere_mesh);
}


std::tuple<noob::basic_models_holder::handle, noob::vec3> noob::globals::get_model(const noob::shapes_holder::handle h)
{
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			// logger::log("[Globals] sphere shape to model");
			return std::make_tuple(unit_sphere_model, s->scales);
		case(noob::shape::type::BOX):
			// logger::log("[Globals] box shape to model");
			return std::make_tuple(unit_cube_model, s->scales);
			//case(noob::shape::type::CAPSULE):
			// logger::log("[Globals] capsule shape to model");
			//	return std::make_tuple(unit_capsule_model, s->scales);
		case(noob::shape::type::CYLINDER):
			// logger::log("[Globals] cylinder shape to model");
			return std::make_tuple(unit_cylinder_model, s->scales);
		case(noob::shape::type::CONE):
			// logger::log("[Globals] cone shape to model");
			return std::make_tuple(unit_cone_model, s->scales);
		case(noob::shape::type::HULL):
			{
				auto mesh = make_mesh(h);
				auto mesh_h = add_mesh(mesh);
				auto model_h = basic_model(mesh_h);
				// logger::log("[Globals] convex hull to model");
				return std::make_tuple(model_h, noob::vec3(1.0, 1.0, 1.0));
			}
		case(noob::shape::type::TRIMESH):
			{
				auto mesh = make_mesh(h);
				auto mesh_h = add_mesh(mesh);
				auto model_h = basic_model(mesh_h);
				// logger::log("[Globals] convex hull to model");
				return std::make_tuple(model_h, noob::vec3(1.0, 1.0, 1.0));
			}
		default:
			logger::log("[Globals] - USER DATA WARNING - INVALID SHAPE TO MODEL :(");
			break;
	};
	return std::make_tuple(unit_sphere_model, noob::vec3(1.0, 1.0, 1.0));
}


noob::shapes_holder::handle noob::globals::sphere(float r)
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


noob::shapes_holder::handle noob::globals::box(float x, float y, float z)
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


noob::shapes_holder::handle noob::globals::cylinder(float r, float h)
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


noob::shapes_holder::handle noob::globals::cone(float r, float h)
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


/*
   noob::shapes_holder::handle noob::globals::capsule(float r, float h)
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
   noob::shapes_holder::handle noob::globals::plane(const noob::vec3& normal, float offset)
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

noob::shapes_holder::handle noob::globals::hull(const std::vector<vec3> & _points)
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


noob::shapes_holder::handle noob::globals::static_trimesh(const noob::meshes_holder::handle _mesh)
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


noob::meshes_holder::handle noob::globals::add_mesh(const noob::basic_mesh& m)
{
	std::unique_ptr<noob::basic_mesh> mesh_ptr = std::make_unique<noob::basic_mesh>();
	*mesh_ptr = m;
	return meshes.add(std::move(mesh_ptr));	
}


noob::basic_models_holder::handle noob::globals::basic_model(const noob::meshes_holder::handle _mesh)
{
	auto search = meshes_to_models.find(_mesh.get_inner());
	if (search == meshes_to_models.end())
	{		
		std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
		temp->init(*meshes.get(_mesh));
		noob::basic_models_holder::handle h = basic_models.add(std::move(temp));
		meshes_to_models.insert(std::make_pair(_mesh.get_inner(), h));
		return h;
	}
	return search->second;
}


noob::animated_models_holder::handle noob::globals::animated_model(const std::string& filename)
{
	std::unique_ptr<noob::animated_model> temp = std::make_unique<noob::animated_model>();
	temp->init(filename);
	return animated_models.add(std::move(temp));
}


noob::skeletal_anims_holder::handle noob::globals::skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletal_anims.add(std::move(temp));
}


void noob::globals::set_light(const noob::light& l, const std::string& s)
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


noob::light noob::globals::get_light(const std::string& s)
{
	noob::lights_holder::handle temp;
	if (names_to_lights.find(s) != names_to_lights.end())
	{
		temp = names_to_lights[s];
	}
	return lights.get(temp);
}


void noob::globals::set_reflection(const noob::reflection& r, const std::string& s)
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


noob::reflection noob::globals::get_reflection(const std::string& s)
{
	noob::reflections_holder::handle temp;
	if (names_to_reflections.find(s) != names_to_reflections.end())
	{
		temp = names_to_reflections[s];
	}
	return reflections.get(temp);
}


void noob::globals::set_shader(const noob::basic_renderer::uniform& u, const std::string& name)
{
	set_shader(noob::prepared_shaders::uniform(u), name);
}


void noob::globals::set_shader(const noob::triplanar_gradient_map_renderer::uniform& u, const std::string& name)
{
	set_shader(noob::prepared_shaders::uniform(u), name);
}


void noob::globals::set_shader(const noob::prepared_shaders::uniform& u, const std::string& s)
{
	auto search = names_to_shaders.find(s);
	if (search != names_to_shaders.end())
	{
		shaders.set(search->second, u);
	}
	else
	{
		names_to_shaders[s] = shaders.add(u);
	}
}


noob::shaders_holder::handle noob::globals::get_shader(const std::string& s)
{
	noob::shaders_holder::handle temp;
	if (names_to_shaders.find(s) != names_to_shaders.end())
	{
		return names_to_shaders.at(s);
	}
	return default_triplanar_shader;
}
