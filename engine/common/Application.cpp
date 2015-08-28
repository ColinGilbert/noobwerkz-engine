#include "Application.hpp"
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

noob::application* noob::application::app_pointer = nullptr;

noob::application::application() 
{
	logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("./"));
	view_mat = noob::look_at(noob::vec3(0, 100.0, -500.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
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

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	voxels.init();
	stage.init();
/*
	chai = std::unique_ptr<chaiscript::ChaiScript>(new chaiscript::ChaiScript(chaiscript::Std_Lib::library()));
	using namespace chaiscript;

	chai->add(user_type<noob::graphics::uniform>(), "uniform");
	chai->add(user_type<noob::graphics::sampler>(), "sampler");
	chai->add(user_type<noob::graphics::shader>(), "shader");
	chai->add(user_type<noob::vec2>(), "vec2");
	chai->add(user_type<noob::vec3>(), "vec3");
	chai->add(user_type<noob::vec4>(), "vec4");
	chai->add(user_type<noob::mat3>(), "mat3");
	chai->add(user_type<noob::mat4>(), "mat4");
	chai->add(user_type<noob::versor>(), "versor");
	chai->add(user_type<noob::transform_helper>(), "transform_helper");
	chai->add(user_type<noob::triplanar_renderer::uniform_info>(), "triplanar_uniform");

	chaiscript::ModulePtr vec2 = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::vec2>(*vec2, "vec2",
			{
			constructor<noob::vec2()>(),
			constructor<noob::vec2(float, float)>()
			},
			{
			{fun(&noob::vec2::operator[]), "[]" },
			{fun(&noob::vec2::to_string), "to_string"}
			});
	chai->add(vec2);

	chaiscript::ModulePtr vec3 = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::vec3>(*vec3, "vec3",
			{
			constructor<noob::vec3()>(),
			constructor<noob::vec3(float, float, float)>(),
			constructor<noob::vec3(const noob::vec2&, float)>(),
			constructor<noob::vec3(const noob::vec4&)>(),
			constructor<noob::vec3(const noob::vec3&)>(),
			constructor<noob::vec3(const btVector3&)>()
			},
			{
			{ fun(static_cast<noob::vec3 (noob::vec3::*)(const noob::vec3&) const>(&noob::vec3::operator+)), "+"},
			{ fun(static_cast<noob::vec3 (noob::vec3::*)(float) const>(&noob::vec3::operator+)), "+"},
			{ fun(&noob::vec3::operator+=), "+=" },
			{ fun(static_cast<noob::vec3 (noob::vec3::*)(const noob::vec3&) const>(&noob::vec3::operator-)), "-"},
			{ fun(static_cast<noob::vec3 (noob::vec3::*)(float) const>(&noob::vec3::operator-)), "-"},
			{ fun(&noob::vec3::operator-=), "-=" },
			{ fun(&noob::vec3::operator*), "*" },
			{ fun(&noob::vec3::operator*=), "*=" },
			{ fun(&noob::vec3::operator/), "/" },
			{ fun(&noob::vec3::operator=), "=" },
			{fun(&noob::vec3::operator[]), "[]" },
			{fun(&noob::vec3::to_string), "to_string"}
			});
	chai->add(vec3);

	chaiscript::ModulePtr vec4 = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::vec4>(*vec4, "vec4",
			{
			constructor<noob::vec4()>(),
			constructor<noob::vec4(float, float, float, float)>(),
			constructor<noob::vec4(const noob::vec2&, float, float)>(),
			constructor<noob::vec4(const noob::vec3&, float)>()
			},
			{
			{fun(&noob::vec4::operator[]), "[]" },
			{fun(&noob::vec4::to_string), "to_string"}
			});
	chai->add(vec4);

	chai->add(fun(&noob::length), "length");
	chai->add(fun(&noob::length2), "length2");
	chai->add(fun(static_cast<noob::versor (*)(const noob::versor&)>(&noob::normalize)), "normalize");
	chai->add(fun(static_cast<noob::vec3 (*)(const noob::vec3&)>(&noob::normalize)), "normalize");
	chai->add(fun(static_cast<float (*)(const noob::vec3&, const noob::vec3&)>(&noob::dot)), "dot");
	chai->add(fun(static_cast<float (*)(const noob::versor&, const noob::versor&)>(&noob::dot)), "dot");
	chai->add(fun(&noob::cross), "cross");
	chai->add(fun(&noob::get_squared_dist), "get_squared_dist");
	chai->add(fun(&noob::direction_to_heading), "direction_to_heading");
	chai->add(fun(&noob::heading_to_direction), "heading_to_direction");
	chai->add(fun(&noob::linearly_dependent), "linearly_dependent");
	chai->add(fun(&noob::zero_mat3), "zero_mat3");
	chai->add(fun(&noob::identity_mat3), "identity_mat3");
	chai->add(fun(&noob::zero_mat4), "zero_mat4");
	chai->add(fun(&noob::identity_mat4), "identity_mat4");
	chai->add(fun(&noob::determinant), "determinant");
	chai->add(fun(&noob::inverse), "inverse");
	chai->add(fun(&noob::transpose), "transpose");
	chai->add(fun(&noob::translate), "translate");
	chai->add(fun(&noob::rotate_x_deg), "rotate_x_deg");
	chai->add(fun(&noob::rotate_y_deg), "rotate_y_deg");
	chai->add(fun(&noob::rotate_z_deg), "rotate_z_deg");
	chai->add(fun(&noob::scale), "scale");
	chai->add(fun(&noob::translation_from_mat4), "translation_from_mat4");
	chai->add(fun(&noob::scale_from_mat4), "scale_from_mat4");
	chai->add(fun(&noob::look_at), "look_at");
	chai->add(fun(&noob::perspective), "perspective");
	chai->add(fun(&noob::ortho), "ortho");
	chai->add(fun(&noob::quat_from_axis_rad), "quat_from_axis_rad");
	chai->add(fun(&noob::quat_from_axis_deg), "quat_from_axis_deg");
	chai->add(fun(static_cast<noob::mat4 (*)(const noob::versor&)>(&noob::quat_to_mat4)), "quat_to_mat4");
	//chai->add(fun(static_cast<noob::versor (*)(const noob::versor&, const noob::versor&)>(&noob::slerp)), "slerp");
	chai->add(fun(static_cast<noob::versor (*)(noob::versor&, noob::versor&, float t)>(&noob::slerp)), "slerp");

	chai->add(fun(&noob::transform_helper::get_matrix), "get_matrix");
	chai->add(fun(&noob::transform_helper::scale), "scale");
	chai->add(fun(&noob::transform_helper::translate), "translate");
	chai->add(fun(&noob::transform_helper::rotate), "rotate");
	chai->add(fun(&noob::transform_helper::scaling), "scaling");
	chai->add(fun(&noob::transform_helper::translation), "translation");
	chai->add(fun(&noob::transform_helper::rotation), "rotation");

	chaiscript::ModulePtr mesh = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::mesh>(*mesh, "mesh",
			{ constructor<noob::mesh()>() },
			{
			{ fun(&noob::mesh::vertices), "vertices" },
			{ fun(&noob::mesh::normals), "normals" },
			{ fun(&noob::mesh::indices), "indices" },
			{ fun(&noob::mesh::normalize), "normalize" },
			{ fun(&noob::mesh::to_origin), "to_origin" },
			{ fun(&noob::mesh::to_half_edges), "to_half_edges" },
			{ fun(static_cast<std::vector<noob::mesh>(noob::mesh::*)(void) const>(&noob::mesh::convex_decomposition)), "convex_decomposition" },
			{ fun(&noob::mesh::transform), "transform" },
			{ fun(&noob::mesh::get_bbox), "get_bbox" },
			{ fun(static_cast<void (noob::mesh::*)(const std::string&, size_t) const>(&noob::mesh::decimate)), "decimate" },
			{ fun(static_cast<noob::mesh (noob::mesh::*)(size_t) const>(&noob::mesh::decimate)), "decimate" }, 
			{ fun(static_cast<bool (noob::mesh::*)(const std::string&, const std::string&)>(&noob::mesh::load)), "load" }, 
			{ fun(static_cast<bool (noob::mesh::*)(std::tuple<size_t, const char*>, const std::string&)>(&noob::mesh::load)), "load" },
			{ fun(static_cast<std::tuple<size_t, const char*> (noob::mesh::*)() const>(&noob::mesh::snapshot)), "snapshot" },
			{ fun(static_cast<void (noob::mesh::*)(const std::string&) const>(&noob::mesh::snapshot)), "snapshot" }
			});
	chai->add(mesh);

	chaiscript::ModulePtr drawable = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::drawable3d>(*drawable, "drawable3d",
			{ constructor<noob::drawable3d()>() },
			{
			{fun(static_cast<void (noob::drawable3d::*)(const noob::mesh&)>(&noob::drawable3d::init)), "init"},
			{fun(&noob::drawable3d::draw), "draw"},
			{fun(&noob::drawable3d::get_bbox), "get_bbox"}
			});
	chai->add(drawable);

	chaiscript::ModulePtr triplanar_uniform = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::triplanar_renderer::uniform_info>(*triplanar_uniform, "triplanar_uniform",
			{ constructor<noob::triplanar_renderer::uniform_info()>() },
			{
			{ fun(&noob::triplanar_renderer::uniform_info::set_colour), "set_colour" },
			{ fun(&noob::triplanar_renderer::uniform_info::set_mapping_blends), "set_mapping_blends" },
			{ fun(&noob::triplanar_renderer::uniform_info::set_scales), "set_scales" },
			{ fun(&noob::triplanar_renderer::uniform_info::set_colour_positions), "set_colour_positions" }
			});
	chai->add(triplanar_uniform);


	chaiscript::ModulePtr body_properties = chaiscript::ModulePtr(new chaiscript::Module());
	chaiscript::utility::add_class<noob::physics_world::body_properties>(*body_properties, "body_properties",
			{ constructor<noob::physics_world::body_properties()>() },
			{
			{ fun(&noob::physics_world::body_properties::mass), "mass" },
			{ fun(&noob::physics_world::body_properties::linear_damping), "linear_damping" },
			{ fun(&noob::physics_world::body_properties::angular_damping), "angular_damping" },
			{ fun(&noob::physics_world::body_properties::linear_factor), "linear_factor" },
			{ fun(&noob::physics_world::body_properties::angular_factor), "angular_factor" }
			//{ fun(&noob::physics_world::body_properties::shape), "shape" },
			});
	chai->add(body_properties);

	chai->add(var(&voxels), "voxels");
	chai->add(fun(&noob::voxel_world::clear_world), "clear_world");
	chai->add(fun(&noob::voxel_world::set), "set");
	chai->add(fun(&noob::voxel_world::apply_to_region), "apply_to_region");
	chai->add(fun(&noob::voxel_world::sphere), "sphere");
	chai->add(fun(&noob::voxel_world::cube), "cube");
	chai->add(fun(&noob::voxel_world::cylinder), "cylinder");
	chai->add(fun(&noob::voxel_world::get), "get");
	chai->add(fun(&noob::voxel_world::extract_region), "extract_region");

	chai->add(var(&noob::application::scene), "scene");
	chai->add(var(&noob::application::scene.physics), "physics");

	chai->add(fun(&noob::physics_world::apply_global_force), "apply_global_force");
	chai->add(fun(&noob::physics_world::create_body), "create_body");
	chai->add(fun(static_cast<void (noob::physics_world::*)(btGeneric6DofSpring2Constraint*)>(&noob::physics_world::add)), "add");
	chai->add(fun(static_cast<void (noob::physics_world::*)(btRigidBody*, short, short)>(&noob::physics_world::add)), "add");

	chai->add(fun(&noob::physics_world::sphere), "sphere");
	chai->add(fun(&noob::physics_world::box), "box");
	chai->add(fun(&noob::physics_world::cylinder), "cylinder");
	chai->add(fun(&noob::physics_world::capsule), "capsule");
	chai->add(fun(&noob::physics_world::cone), "cone");
	chai->add(fun(&noob::physics_world::plane), "plane");
	chai->add(fun(&noob::physics_world::static_mesh), "static_mesh");
	chai->add(fun(static_cast<btConvexHullShape* (noob::physics_world::*)(const std::vector<noob::vec3>&)>(&noob::physics_world::convex_hull)), "convex_hull");
	chai->add(fun(static_cast<btConvexHullShape* (noob::physics_world::*)(const noob::mesh&)>(&noob::physics_world::convex_hull)), "convex_hull");
	chai->add(fun(&noob::physics_world::compound_shape_decompose), "compound_shape_decompose");
	chai->add(fun(static_cast<btCompoundShape* (noob::physics_world::*)(const std::vector<noob::mesh>&)>(&noob::physics_world::compound_shape)), "compound_shape");
	chai->add(fun(static_cast<btCompoundShape* (noob::physics_world::*)(const std::vector<btCollisionShape*>&)>(&noob::physics_world::compound_shape)), "compound_shape");

	chai->add(fun(static_cast<void (noob::scene3d::*)(const btRigidBody*, const noob::drawable3d*, const noob::triplanar_renderer::uniform_info&, const std::string&)>(&noob::scene3d::add)), "add");
	chai->add(fun(static_cast<void (noob::scene3d::*)(const btGeneric6DofSpring2Constraint*, const std::string&)>(&noob::scene3d::add)), "add");
	chai->add(fun(&noob::scene3d::remove_actors), "remove_actors");
	chai->add(fun(&noob::scene3d::remove_constraints), "remove_constraints");
	chai->add(fun(&noob::scene3d::get_bodies), "get_bodies");
	chai->add(fun(&noob::scene3d::get_constraints), "get_constraints");
*/

	logger::log("[Sandbox] done init.");
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);

	static double time_elapsed = 0.0;
	time_elapsed += delta;

	stage.update(delta);

/*
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
			logger::log(fmt::format("[Application] - update() - error reading {0}: {1}", p.generic_string(), ec.message()));
		}	
		else if (last_write != t)
		{
			init();

			try
			{
				chai->eval_file(p.generic_string());
			}
			catch(std::exception e)
			{
				logger::log(fmt::format("[Application]. Caught ChaiScript exception: ", e.what()));
			}
			last_write = t;
		}
		time_elapsed = 0.0;
	}
	*/
}


void noob::application::draw()
{

	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	stage.draw();

	gui.text("The goat stumbled upon the cheese", 150.0, 50.0);
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
		update(delta);
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

	logger::log(fmt::format("[Application] Setting archive directory (\"{0}\")", filepath));
	prefix = std::unique_ptr<std::string>(new std::string(filepath));
	logger::log(fmt::format("[Application] Archive dir = {0}", *prefix));
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		logger::log(fmt::format("[Application] Touch - pointer ID = {0}, ({1}, {2}), action = {3}", pointerID, x, y, action));

		if (pointerID < 3)
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
	if (window_height == 0) 
	{
		window_height = 1;
	}

	logger::log(fmt::format("[Application] Resize window to ({0}, {1})", window_width, window_height));
}
