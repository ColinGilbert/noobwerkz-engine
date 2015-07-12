#include "Application.hpp"
#include <boost/filesystem.hpp> ///filesystem.hpp>
#include <boost/system/error_code.hpp>

noob::application* noob::application::app_pointer = nullptr;

noob::application::application() : chai(chaiscript::Std_Lib::library()) 
{
	logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("./"));

	using namespace chaiscript;

	chai.add(user_type<noob::drawable>(), "drawable");
	chai.add(user_type<noob::mesh>(), "mesh");
	chai.add(user_type<noob::mesh::bbox_info>(), "bbox_info");
	chai.add(user_type<noob::voxel_world>(), "voxel_world");
	chai.add(user_type<noob::graphics::mesh_vertex>(), "mesh_vertex");
	chai.add(user_type<noob::graphics::uniform>(), "uniform");
	chai.add(user_type<noob::graphics::sampler>(), "sampler");
	chai.add(user_type<noob::graphics::shader>(), "shader");
	chai.add(user_type<noob::vec2>(), "vec2");
	chai.add(user_type<noob::vec3>(), "vec3");
	chai.add(user_type<noob::vec4>(), "vec4");
	chai.add(user_type<noob::mat3>(), "mat3");
	chai.add(user_type<noob::mat4>(), "mat4");
	chai.add(user_type<noob::versor>(), "versor");
	chai.add(user_type<noob::transform_helper>(), "transform_helper");

	chai.add(fun(&noob::length), "length");
	chai.add(fun(&noob::length2), "length2");
	chai.add(fun(static_cast<noob::versor (*)(const noob::versor&)>(&noob::normalize)), "normalize");
	chai.add(fun(static_cast<noob::vec3 (*)(const noob::vec3&)>(&noob::normalize)), "normalize");
	chai.add(fun(static_cast<float (*)(const noob::vec3&, const noob::vec3&)>(&noob::dot)), "dot");
	chai.add(fun(static_cast<float (*)(const noob::versor&, const noob::versor&)>(&noob::dot)), "dot");
	chai.add(fun(&noob::cross), "cross");
	chai.add(fun(&noob::get_squared_dist), "get_squared_dist");
	chai.add(fun(&noob::direction_to_heading), "direction_to_heading");
	chai.add(fun(&noob::heading_to_direction), "heading_to_direction");
	chai.add(fun(&noob::linearly_dependent), "linearly_dependent");
	chai.add(fun(&noob::zero_mat3), "zero_mat3");
	chai.add(fun(&noob::identity_mat3), "identity_mat3");
	chai.add(fun(&noob::zero_mat4), "zero_mat4");
	chai.add(fun(&noob::identity_mat4), "identity_mat4");
	chai.add(fun(&noob::determinant), "determinant");
	chai.add(fun(&noob::inverse), "inverse");
	chai.add(fun(&noob::transpose), "transpose");
	chai.add(fun(&noob::translate), "translate");
	chai.add(fun(&noob::rotate_x_deg), "rotate_x_deg");
	chai.add(fun(&noob::rotate_y_deg), "rotate_y_deg");
	chai.add(fun(&noob::rotate_z_deg), "rotate_z_deg");
	chai.add(fun(&noob::scale), "scale");
	chai.add(fun(&noob::translation_from_mat4), "translation_from_mat4");
	chai.add(fun(&noob::scale_from_mat4), "scale_from_mat4");
	chai.add(fun(&noob::look_at), "look_at");
	chai.add(fun(&noob::perspective), "perspective");
	chai.add(fun(&noob::ortho), "ortho");
	chai.add(fun(&noob::quat_from_axis_rad), "quat_from_axis_rad");
	chai.add(fun(&noob::quat_from_axis_deg), "quat_from_axis_deg");
	chai.add(fun(static_cast<noob::mat4 (*)(const noob::versor&)>(&noob::quat_to_mat4)), "quat_to_mat4");
	//chai.add(fun(static_cast<noob::versor (*)(const noob::versor&, const noob::versor&)>(&noob::slerp)), "slerp");
	chai.add(fun(static_cast<noob::versor (*)(noob::versor&, noob::versor&, float t)>(&noob::slerp)), "slerp");

	chai.add(fun(&noob::transform_helper::get_matrix), "get_matrix");
	chai.add(fun(&noob::transform_helper::scale), "scale");
	chai.add(fun(&noob::transform_helper::translate), "translate");
	chai.add(fun(&noob::transform_helper::rotate), "rotate");
	
	chai.add(fun(&noob::transform_helper::scaling), "scaling");
	chai.add(fun(&noob::transform_helper::translation), "translation");
	chai.add(fun(&noob::transform_helper::rotation), "rotation");

	chai.add(fun(&noob::mesh::vertices), "vertices");
	chai.add(fun(&noob::mesh::normals), "normals");
	chai.add(fun(&noob::mesh::indices), "indices");
	//chai.add(fun(&noob::mesh::normalize), "normalize");
	chai.add(fun(&noob::mesh::to_half_edges), "to_half_edges");
	chai.add(fun(&noob::mesh::convex_decomposition), "convex_decomposition");
	chai.add(fun(&noob::mesh::transform), "transform");
	chai.add(fun(&noob::mesh::get_bbox), "get_bbox");
	chai.add(fun(static_cast<void (noob::mesh::*)(const std::string&, size_t) const>(&noob::mesh::decimate)), "decimate");
	chai.add(fun(static_cast<noob::mesh (noob::mesh::*)(size_t) const>(&noob::mesh::decimate)), "decimate");
	chai.add(fun(static_cast<bool (noob::mesh::*)(const std::string&, const std::string&)>(&noob::mesh::load)), "load");
	chai.add(fun(static_cast<bool (noob::mesh::*)(std::tuple<size_t, const char*>, const std::string&)>(&noob::mesh::load)), "load");
	chai.add(fun(static_cast<std::tuple<size_t, const char*> (noob::mesh::*)() const>(&noob::mesh::snapshot)), "snapshot");
	chai.add(fun(static_cast<void (noob::mesh::*)(const std::string&) const>(&noob::mesh::snapshot)), "snapshot");

	chai.add(fun(&noob::voxel_world::init), "init");
	chai.add(fun(&noob::voxel_world::clear_world), "clear_world");
	chai.add(fun(&noob::voxel_world::set), "set");
	chai.add(fun(&noob::voxel_world::apply_to_region), "apply_to_region");
	chai.add(fun(&noob::voxel_world::sphere), "sphere");
	chai.add(fun(&noob::voxel_world::cube), "cube");
	chai.add(fun(&noob::voxel_world::cylinder), "cylinder");
	chai.add(fun(&noob::voxel_world::get), "get");
	chai.add(fun(&noob::voxel_world::extract_region), "extract_region");

	chai.add(user_type<noob::physics_world>(), "physics_world");
	chai.add(fun(&noob::physics_world::init), "init");
	chai.add(fun(&noob::physics_world::step), "step");
	chai.add(fun(&noob::physics_world::apply_global_force), "apply_global_force");
	chai.add(fun(&noob::physics_world::dynamic_body), "dynamic_body");
	chai.add(fun(&noob::physics_world::sphere), "sphere");
	chai.add(fun(&noob::physics_world::box), "box");
	chai.add(fun(&noob::physics_world::cylinder), "cylinder");
	chai.add(fun(&noob::physics_world::capsule), "capsule");
	chai.add(fun(&noob::physics_world::cone), "cone");
	chai.add(fun(&noob::physics_world::plane), "plane");
	chai.add(fun(&noob::physics_world::static_mesh), "static_mesh");
	chai.add(fun(static_cast<btConvexHullShape* (noob::physics_world::*)(const std::vector<noob::vec3>&)>(&noob::physics_world::convex_hull)), "convex_hull");
	chai.add(fun(static_cast<btConvexHullShape* (noob::physics_world::*)(const noob::mesh&)>(&noob::physics_world::convex_hull)), "convex_hull");
	chai.add(fun(static_cast<btCompoundShape* (noob::physics_world::*)(const noob::mesh&)>(&noob::physics_world::compound_shape)), "compound_shape");
	chai.add(fun(static_cast<btCompoundShape* (noob::physics_world::*)(const std::vector<noob::mesh>&)>(&noob::physics_world::compound_shape)), "compound_shape");
	chai.add(fun(static_cast<btCompoundShape* (noob::physics_world::*)(const std::vector<btCollisionShape*>&)>(&noob::physics_world::compound_shape)), "compound_shape");



	chai.add(var(&voxels), "voxels");

}

