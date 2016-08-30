#include "Globals.hpp"
#include "MeshUtils.hpp"
#include "RandomGenerator.hpp"

noob::globals* noob::globals::ptr_to_instance;

bool noob::globals::init() noexcept(true) 
{
	basic_drawer.init();
	triplanar_drawer.init();

	// renderer.init();
	unit_sphere_shape = sphere_shape(0.5);
	unit_cube_shape = box_shape(0.5, 0.5, 0.5);
	unit_cylinder_shape = cylinder_shape(0.5, 1.0);
	unit_cone_shape = cone_shape(0.5, 1.0);

	fmt::MemoryWriter ww;
	ww << "[Globals] unit sphere shape handle " << unit_sphere_shape.index() << ", unit cube shape handle " << unit_cube_shape.index() << ", unit cylinder shape handle " << unit_cylinder_shape.index() << ", unit cone shape handle " << unit_cone_shape.index();
	logger::log(ww.str());
	logger::log("[Globals] Making unit sphere model");
	unit_sphere_model = model_from_mesh(noob::mesh_utils::sphere(0.5, 1));//basic_models.add(std::move(temp));
	logger::log("[Globals] Making unit cube model");
	unit_cube_model = model_from_mesh(noob::mesh_utils::box(1.0, 1.0, 1.0));
	

	logger::log("[Globals] Making unit cone model");
	unit_cone_model = model_from_mesh(noob::mesh_utils::cone(0.5, 1.0, 8));
	
	logger::log("[Globals] Making unit cylinder model");
	unit_cylinder_model = model_from_mesh(noob::mesh_utils::cylinder(0.5, 1.0, 8));

	fmt::MemoryWriter ww_2;
	ww_2 << "[Globals] unit sphere model handle " << unit_sphere_model.model_h.index() << ", unit cube model handle " << unit_cube_model.model_h.index() << ", unit cylinder model handle " << unit_cylinder_model.model_h.index() << ", unit cone model handle " << unit_cone_model.model_h.index();
	logger::log(ww_2.str());

	//  Init basic default shader
	noob::basic_renderer::uniform dbg;
	dbg.colour = noob::vec4(1.0, 1.0, 1.0, 0.0);
	set_shader(dbg, "debug");
	// logger::log("[Globals] Set debug shader");
	shader t1 = get_shader("debug");
	debug_shader = basic_shader_handle::make(t1.handle);
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

	shader t2 = get_shader("default-triplanar");
	default_triplanar_shader = triplanar_shader_handle::make(t2.handle);

	noob::light l;
	l.set_colour(noob::vec3(0.0, 1.0, 1.0));
	default_light = set_light(l, "default");

	noob::reflectance r;
	default_reflectance = set_reflectance(r, "default");

	// logger::log("[Globals] Got default triplanar shader handle.");

	noob::actor_blueprints bp;
	set_actor_blueprints(bp, "default");

	strings.add(std::move(std::make_unique<std::string>("default")));

	audio_interface.init();

	noob::random_generator rng;

	for (uint32_t i = 0; i < num_pseudo_randoms; ++i)
	{
		pseudo_randoms[i] = rng.get();
	}

	logger::log("[Globals] Init complete.");
	init_done = true;
	return true;
}


