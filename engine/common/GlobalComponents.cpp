#include "GlobalComponents.hpp"

noob::shaders_holder::handle noob::globals::debug_shader;
noob::shaders_holder::handle noob::globals::default_triplanar_shader;
noob::shaders_holder::handle noob::globals::uv_shader;

noob::shapes_holder::handle noob::globals::unit_sphere_shape;
noob::shapes_holder::handle noob::globals::unit_cube_shape;
noob::shapes_holder::handle noob::globals::unit_capsule_shape;
noob::shapes_holder::handle noob::globals::unit_cylinder_shape;
noob::shapes_holder::handle noob::globals::unit_cone_shape;

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

std::unordered_map<std::string, noob::shapes_holder::handle> noob::globals::names_to_shapes;
std::unordered_map<std::string, noob::basic_models_holder::handle> noob::globals::names_to_basic_models;
std::unordered_map<std::string, noob::shaders_holder::handle> noob::globals::names_to_shaders;
std::unordered_map<std::string, noob::lights_holder::handle> noob::globals::names_to_lights;
std::unordered_map<std::string, noob::reflections_holder::handle> noob::globals::names_to_reflections;

std::map<size_t, noob::basic_models_holder::handle> noob::globals::shapes_to_models;

void noob::globals::init()
{
	renderer.init();
	unit_sphere_shape = sphere(0.5);
	unit_cube_shape = box(1.0, 1.0, 1.0);
	unit_cylinder_shape = cylinder(0.5, 1.0);
	unit_cone_shape = cone(0.5, 1.0);

	// logger::log("[Globals] Making unit sphere model");
	unit_sphere_model = basic_model(noob::mesh_utils::sphere(0.5));//basic_models.add(std::move(temp));
	// logger::log("[Globals] Making unit cube model");
	unit_cube_model = basic_model(noob::mesh_utils::box(1.0, 1.0, 1.0));
	// unit_cube_model = basic_model(noob::mesh_utils::box(1.0, 1.0, 1.0));
	// logger::log("[Globals] Making unit cylinder model");
	// unit_cylinder_model = basic_model(noob::mesh_utils::cylinder(1.0, 0.5));
	unit_cylinder_model = basic_model(noob::mesh_utils::cylinder(0.5, 1.0));
	// logger::log("[Globals] Making unit cone model");
	unit_cone_model = basic_model(noob::mesh_utils::cone(0.5, 1.0));

	//  Init basic default shader
	noob::basic_renderer::uniform dbg;
	dbg.colour = noob::vec4(1.0, 1.0, 1.0, 0.0);
	set_shader(dbg, "debug"); 
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

/*
noob::basic_mesh noob::globals::mesh_from_shape(const noob::shapes_holder::handle h)
{
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			{
				// logger::log("[Globals] making sphere mesh");
				// return noob::mesh_utils::sphere(static_cast<btSphereShape*>(s->inner_shape)->getRadius());
			}
		case(noob::shape::type::BOX):
			{
				// logger::log("[Globals] making box mesh");
				noob::vec3 half_extents = static_cast<btBoxShape*>(s->inner_shape)->getHalfExtentsWithoutMargin();
				// return noob::mesh_utils::box(half_extents.v[0]*2, half_extents.v[1]*2, half_extents.v[2]*2);
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
				// return noob::mesh_utils::cylinder(temp->getRadius(), half_extents.v[1]*2);
			}
		case(noob::shape::type::CONE):
			{
				// logger::log("[Globals] making cone mesh");
				btConeShape* temp = static_cast<btConeShape*>(s->inner_shape);
				// return noob::mesh_utils::cone(temp->getRadius(), temp->getHeight());
			}
		case(noob::shape::type::HULL):
			{
				// logger::log("[Globals] making convex mesh");
				// return *meshes.get(shapes_to_meshes[h.get_inner()]);
			}
		case(noob::shape::type::TRIMESH):
			{
				// logger::log("[Globals] making trimesh");
				// return *meshes.get(shapes_to_meshes[h.get_inner()]);
			}
		default:
			logger::log("[Globals] - USER DATA WARNING - INVALID SHAPE TO MESH :(");
			break;
	};
	// return  *meshes.get(unit_sphere_mesh);
}
*/

noob::basic_models_holder::handle noob::globals::basic_model(const noob::basic_mesh& m)
{
	std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
	temp->init(m);
	return basic_models.add(std::move(temp));
}


noob::globals::scaled_model noob::globals::model_by_shape(const noob::shapes_holder::handle h)
{
	noob::globals::scaled_model results;
	noob::shape* s = shapes.get(h);
	switch(s->shape_type)
	{
		case(noob::shape::type::SPHERE):
			results.model_h = unit_sphere_model;
			results.scales = s->scales;
			return results;
		case(noob::shape::type::BOX):
			results.model_h = unit_cube_model;
			results.scales = s->scales;
		 	return results;
		case(noob::shape::type::CYLINDER):
			results.model_h = unit_cylinder_model;
			results.scales = s->scales;
			return results;
		case(noob::shape::type::CONE):
			results.model_h = unit_cone_model;
			results.scales = s->scales;
			return results;
		case(noob::shape::type::HULL):
			{
				auto search = shapes_to_models.find(h.get_inner());
				if (search == shapes_to_models.end())
				{
					results.model_h = basic_models.make_handle(search->first);
					return results;
				}
			}
		case(noob::shape::type::TRIMESH):
			{
				auto search = shapes_to_models.find(h.get_inner());
				if (search == shapes_to_models.end())
				{
					results.model_h = basic_models.make_handle(search->first);
					return results;
				}
			}
		default:
			logger::log("[Globals] - USER DATA WARNING - INVALID SHAPE TO MODEL :(");
			break;
	};
	return results; 
}


noob::shapes_holder::handle noob::globals::sphere(float r)
{
	fmt::MemoryWriter w;
	w << "sphere-" << static_cast<uint32_t>(r);

	auto search = names_to_shapes.find(w.str());
	// auto search = sphere_shapes.find(r);
	if (search == names_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r);
		
		fmt::MemoryWriter ww;
		ww << "sphere-" << static_cast<uint32_t>(r);

		auto results = names_to_shapes.insert(std::make_pair(ww.str(), shapes.add(std::move(temp))));
		return (results.first)->second;

	}
	return search->second;
}


