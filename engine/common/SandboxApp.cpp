#include <atomic>
#include <array>
#include <ulmblas/ulmblas.h>

#include "EditorUtils.hpp"
#include "Application.hpp"

void noob::application::init()
{
	
	logger::log("");

	// TODO: Move out of editor program
	// noob::editor_utils::blend_channels();
	
	ui_enabled = true;

	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_current", "fs_current");

	{
		std::stringstream ss;
		ss << "Is program valid? ";
		if (bgfx::invalidHandle != program_handle.idx)
		{
			ss << "true";
		}
		else
		{
			ss << "false";
		}

		logger::log(ss.str());
	}

	noob::graphics::add_sampler("u_texture");

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Uniform4fv);

	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Uniform3fv);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Uniform2fv);

	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Uniform3fv);

	noob::graphics::shader shad;
	shad.program = program_handle;
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("current", shad);
	noob::graphics::load_texture("grad_map", "gradient_map.dds");

	std::string fontfile = *prefix + "/font/droidsans.ttf";
	// droid_font->init(fontfile);

	nvg = nvgCreate(1, 0);

	vox_world.init();
	vox_world.cylinder(20, 80, 100, 100, 100, noob::voxel_world::op_type::ADD);
	noob::voxel_world::region reg;
	reg.lower_x = 0;
	reg.lower_y = 0;
	reg.lower_z = 0;
	reg.upper_x = 255;
	reg.upper_y = 255;
	reg.upper_z = 255;
	vox_world.extract_region(reg, "models/test.off");
	
	sphere = std::unique_ptr<noob::drawable>(new noob::drawable());
	std::string meshFile = *prefix + "/models/test.off";
	sphere->set_mesh_file(meshFile);
	sphere->load_mesh();


	logger::log("Done init");
}

void noob::application::update(double delta)
{
	if (started == false)
	{
		

	}
	else started = true;
}

void noob::application::draw()
{
	if (ui_enabled)
	{
		bgfx::setViewSeq(0, true);
		nvgBeginFrame(nvg, width, height, 1.0f);

		nvgEndFrame(nvg);
		bgfx::setViewSeq(0, false);
	}

	noob::vec3 cam_up(0.0, 1.0, 0.0);
	noob::vec3 cam_target(0.0, 0.0, 0.0);
	noob::vec3 cam_pos(180.0, 180.0, 400.0);
	noob::mat4 view = noob::look_at(cam_pos, cam_target, cam_up);

	// Compute projection matrix
	if (height == 0) height = 1;
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(width)/static_cast<float>(height), 0.1f, 2000.0f);

	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());

	noob::vec4 colour_1(0.0, 0.3, 0.3, 1.0);
	noob::vec4 colour_2(0.0, 0.5, 0.5, 1.0);
	noob::vec4 colour_3(0.3, 0.3, 0.3, 1.0);
	noob::vec4 colour_4(1.0, 1.0, 1.0, 1.0);

	noob::vec3 mapping_blend(0.0, 0.0, 1.0);

	noob::vec2 colour_positions(0.3, 0.6);

	noob::vec3 scales(3,3,3);


	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &colour_1.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &colour_2.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &colour_3.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &colour_4.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &mapping_blend.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scales.v[0]);

	bgfx::setViewTransform(0, &view.m[0], &proj.m[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("current");

	sphere->draw(model_mat, s.program);
}
