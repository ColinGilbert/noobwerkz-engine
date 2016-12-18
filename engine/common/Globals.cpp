#include "Globals.hpp"
#include "MeshUtils.hpp"
#include "RandomGenerator.hpp"

bool noob::globals::init() noexcept(true) 
{
	unit_sphere_shape = sphere_shape(0.5);
	unit_cube_shape = box_shape(0.5, 0.5, 0.5);


	noob::logger::log(noob::importance::INFO, noob::concat("[Globals] unit sphere shape handle ", noob::to_string(unit_sphere_shape.index()) ,", unit cube shape handle ", noob::to_string(unit_cube_shape.index())));
	// << ", unit cylinder shape handle " << unit_cylinder_shape.index() << ", unit cone shape handle " << unit_cone_shape.index();

	noob::light l;
	l.set_colour(noob::vec3f(0.0, 1.0, 1.0));
	default_light = set_light(l, "default");

	noob::reflectance r;
	default_reflectance = set_reflectance(r, "default");

	// logger::log(noob::importance::INFO, "[Globals] Got default triplanar shader handle.");


	for (uint32_t i = 0; i < num_pseudo_randoms; ++i)
	{
		pseudo_randoms[i] = noob::random::get();
	}

	noob::logger::log(noob::importance::INFO, "[Globals] Init complete.");
	init_done = true;
	return true;
}

noob::instanced_model_handle noob::globals::model_from_shape(noob::shape_handle arg, uint32_t num) noexcept(true)
{
	noob::shape shp = shapes.get(arg);

	noob::graphics& gfx = noob::get_graphics();
	return gfx.instanced_model(shp.get_mesh(), num);
}


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

noob::shape_handle noob::globals::hull_shape(const std::vector<noob::vec3f>& points) noexcept(true) 
{
	noob::shape temp;
	temp.hull(points);
	noob::shape_handle shape_h = add_shape(temp);

	noob::mesh_3d temp_mesh = noob::mesh_utils::hull(points);
	// TODO: Replace
	// noob::model temp_model = model_from_mesh(temp_mesh);

	// auto temp_cell = shapes_to_models.insert(shape_h.index());
	// temp_cell->value = temp_model.index();

	return shape_h;
}


noob::shape_handle noob::globals::static_trimesh_shape(const noob::mesh_3d& m) noexcept(true) 
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
	std::get<1>(shapes.get_ptr_mutable(h))->set_self_index(h.index());
	return h;
}

