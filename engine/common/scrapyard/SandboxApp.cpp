#include <atomic>
#include <array>

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>
*/

//#include "EditorUtils.hpp"
#include "Application.hpp"
//#include "Shape2D.hpp"

void noob::application::init()
{

	logger::log("");
	// TODO: Move out of editor program?
	// noob::editor_utils::blend_channels();


	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	//triplanar_render.init();
	//hacd_render.init();

	scene.init();
	//cam.init(5.0, 100.0, noob::vec3(0.0, 0.0, 100.0));
	//voxel_world.init();
	/*
	//logger::log("[Sandbox] - Creating sphere.");
	//voxel_world.sphere(30, 128, 128, 128);
	//logger::log("[Sandbox] - Creating cube.");	
	voxel_world.cube(30,30,30, 60, 60, 60);
	logger::log("[Sandbox] - Done making the cube. Extracting region.");
	sample_mesh = voxel_world.extract_region(0, 0, 0, 256, 256, 256);
	//sample_mesh.snapshot("temp/temp-voxworld.off");
	logger::log("[Sandbox] - Region extracted. Decimating.");
	//sample_mesh.load("temp/temp-voxworld.off");
	sample_mesh = sample_mesh.decimate(10000);
	logger::log("[Sandbox] - Decimated. Creating drawable.");
	sample_drawable = std::unique_ptr<noob::drawable>(new noob::drawable());
	logger::log("[Sandbox] - Drawable created. About to init.");
	sample_drawable->init(sample_mesh); 
	logger::log("[Sandbox] - Drawable initialized.");
	//noob::mat4 T = sample_drawable->get_transform();

	// T = noob::rotate_y_deg(T, 60);
	// T = noob::translate(T, noob::vec3(15, 15, 15));
	// T = noob::scale(T, noob::vec3(2,1,1));
	// transformed_mesh = sample_mesh.transform(T);
	// transformed_drawable = std::unique_ptr<noob::drawable>(new noob::drawable());
	// transformed_drawable->init(transformed_mesh); 
	logger::log("[Sandbox] About to do convex decomposition.");
	hulls = sample_mesh.convex_decomposition();
	hacd_render.set_items(hulls);
*/
	noob::vec3 cam_up(0.0, 1.0, 0.0);
	noob::vec3 cam_target(0.0, 0.0, 0.0);
	noob::vec3 cam_pos(0.0, 0.0, 512.0);
	view_mat = noob::look_at(cam_pos, cam_target, cam_up);
	//noob::mesh::bbox_info bbox = sample_mesh.get_bbox();
	//logger::log(fmt::format("[Sandbox] sample_drawable (local coords) bbox center = {0}, max extent = {1}, min extent = {2}.", bbox.bbox_center.to_string(), bbox.max.to_string(), bbox.min.to_string()));

	//bbox = transformed_mesh.get_bbox();
	//logger::log(fmt::format("[Sandbox] transformed drawable (local coords) bbox center = {0}, max extent = {1}, min extent = {2}.", bbox.bbox_center.to_string(), bbox.max.to_string(), bbox.min.to_string()));
	//logger::log(fmt::format("[Sandbox] sample mesh = {0}"));
	gui.crosshairs(true);

	/*
	   class Test
	   {
	   public:
	   void function() {}
	   std::string function2() { return "Function2"; }
	   void function3() {}
	   std::string functionOverload(double) { return "double"; }
	   std::string functionOverload(int) { return "int"; }
	   };

	   ModulePtr m = ModulePtr(new Module());
	   utility::add_class<Test>(*m,
	   "Test",
	   { constructor<Test()>(),
	   constructor<Test(const Test &)>() },
	   { {fun(&Test::function), "function"},
	   {fun(&Test::function2), "function2"},
	   {fun(&Test::function2), "function3"}
	   {fun(static_cast<std::string Test::*(double)>(&Test::functionOverload)), "functionOverload"}
	   {fun(static_cast<std::string Test::*(int)>(&Test::functionOverload)), "functionOverload"} }
	   );
	   ChaiScript chai;
	   chai.add(m);
	   */
/*
	using namespace chaiscript;

	chai.add(user_type<noob::drawable>(), "drawable_t");
	chai.add(user_type<noob::mesh>(), "mesh_t");
	chai.add(user_type<noob::mesh::bbox_info>(), "bbox_info_t");
	chai.add(user_type<noob::voxel_world>(), "voxel_world_t");
	chai.add(user_type<noob::transform>(), "transform_t");
	
	chai.add(user_type<noob::graphics::mesh_vertex>(), "vertex_t");
	chai.add(user_type<noob::graphics::uniform>(), "uniform_t");
	chai.add(user_type<noob::graphics::sampler>(), "sampler_t");
	chai.add(user_type<noob::graphics::shader>(), "shader_t");

	// chai.add(chaiscript::fun<ReturnType (ParamType1, ParamType2)>(&function_with_overloads), "function_name");
	// chai.add(chaiscript::fun(std::static_cast<ReturnType (*)(ParamType1, ParamType2)>(&function_with_overloads)), "function_name");

	//chai.add(constructor<noob::mesh>(), "mesh");
	//chai.add(fun(std::static_cast<void(*)(const std::string&, size_t)>(&noob::mesh::decimate)), "decimate");
	//chai.add(fun<noob::mesh(size_t)>(&noob::mesh::decimate), "decimate");
	
	// chai.add(fun(&noob::mesh::decimate), "decimate");
	chai.add(chaiscript::fun<void(noob::mesh::*)(const std::string&, size_t)>(&noob::mesh::decimate), "decimate");
	//chai.add(fun(&noob::mesh::decimate), "decimate");
	chai.add(fun(&noob::mesh::normalize), "normalize");
	// chai.add(fun(&noob::mesh::snapshot), "snapshot");
	// chai.add(fun(&noob::mesh::snapshot), "snapshot");
	// chai.add(fun(&noob::mesh::snapshot), "snapshot");
	// chai.add(fun(&noob::mesh::load), "load");
	// chai.add(fun(&noob::mesh::load), "load");
	// chai.add(fun(&noob::mesh::load), "load");
	// chai.add(fun(&noob::mesh::load), "load");
	
	//chai.add(fun<std::tuple<size_t, const char*>(void)>(&noob::mesh::snapshot), "snapshot");
	//chai.add(fun<void(const std::string&)>(&noob::mesh::snapshot), "snapshot");
	//chai.add(fun<bool(const std::string&, const std::string&)>(&noob::mesh::load), "load");
	//chai.add(fun<bool(std::tuple<size_t, const char*>, const std::string&)>(&noob::mesh::load), "load");
	chai.add(fun(&noob::mesh::transform), "transform");
	chai.add(fun(&noob::mesh::convex_decomposition), "convex_decomposition");
	chai.add(fun(&noob::mesh::get_bbox), "get_bbox");

//	static noob::mesh csg(const noob::mesh& a, const noob::mesh& b, const noob::csg_op op);
//	static noob::mesh cone(float radius, float height, size_t subdivides = 0);
//	static noob::mesh cube(float width, float height, float depth, size_t subdivides = 0);
//	static noob::mesh cylinder(float radius, float height, size_t subdivides = 0);
//	static noob::mesh sphere(float radius);

	chai.add(var(&voxel_world), "world");
	chai.add(fun(&noob::voxel_world::init), "init");
	chai.add(fun(&noob::voxel_world::clear_world), "clear_world");
	chai.add(fun(&noob::voxel_world::set), "set");
	chai.add(fun(&noob::voxel_world::apply_to_region), "apply_to_region" );
	chai.add(fun(&noob::voxel_world::sphere), "sphere");
	chai.add(fun(&noob::voxel_world::cube), "cube");
	chai.add(fun(&noob::voxel_world::cylinder), "cylinder");
	chai.add(fun(&noob::voxel_world::get), "get");
	chai.add(fun(&noob::voxel_world::extract_region), "extract_region");

	chai.add(var(&gui), "gui");
	chai.add(fun(&noob::gui::text), "text");
	chai.add(fun(&noob::gui::crosshairs), "crosshairs");
*/


	logger::log("[Sandbox] done init.");
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);
	if (started == false)
	{
	}
	else started = true;
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		float damping = 360.0;

		view_mat = noob::rotate_x_deg(view_mat, -info.rotation[0]/damping);
		view_mat = noob::rotate_y_deg(view_mat, -info.rotation[1]/damping);
		view_mat = noob::rotate_z_deg(view_mat, -info.rotation[2]/damping);
		view_mat = noob::translate(view_mat, noob::vec3(-info.translation[0]/damping, -info.translation[1]/damping, -info.translation[2]/damping));
	}
}


