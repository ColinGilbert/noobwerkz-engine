#include "Globals.hpp"

noob::globals* noob::globals::ptr_to_instance;

bool noob::globals::init()
{
	basic_drawer.init();
	triplanar_drawer.init();

	// renderer.init();
	unit_sphere_shape = sphere_shape(0.5);
	unit_cube_shape = box_shape(1.0, 1.0, 1.0);
	unit_cylinder_shape = cylinder_shape(0.5, 1.0);
	unit_cone_shape = cone_shape(0.5, 1.0);

	fmt::MemoryWriter ww;
	ww << "[Globals] unit sphere shape handle " << unit_sphere_shape.get_inner() << ", unit cube shape handle " << unit_cube_shape.get_inner() << ", unit cylinder shape handle " << unit_cylinder_shape.get_inner() << ", unit cone shape handle " << unit_cone_shape.get_inner();
	logger::log(ww.str());

	// logger::log("[Globals] Making unit sphere model");
	unit_sphere_model = model_from_mesh(noob::mesh_utils::sphere(0.5), "unit-sphere");//basic_models.add(std::move(temp));
	// logger::log("[Globals] Making unit cube model");
	unit_cube_model = model_from_mesh(noob::mesh_utils::box(1.0, 1.0, 1.0), "unit-cube");
	// unit_cube_model = model_from_mesh(noob::mesh_utils::box(1.0, 1.0, 1.0));
	// logger::log("[Globals] Making unit cylinder model");
	// unit_cylinder_model = model_from_mesh(noob::mesh_utils::cylinder(1.0, 0.5));
	unit_cylinder_model = model_from_mesh(noob::mesh_utils::cylinder(0.5, 1.0), "unit-cylinder");
	// logger::log("[Globals] Making unit cone model");
	unit_cone_model = model_from_mesh(noob::mesh_utils::cone(0.5, 1.0), "unit-cone");

	fmt::MemoryWriter ww_2;
	ww_2 << "[Globals] unit sphere model handle " << unit_sphere_model.model_h.get_inner() << ", unit cube model handle " << unit_cube_model.model_h.get_inner() << ", unit cylinder model handle " << unit_cylinder_model.model_h.get_inner() << ", unit cone model handle " << unit_cone_model.model_h.get_inner();
	logger::log(ww_2.str());

	//  Init basic default shader
	noob::basic_renderer::uniform dbg;
	dbg.colour = noob::vec4(1.0, 1.0, 1.0, 0.0);
	set_shader(dbg, "debug");
	// logger::log("[Globals] Set debug shader");
	shader_results t1 = get_shader("debug");
	debug_shader = basic_shaders.make_handle(t1.handle);
	// logger::log("[Globals] Got debug shader handle");
	// Init triplanar shader. For fun.
	noob::triplanar_gradient_map_renderer::uniform triplanar_info;

	triplanar_info.colours[0] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	triplanar_info.colours[1] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	triplanar_info.colours[2] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	triplanar_info.colours[3] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	triplanar_info.blend = noob::vec4(1.0, 0.0, 0.0, 0.0);
	triplanar_info.scales = noob::vec4(1.0, 1.0, 1.0, 0.0);
	triplanar_info.colour_positions = noob::vec4(0.3, 0.6, 0.0, 0.0);
	set_shader(triplanar_info, "default-triplanar");
	// logger::log("[Globals] Set default triplanar shader.");

	shader_results t2 = get_shader("default-triplanar");
	default_triplanar_shader = triplanar_shaders.make_handle(t2.handle);

	noob::light l;
	l.set_colour(noob::vec3(0.0, 1.0, 1.0));
	default_light = set_light(l, "default");

	noob::reflectance r;
	default_reflectance = set_reflectance(r, "default");
	// logger::log("[Globals] Got default triplanar shader handle.");
	logger::log("[Globals] Init complete.");
	return true;
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

noob::scaled_model noob::globals::sphere_model(float r)
{
	noob::scaled_model temp = unit_sphere_model;
	temp.scales = noob::vec3(r*2.0, r*2.0, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::box_model(float x, float y, float z)
{
	noob::scaled_model temp;
	temp.model_h= unit_cube_model.model_h;
	temp.scales = noob::vec3(x, y, z);
	return temp;
}


noob::scaled_model noob::globals::cylinder_model(float r, float h)
{
	noob::scaled_model temp = unit_cylinder_model;
	temp.scales = noob::vec3(r*2.0, h, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::cone_model(float r, float h)
{
	noob::scaled_model temp = unit_cone_model;
	temp.scales = noob::vec3(r*2.0, h, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::model_from_mesh(const noob::basic_mesh& m, const std::string& name)
{
	std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
	temp->init(m);
	noob::basic_models_holder::handle h;
	auto results = names_to_basic_models.find(name);
	if (results != names_to_basic_models.end())
	{
		basic_models.set(results->second, std::move(temp));
		h = results->second;//std::move(temp);
	}
	else
	{
		h = basic_models.add(std::move(temp));
		//h = temp.model_h;
		names_to_basic_models.insert(std::make_pair(name, h));
	}

	noob::scaled_model retval;
	retval.model_h = h;
	retval.scales = noob::vec3(1.0, 1.0, 1.0);
	return retval;
}


noob::scaled_model noob::globals::model_by_shape(const noob::shapes_holder::handle h)
{
	// fmt::MemoryWriter ww;
	// ww << "[Globals] about to get model from shape " << h.get_inner();
	// logger::log(ww.str());

	scaled_model results;
	//results.scales = noob::vec3(1.0, 1.0, 1.0);
	noob::shape* s = shapes.get(h);

	// logger::log("[Globals] got shape pointer");

	switch(s->shape_type)
	{
		// logger::log("[Globals] choosing shape type");

		case(noob::shape::type::SPHERE):
		results.model_h = unit_sphere_model.model_h;
		results.scales = s->scales;
		return results;
		case(noob::shape::type::BOX):
		results.model_h = unit_cube_model.model_h;
		results.scales = s->scales;
		return results;
		case(noob::shape::type::CYLINDER):
		results.model_h = unit_cylinder_model.model_h;
		results.scales = s->scales;
		return results;
		case(noob::shape::type::CONE):
		results.model_h = unit_cone_model.model_h;
		results.scales = s->scales;
		return results;
		case(noob::shape::type::HULL):
		{
			auto search = shapes_to_models.find(h.get_inner());
			if (search == shapes_to_models.end())
			{
				results.model_h = search->second;
				results.scales = noob::vec3(1.0, 1.0, 1.0);
				return results;
			}
		}
		case(noob::shape::type::TRIMESH):
		{
			auto search = shapes_to_models.find(h.get_inner());
			if (search == shapes_to_models.end())
			{
				results.model_h = search->second;
				results.scales = noob::vec3(1.0, 1.0, 1.0);					
				return results;
			}
		}
		default:
		{
			logger::log("[Globals] - USER DATA WARNING - INVALID SHAPE TO MODEL :(");
			break;
		}
	};
	return results; 
}


noob::shapes_holder::handle noob::globals::sphere_shape(float r)
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


noob::shapes_holder::handle noob::globals::box_shape(float x, float y, float z)
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


noob::shapes_holder::handle noob::globals::cylinder_shape(float r, float h)
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


noob::shapes_holder::handle noob::globals::cone_shape(float r, float h)
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


noob::shapes_holder::handle noob::globals::hull_shape(const std::vector<vec3>& points, const std::string& name)
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


noob::shapes_holder::handle noob::globals::static_trimesh_shape(const noob::basic_mesh& m, const std::string& name)
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


noob::lights_holder::handle noob::globals::set_light(const noob::light& l, const std::string& s)
{
	auto search = names_to_lights.find(s);
	if (search != names_to_lights.end())
	{
		lights.set(search->second, l);
		return search->second;
	}
	else
	{
		noob::lights_holder::handle h = lights.add(l);
		names_to_lights.insert(std::make_pair(s, h));
		return h;
	}

}


noob::lights_holder::handle noob::globals::get_light(const std::string& s) const
{
	noob::lights_holder::handle temp;
	auto search = names_to_lights.find(s);
	if (search != names_to_lights.end())
	{		
		temp = search->second;
	}
	return temp;
}



noob::reflectances_holder::handle noob::globals::set_reflectance(const noob::reflectance& r, const std::string& s)
{
	auto search = names_to_reflectances.find(s);
	if (search != names_to_reflectances.end())
	{
		reflectances.set(search->second, r);
		return search->second;
	}
	else
	{
		noob::reflectances_holder::handle h = reflectances.add(r);
		names_to_reflectances.insert(std::make_pair(s, h));
		return h;
	}
}


noob::reflectances_holder::handle noob::globals::get_reflectance(const std::string& s) const
{
	noob::reflectances_holder::handle temp;
	auto search = names_to_reflectances.find(s);
	if (search != names_to_reflectances.end())
	{		
		temp = search->second;
	}
	return temp;
}




/*
   noob::light noob::globals::get_light(const noob::lights_holder::handle h) const
   {
   return lights.get(h);
   }
   */

void noob::globals::set_shader(const noob::basic_renderer::uniform& u, const std::string& name)
{
	auto search = names_to_shaders.find(name);
	if (search == names_to_shaders.end())
	{
		noob::basic_shader_handle h = basic_shaders.add(u);
		noob::globals::shader_results r;
		r.type = noob::shader_type::BASIC;
		r.handle = h.get_inner();

		names_to_shaders.insert(std::make_pair(name, r));
	}
}


void noob::globals::set_shader(const noob::triplanar_gradient_map_renderer::uniform& u, const std::string& name)
{
	// set_shader(noob::prepared_shaders::uniform(u), name);
	auto search = names_to_shaders.find(name);
	if (search == names_to_shaders.end())
	{
		noob::triplanar_shader_handle h = triplanar_shaders.add(u);
		noob::globals::shader_results r;
		r.type = noob::shader_type::TRIPLANAR;
		r.handle = h.get_inner();

		names_to_shaders.insert(std::make_pair(name, r));
	}
}


noob::globals::shader_results noob::globals::get_shader(const std::string& s) const
{
	noob::globals::shader_results results;
	auto search = names_to_shaders.find(s);
	if (search != names_to_shaders.end())
	{
		// fmt::MemoryWriter ww;		
		// ww << "[Globals] Found shader " << s << " with handle " << names_to_shaders[s].get_inner() << ".";// << ". Returning default.";
		// logger::log(ww.str());		
		results = search->second;
	}
	else
	{
		fmt::MemoryWriter ww;
		ww << "[Globals] Could not find shader " << s << ". Returning default.";
		logger::log(ww.str());
	}
	return results;
}