noob::shapes_holder::handle noob::globals::box(float x, float y, float z)
{
	fmt::MemoryWriter w;
	w << "box-" << static_cast<uint32_t>(x) << "-" << static_cast<uint32_t>(y)  << "-" << static_cast<uint32_t>(z);
	auto search = names_to_shapes.find(w.str());
	//auto search = box_shapes.find(std::make_tuple(x,y,z));
	if (search == names_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);

		fmt::MemoryWriter ww;
		ww << "box-" << static_cast<uint32_t>(x) << "-" << static_cast<uint32_t>(y) << "-" << static_cast<uint32_t>(z);

		auto results = names_to_shapes.insert(std::make_pair(ww.str(), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return search->second;
}


noob::shapes_holder::handle noob::globals::cylinder(float r, float h)
{
	// auto search = cylinder_shapes.find(std::make_tuple(r, h));
	fmt::MemoryWriter w;
	w << "cylinder-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);
	auto search = names_to_shapes.find(w.str());
	if (search == names_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);

		fmt::MemoryWriter ww;
		ww << "cylinder-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);

		auto results = names_to_shapes.insert(std::make_pair(ww.str(), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return search->second;
}


noob::shapes_holder::handle noob::globals::cone(float r, float h)
{
	fmt::MemoryWriter w;
	w << "cone-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);
	auto search = names_to_shapes.find(w.str());
	if (search == names_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);

		fmt::MemoryWriter ww;
		ww << "cone-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);

		auto results = names_to_shapes.insert(std::make_pair(ww.str(), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return search->second;
}


//   noob::shapes_holder::handle noob::globals::capsule(float r, float h)
//   {
//   auto search = capsule_shapes.find(std::make_tuple(r, h));
//   if (search == capsule_shapes.end())
//   {
//   std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
//   temp->capsule(r, h);
//   auto results = capsule_shapes.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
//   return (results.first)->second;
//   }
//   return capsule_shapes[std::make_tuple(r, h)];
//   }


//  noob::shapes_holder::handle noob::globals::plane(const noob::vec3& normal, float offset)
// {
// auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
// if (search == planes.end())
// {
// std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
// temp->plane(normal, offset);
// auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes.add(std::move(temp))));
// return (results.first)->second;
// }
// return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
// }


noob::shapes_holder::handle noob::globals::hull(const std::vector<vec3>& points, const std::string& name)
{
	auto search = names_to_shapes.find(name);
	if (search == names_to_shapes.end())
	{
		// TODO: Add to shapes_to_meshes
		std::unique_ptr<noob::shape> temp_shape = std::make_unique<noob::shape>();
		temp_shape->hull(points);
		return shapes.add(std::move(temp_shape));
	}
	return search->second;
}


noob::shapes_holder::handle noob::globals::static_trimesh(const noob::basic_mesh& m, const std::string& name)
{
	auto search = names_to_shapes.find(name);
	if (search == names_to_shapes.end())
	{
		std::unique_ptr<noob::shape> temp_shape_ptr = std::make_unique<noob::shape>();
		temp_shape_ptr->trimesh(&m);
		return shapes.add(std::move(temp_shape_ptr));
	}
	return search->second;
}

/*
scaled_model noob::globals::get_model_by_name(const std::string& name)
{
	auto search = names_to_models.find(name);
	if (search == names_to_models.end())
	{		
		// std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
		// temp->init(*meshes.get(_mesh));
		// return basic_models.add(std::move(temp));
		fmt::MemoryWriter ww;
		ww << "[Globals] Failed to get model " << name << ", returning defaults.";
		logger::log(ww.str());
		noob::basic_models_holder::handle h;
		return h;
	}
	// logger::log("[Globals] Getting model by name failed.");
	return search->second;
}
*/

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
		fmt::MemoryWriter ww;
		ww << "[Globals] set_shader - Found shader with name: " << s << ". Updating.";//Creating and setting to handle " << h.get_inner();
		logger::log(ww.str());
		shaders.set(search->second, u);
	}
	else
	{
		noob::shaders_holder::handle h = shaders.add(u);
		
		fmt::MemoryWriter ww;
		ww << "[Globals] set_shader - Could not find shader with name: " << s << ". Creating and setting to handle " << h.get_inner();
		logger::log(ww.str());
		names_to_shaders.insert(std::make_pair(s, h));
	}
}


noob::shaders_holder::handle noob::globals::get_shader(const std::string& s)
{
	noob::shaders_holder::handle temp;
	if (names_to_shaders.find(s) != names_to_shaders.end())
	{
		return names_to_shaders.at(s);
	}
	else
	{
		fmt::MemoryWriter ww;
		ww << "[Globals] Could not find shader " << s << ". Returning default.";
		logger::log(ww.str());
		return debug_shader;
	}
}