noob::scaled_model noob::globals::sphere_model(float r) noexcept(true) 
{
	noob::scaled_model temp = unit_sphere_model;
	temp.scales = noob::vec3(r*2.0, r*2.0, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::box_model(float x, float y, float z) noexcept(true) 
{
	noob::scaled_model temp = unit_cube_model;
	temp.scales = noob::vec3(x, y, z);
	return temp;
}


noob::scaled_model noob::globals::cylinder_model(float r, float h) noexcept(true) 
{
	noob::scaled_model temp = unit_cylinder_model;
	temp.scales = noob::vec3(r*2.0, h, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::cone_model(float r, float h) noexcept(true) 
{
	noob::scaled_model temp = unit_cone_model;
	temp.scales = noob::vec3(r*2.0, h, r*2.0);
	return temp;
}


noob::scaled_model noob::globals::model_from_mesh(const noob::basic_mesh& m) noexcept(true) 
{
	std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
	temp->init(m);
	noob::model_handle h = basic_models.add(std::move(temp));
	noob::scaled_model retval;
	retval.model_h = h;
	retval.scales = noob::vec3(1.0, 1.0, 1.0);
	return retval;
}


noob::scaled_model noob::globals::model_from_shape(const noob::shape_handle h) noexcept(true) 
{
	// fmt::MemoryWriter ww;
	// ww << "[Globals] about to get model from shape " << h.index();
	// logger::log(ww.str());

	scaled_model results;
	noob::shape s = shapes.get(h);

	// logger::log("[Globals] got shape pointer");

	bool please_insert = false;

	switch(s.shape_type)
	{
		// logger::log("[Globals] choosing shape type");

		case(noob::shape::type::SPHERE):
			{
				results.model_h = unit_sphere_model.model_h;
				results.scales = s.scales;
				break;
			}
		case(noob::shape::type::BOX):
			{
				results.model_h = unit_cube_model.model_h;
				results.scales = s.scales;
				break;
			}
		case(noob::shape::type::CYLINDER):
			{
				results.model_h = unit_cylinder_model.model_h;
				results.scales = s.scales;
				break;
			}
		case(noob::shape::type::CONE):
			{
				results.model_h = unit_cone_model.model_h;
				results.scales = s.scales;
				break;
			}
		case(noob::shape::type::HULL):
			{
				noob::fast_hashtable::cell* search = shapes_to_models.lookup(h.index());
				if (shapes_to_models.is_valid(search))
				{
					size_t val = search->value;
					results.model_h = model_handle::make(val);
				}
				else
				{
					fmt::MemoryWriter ww;
					ww << "[Globals] DATA ERROR: Attempted to get a hull model with an invalid shape handle.";
					logger::log(ww.str());
				}
				results.scales = noob::vec3(1.0, 1.0, 1.0);					
				break; 
			}

		case(noob::shape::type::TRIMESH):
			{
				static uint32_t trimesh_model_count = 0;
				noob::fast_hashtable::cell* search = shapes_to_models.lookup(h.index());
				if (shapes_to_models.is_valid(search))
				{
					size_t val = search->value;
					if (val != std::numeric_limits<size_t>::max())
					{
						// We can simply give back the results:
						results.model_h = model_handle::make(val);
					}
					else
					{
						fmt::MemoryWriter ww;
						ww << "[Globals] DATA ERROR: Shape handle " << h.index() << " has invalid model mapping.";
						logger::log(ww.str());
					}
				}	
				// We must first create the model:
				else
				{
					noob::shape shp = shapes.get(h);
					noob::basic_mesh m = shp.get_mesh();
					noob::scaled_model temp_scaled_model = model_from_mesh(m);
					results.model_h = temp_scaled_model.model_h;
					please_insert = true;
				}

				results.scales = noob::vec3(1.0, 1.0, 1.0);					
				break;
			}
		default:
			{
				logger::log("[Globals] INVALID ENUM: model_from_shape()");
			}
	}

	if (please_insert)
	{
		noob::fast_hashtable::cell* search = shapes_to_models.insert(h.index());
		search->value = results.model_h.index();

	}
	return results; 
}


noob::shape_handle noob::globals::sphere_shape(float r) noexcept(true) 
{
	fmt::MemoryWriter w;
	w << "sphere-" << static_cast<uint32_t>(r);

	auto search = names_to_shapes.find(rde::string(w.c_str()));
	// auto search = sphere_shapes.find(r);
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.sphere(r);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(w.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}


noob::shape_handle noob::globals::box_shape(float x, float y, float z) noexcept(true) 
{
	fmt::MemoryWriter w;
	w << "box-" << static_cast<uint32_t>(x) << "-" << static_cast<uint32_t>(y)  << "-" << static_cast<uint32_t>(z);
	auto search = names_to_shapes.find(rde::string(w.c_str()));
	//auto search = box_shapes.find(std::make_tuple(x,y,z));
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.box(x, y, z);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(w.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}


noob::shape_handle noob::globals::cylinder_shape(float r, float h) noexcept(true) 
{
	// auto search = cylinder_shapes.find(std::make_tuple(r, h));
	fmt::MemoryWriter w;
	w << "cyl-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);
	auto search = names_to_shapes.find(rde::string(w.c_str()));
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.cylinder(r, h);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(w.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}


noob::shape_handle noob::globals::cone_shape(float r, float h) noexcept(true) 
{
	fmt::MemoryWriter w;
	w << "cone-" << static_cast<uint32_t>(r) << "-" << static_cast<uint32_t>(h);
	auto search = names_to_shapes.find(rde::string(w.c_str()));
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.cone(r, h);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(w.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}


//   noob::shape_handle noob::globals::capsule(float r, float h)
//   {
//   auto search = capsule_shapes.find(std::make_tuple(r, h));
//   if (search == capsule_shapes.end())
//   {
//   std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
//   temp->capsule(r, h);
//   auto results = capsule_shapes.insert(std::make_pair(std::make_tuple(r, h), add_shape(std::move(temp))));
//   return (results.first)->second;
//   }
//   return capsule_shapes[std::make_tuple(r, h)];
//   }


//  noob::shape_handle noob::globals::plane(const noob::vec3& normal, float offset)
// {
// auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
// if (search == planes.end())
// {
// std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
// temp->plane(normal, offset);
// auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), add_shape(std::move(temp))));
// return (results.first)->second;
// }
// return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
// }


noob::shape_handle noob::globals::hull_shape(const std::vector<vec3>& points) noexcept(true) 
{
	noob::shape temp;
	temp.hull(points);
	noob::shape_handle shape_h = add_shape(temp);

	noob::basic_mesh temp_mesh = noob::mesh_utils::hull(points);
	noob::scaled_model temp_scaled_model = model_from_mesh(temp_mesh);

	auto temp_cell = shapes_to_models.insert(shape_h.index());
	temp_cell->value = temp_scaled_model.model_h.index();

	return shape_h;
}


noob::shape_handle noob::globals::static_trimesh_shape(const noob::basic_mesh& m) noexcept(true) 
{
	noob::shape temp;
	temp.trimesh(m);
	return add_shape(temp);
}


noob::animated_model_handle noob::globals::animated_model(const std::string& filename) noexcept(true) 
{
	std::unique_ptr<noob::animated_model> temp = std::make_unique<noob::animated_model>();
	temp->init(filename);
	return animated_models.add(std::move(temp));
}


noob::skeletal_anim_handle noob::globals::skeleton(const std::string& filename) noexcept(true) 
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletal_anims.add(std::move(temp));
}


noob::light_handle noob::globals::set_light(const noob::light& l, const std::string& s) noexcept(true) 
{
	auto search = names_to_lights.find(rde::string(s.c_str()));
	if (search != names_to_lights.end())
	{
		lights.set(search->second, l);
		return search->second;
	}
	else
	{
		noob::light_handle h = lights.add(l);
		names_to_lights.insert(rde::make_pair(rde::string(s.c_str()), h));
		return h;
	}

}


noob::light_handle noob::globals::get_light(const std::string& s) const noexcept(true) 
{
	noob::light_handle temp;
	auto search = names_to_lights.find(rde::string(s.c_str()));
	if (search != names_to_lights.end())
	{		
		temp = search->second;
	}
	return temp;
}


noob::reflectance_handle noob::globals::set_reflectance(const noob::reflectance& r, const std::string& s) noexcept(true) 
{
	auto search = names_to_reflectances.find(rde::string(s.c_str()));
	if (search != names_to_reflectances.end())
	{
		reflectances.set(search->second, r);
		return search->second;
	}
	else
	{
		noob::reflectance_handle h = reflectances.add(r);
		names_to_reflectances.insert(rde::make_pair(rde::string(s.c_str()), h));
		return h;
	}
}


noob::reflectance_handle noob::globals::get_reflectance(const std::string& s) const noexcept(true) 
{
	noob::reflectance_handle temp;
	auto search = names_to_reflectances.find(rde::string(s.c_str()));
	if (search != names_to_reflectances.end())
	{		
		temp = search->second;
	}
	return temp;
}


void noob::globals::set_shader(const noob::basic_renderer::uniform& u, const std::string& name) noexcept(true) 
{
	auto search = names_to_shaders.find(rde::string(name.c_str()));
	if (search == names_to_shaders.end())
	{
		noob::basic_shader_handle h = basic_shaders.add(u);
		noob::shader r;
		r.type = noob::shader_type::BASIC;
		r.handle = h.index();

		names_to_shaders.insert(rde::make_pair(rde::string(name.c_str()), r));
	}
}


void noob::globals::set_shader(const noob::triplanar_gradient_map_renderer::uniform& u, const std::string& name) noexcept(true) 
{
	auto search = names_to_shaders.find(rde::string(name.c_str()));
	if (search == names_to_shaders.end())
	{
		noob::triplanar_shader_handle h = triplanar_shaders.add(u);
		noob::shader r;
		r.type = noob::shader_type::TRIPLANAR;
		r.handle = h.index();

		names_to_shaders.insert(rde::make_pair(rde::string(name.c_str()), r));
	}
}


noob::shader noob::globals::get_shader(const std::string& s) const noexcept(true) 
{
	noob::shader results;
	auto search = names_to_shaders.find(rde::string(s.c_str()));

	fmt::MemoryWriter ww;
	if (search != names_to_shaders.end())
	{
		ww << "[Globals] Found shader " << s << ", with handle " << (search->second).to_string() << ".";
		results = search->second;
	}
	else
	{
		ww << "[Globals] Could not find shader " << s << ". Returning default.";
	}

	logger::log(ww.str());
	return results;
}


void noob::globals::set_actor_blueprints(const noob::actor_blueprints& bp, const std::string& name) noexcept(true)
{

	auto search = names_to_actor_blueprints.find(rde::string(name.c_str()));
	if (search == names_to_actor_blueprints.end())
	{
		noob::actor_blueprints_handle h = actor_blueprints.add(bp);

		names_to_actor_blueprints.insert(rde::make_pair(rde::string(name.c_str()), h));
	}
}


noob::actor_blueprints_handle noob::globals::get_actor_blueprints(const std::string& name) const noexcept(true)
{

	auto search = names_to_actor_blueprints.find(rde::string(name.c_str()));
	if (search != names_to_actor_blueprints.end())
	{
		return search->second;
	}

	return noob::actor_blueprints_handle::make(0);
}

double noob::globals::get_random() noexcept(true)
{
	if (current_random == num_pseudo_randoms - 1)
	{
		current_random = 0;
	}

	return pseudo_randoms[current_random];

}


// Hack used to set the shape's index-to-self
noob::shape_handle noob::globals::add_shape(const noob::shape& s) noexcept(true)
{
	noob::shape_handle h = shapes.add(s);
	std::get<1>(shapes.get_ptr_mutable(h))->get_inner_mutable()->setUserIndex(h.index());
	return h;
}

