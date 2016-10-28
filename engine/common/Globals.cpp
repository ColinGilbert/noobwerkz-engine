#include "Globals.hpp"
#include "MeshUtils.hpp"
#include "RandomGenerator.hpp"

noob::globals* noob::globals::ptr_to_instance;

bool noob::globals::init() noexcept(true) 
{
	unit_sphere_shape = sphere_shape(0.5);
	unit_cube_shape = box_shape(0.5, 0.5, 0.5);
	// unit_cylinder_shape = cylinder_shape(0.5, 1.0);
	// unit_cone_shape = cone_shape(0.5, 1.0);

	noob::logger::log(noob::importance::INFO, noob::concat("[Globals] unit sphere shape handle ", noob::to_string(unit_sphere_shape.index()) ,", unit cube shape handle ", noob::to_string(unit_cube_shape.index())));
	// << ", unit cylinder shape handle " << unit_cylinder_shape.index() << ", unit cone shape handle " << unit_cone_shape.index();

	noob::graphics& gfx = noob::graphics::get_instance();

	// TODO: Replace:
	noob::logger::log(noob::importance::INFO, "[Globals] Making unit sphere model");
	unit_sphere_model = gfx.model_instanced(noob::mesh_utils::sphere(0.5, 1), 1024);
	noob::logger::log(noob::importance::INFO, "[Globals] Making unit cube model");
	unit_cube_model = gfx.model_instanced(noob::mesh_utils::box(1.0, 1.0, 1.0), 1024);

	// logger::log(noob::importance::INFO, "[Globals] Making unit cone model");
	// unit_cone_model = model_from_mesh(noob::mesh_utils::cone(0.5, 1.0, 8));

	// logger::log(noob::importance::INFO, "[Globals] Making unit cylinder model");
	// unit_cylinder_model = model_from_mesh(noob::mesh_utils::cylinder(0.5, 1.0, 8));

	noob::logger::log(noob::importance::INFO, noob::concat("[Globals] unit sphere model handle ", noob::to_string(unit_sphere_model.index()) ,", unit cube model handle ", noob::to_string(unit_cube_model.index())));// << ", unit cylinder model handle " << unit_cylinder_model.model_h.index() << ", unit cone model handle " << unit_cone_model.model_h.index();


	noob::light l;
	l.set_colour(noob::vec3(0.0, 1.0, 1.0));
	default_light = set_light(l, "default");

	noob::reflectance r;
	default_reflectance = set_reflectance(r, "default");

	// logger::log(noob::importance::INFO, "[Globals] Got default triplanar shader handle.");


	noob::random_generator rng;

	for (uint32_t i = 0; i < num_pseudo_randoms; ++i)
	{
		pseudo_randoms[i] = rng.get();
	}

	noob::logger::log(noob::importance::INFO, "[Globals] Init complete.");
	init_done = true;
	return true;
}


std::tuple<noob::model_handle, noob::vec3> noob::globals::sphere_model(float r) noexcept(true) 
{
	noob::model_handle temp = unit_sphere_model;
	noob::vec3 scales(r*2.0, r*2.0, r*2.0);
	return std::make_tuple(temp, scales);
}


std::tuple<noob::model_handle, noob::vec3> noob::globals::box_model(float x, float y, float z) noexcept(true) 
{
	noob::model_handle temp = unit_cube_model;
	noob::vec3 scales(x, y, z);
	return std::make_tuple(temp, scales);
}

/*
   std::tuple<noob::model, noob::vec3> noob::globals::cylinder_model(float r, float h) noexcept(true) 
   {
   std::tuple<noob::model, bool, noob::vec3> temp = unit_cylinder_model;
   temp.scales = noob::vec3(r*2.0, h, r*2.0);
   return temp;
   }


   std::tuple<noob::model, noob::vec3> noob::globals::cone_model(float r, float h) noexcept(true) 
   {
   std::tuple<noob::model, bool, noob::vec3> temp = unit_cone_model;
   temp.scales = noob::vec3(r*2.0, h, r*2.0);
   return temp;
   }
   */