noob::application::~application()
{
	logger::log("~application()");
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}


void noob::application::init()
{
	logger::log("");
	// noob::editor_utils::blend_channels();

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	voxels.init();
	view_mat = noob::look_at(noob::vec3(0.0, 0.0, -500.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
	// TESTING CODE
	scene.init();

	// noob::scene::terrain_info * t = new noob::scene::terrain_info;

	noob::triplanar_renderer::uniform_info info;

	info.colours[0] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	info.colours[1] = noob::vec4(0.0, 0.5, 0.5, 1.0);
	info.colours[2] = noob::vec4(0.5, 0.5, 0.5, 1.0);
	info.colours[3] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	info.mapping_blends = noob::vec3(0.5, 0.5, 1.0);
	info.colour_positions = noob::vec2(0.3, 0.7);
	info.scales = noob::vec3(3.0, 3.0, 3.0);

	// t->colouring_info = info;

	noob::mesh m;
	m.load("terrain.off");
	// t->model.drawable = new noob::drawable();
	// t->model.drawable->init(m);

	// chai.eval("voxels.sphere(30,60,60,60,true);"); //noob::mesh d = chai.eval<noob::mesh>("function(3, 4.75);");
	// chai.eval("var m = voxels.extract_region(0,0,0,90,90,90);");
	logger::log("[Sandbox] done init.");
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);
	
	static double time_elapsed = 0.0;
	time_elapsed += delta;
	
	if (time_elapsed > 0.25)
	{
		boost::filesystem::path p;
		p += *prefix;
		p += "script.chai";
		boost::system::error_code ec;
		
		static std::time_t last_write = 0;
		std::time_t t = boost::filesystem::last_write_time(p, ec);
		if (ec != 0)
		{
			logger::log(fmt::format("[Application] - update() - error reading {0}: {1}",p.generic_string(), ec.message()));
		}	
		else if (last_write != t)
		{
			try
			{
				chai.eval_file(p.generic_string());
			}
			catch(std::exception e)
			{
				logger::log(fmt::format("[Application]. Caught ChaiScript exception: ", e.what()));
			}
			last_write = t;
		}
		
		time_elapsed = 0.0;
	}

}


void noob::application::draw()
{
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	// scene.triplanar_render.draw(*t->model.drawable_ptr, noob::identity_mat4(), t->colouring_info);

	gui.text("The goat stumbled upon the cheese", 150.0, 50.0);
	//gui.frame();

	bgfx::submit(0);
}


void noob::application::eval_script(const std::string& script)
{
	try
	{
		chai.eval(script);
	}
	catch (std::exception e)
	{
		logger::log(fmt::format("[Application] - eval_script() - exception caught: {0}", e.what()));
	}
}

void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		float damping = 360.0;
		noob::vec3 rotation(info.rotation);
		noob::vec3 translation(info.translation);
		view_mat = noob::rotate_x_deg(view_mat, -rotation[0]/damping);
		view_mat = noob::rotate_y_deg(view_mat, -rotation[1]/damping);
		view_mat = noob::rotate_z_deg(view_mat, -rotation[2]/damping);
		view_mat = noob::translate(view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
	}
}


void noob::application::step()
{
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	uint64_t uNowNano = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	double delta = (uNowNano - time) * 0.000000001f;

	time = uNowNano;

	if (!paused)
	{
		update((float)delta);
	}

	draw();
}


void noob::application::pause()
{
	paused = true;
}


void noob::application::resume()
{
	paused = false;
}


void noob::application::set_archive_dir(const std::string& filepath)
{

	{
		std::stringstream ss;
		ss << "setting archive dir(\"" << filepath << "\")";
		logger::log(ss.str());

	}	

	prefix = std::unique_ptr<std::string>(new std::string(filepath));

	{
		std::stringstream ss;
		ss << "archive dir = " << *prefix;
		logger::log(ss.str());
	}
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		{
			std::stringstream ss;
			ss << "Touch - pointerID " << pointerID << ", (" << x << ", " << y << "), action " << action;
			logger::log(ss.str());
		}

		if (pointerID < 5)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}


void noob::application::window_resize(uint32_t w, uint32_t h)
{
	window_width = w;
	window_height = h;
	{
		std::stringstream ss;
		ss << "window_resize(" << window_width << ", " << window_height << ")";
		logger::log(ss.str());
	}

	if (window_height == 0) 
	{
		window_height = 1;
	}
}
