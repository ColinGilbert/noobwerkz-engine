#include "InitFunc.hpp"
#include "UserData.hpp"

bool InitFunc::init()
{
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

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Vec4);

	noob::graphics::shader shad;
	shad.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("current", shad);
	noob::graphics::load_texture("grad_map", "gradient_map.dds");

	data->droid_font = std::unique_ptr<noob::ui_font>(new noob::ui_font());

	std::string fontfile_path = *prefix + "/font/droidsans.ttf";

	data->droid_font->init(fontfile_path, 32, window_width, window_height);
	data->droid_font->set_colour(0xFFFFFFFF);
	data->world.init();
	data->sphere = std::unique_ptr<noob::drawable>(new noob::drawable());
	std::string meshFile = *prefix + "/models/test.off";

	data->sphere.set_mesh_file(meshFile);
	data->sphere.load_mesh();
	logger::log("Done init");
	return true;
}

REGISTERCLASS(InitFunc);