/*
   std::tuple<noob::model, noob::vec3> noob::globals::model_from_mesh(const noob::basic_mesh& m) noexcept(true) 
   {

   std::tuple<noob::model, bool, noob::vec3> results;
   noob::graphics& gfx = noob::graphics::get_instance();
   results.model_h = gfx.model(noob::graphics::model::geom_type::INDEXED_MESH, m);
   results.scales = noob::vec3(1.0, 1.0, 1.0);
   return results;
   }
   */

/*
   noob::model noob::globals::model_from_shape(const noob::shape_handle h) noexcept(true) 
   {
// fmt::MemoryWriter ww;
// ww << "[Globals] about to get model from shape " << h.index();
// logger::log(noob::importance::INFO, ww.str());

std::tuple<noob::model, bool, noob::vec3> results;
noob::shape s = shapes.get(h);

// logger::log(noob::importance::INFO, "[Globals] got shape pointer");

bool please_insert = false;

switch(s.shape_type)
{
// logger::log(noob::importance::INFO, "[Globals] choosing shape type");

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
results.model_h = noob::graphics::model_handle::make(val);
}
else
{
noob::logger::log(noob::importance::INFO, "[Globals] DATA ERROR: Attempted to get a hull model with an invalid shape handle.");
}
results.scales = noob::vec3(1.0, 1.0, 1.0);					
break; opengles3 book
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
results.model_h = noob::graphics::model_handle::make(val);
}
else
{
	noob::logger::log(noob::importance::INFO, noob::concat("[Globals] DATA ERROR: Shape handle ", noob::to_string(h.index()), " has invalid model mapping."));
}
}	
// We must first create the model:
else
{
	noob::shape shp = shapes.get(h);
	noob::basic_mesh m = shp.get_mesh();
	std::tuple<noob::model, bool, noob::vec3> temp_scaled_model = model_from_mesh(m);
	results.model_h = temp_scaled_model.model_h;
	please_insert = true;
}

results.scales = noob::vec3(1.0, 1.0, 1.0);					
break;
}
default:
{
	noob::logger::log(noob::importance::INFO, "[Globals] INVALID ENUM: model_from_shape()");
}
}

if (please_insert)
{
	noob::fast_hashtable::cell* search = shapes_to_models.insert(h.index());
	search->value = results.model_h.index();

}
return results; 
}
*/

noob::shape_handle noob::globals::sphere_shape(float r) noexcept(true) 
{
	std::string s = noob::concat("sphere-", noob::to_string(static_cast<uint32_t>(r)));

	auto search = names_to_shapes.find(rde::string(s.c_str()));
	// auto search = sphere_shapes.find(r);
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.sphere(r);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(s.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}


noob::shape_handle noob::globals::box_shape(float x, float y, float z) noexcept(true) 
{
	std::string s = noob::concat("box-", noob::to_string(static_cast<uint32_t>(x)), "-", noob::to_string(static_cast<uint32_t>(y)), "-", noob::to_string(static_cast<uint32_t>(z)));
	auto search = names_to_shapes.find(rde::string(s.c_str()));
	//auto search = box_shapes.find(std::make_tuple(x,y,z));
	if (search == names_to_shapes.end())
	{
		noob::shape temp;
		temp.box(x, y, z);

		auto results = names_to_shapes.insert(rde::make_pair(rde::string(s.c_str()), add_shape(temp)));
		return (results.first)->second;
	}
	return search->second;
}

/*
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
*/

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
	// TODO: Replace
	// noob::model temp_model = model_from_mesh(temp_mesh);

	// auto temp_cell = shapes_to_models.insert(shape_h.index());
	// temp_cell->value = temp_model.index();

	return shape_h;
}


noob::shape_handle noob::globals::static_trimesh_shape(const noob::basic_mesh& m) noexcept(true) 
{
	noob::shape temp;
	temp.trimesh(m);
	return add_shape(temp);
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

/*
   void noob::globals::set_shader(const noob::basic_renderer::uniform& u, const std::string& name) noexcept(true) 
   {
   auto search = names_to_shaders.find(rde::string(name.c_str()));
   if (search == names_to_shaders.end())
   {
   noob::basic_shader_handle h = basic_shaders.add(u);
   noob::shader_variant r;
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
   noob::shader_variant r;
   r.type = noob::shader_type::TRIPLANAR;
   r.handle = h.index();

   names_to_shaders.insert(rde::make_pair(rde::string(name.c_str()), r));
   }
   }
   */


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