void noob::application::draw()
{
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	// noob::mesh::bbox_info bbox = sample_drawable->get_bbox();
	// noob::mat4 model_mat = noob::translate(noob::identity_mat4(), noob::vec3(-bbox.bbox_center[0], -bbox.bbox_center[1], -bbox.bbox_center[2]));
	// model_mat = noob::rotate_x_deg(model_mat, -30.0f);
	// model_mat = noob::rotate_z_deg(model_mat, 45.0f);
/*
	noob::triplanar_renderer::uniform_info info;
	info.colours[0] = noob::vec4(0.0, 0.0, 0.0, 1.0);
	info.colours[1] = noob::vec4(0.0, 0.5, 0.5, 1.0);
	info.colours[2] = noob::vec4(0.5, 0.5, 0.5, 1.0);
	info.colours[3] = noob::vec4(1.0, 1.0, 1.0, 1.0);
	info.mapping_blends = noob::vec3(0.5, 0.5, 1.0);
	info.colour_positions = noob::vec2(0.3, 0.7);
	info.scales = noob::vec3(3.0, 3.0, 3.0);
*/
	//triplanar_render.draw(*sample_drawable, model_mat, info);
	//triplanar_render.draw(*transformed_drawable, model_mat, info);

	//hacd_render.draw(model_mat);
	gui.text("The goat stumbled upon the cheese", 150.0, 50.0);
	gui.frame();
	
	bgfx::submit(0);
}
