#include "BasicRenderer.hpp"

void noob::basic_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_basic", "fs_basic");

	std::string ss;

	if (bgfx::invalidHandle == program_handle.idx)
	{
		logger::log("[Basic Renderer] invalid program!");
		return;
	}
	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	shader.program = program_handle;
	noob::graphics::add_shader("basic", shader);
}


void noob::basic_renderer::draw(const noob::drawable3d* drawable, const noob::mat4& model_mat, const noob::basic_renderer::uniform_info& info, uint8_t view_id) const
{
	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &info.colour.v[0]);
	noob::graphics::shader s = noob::graphics::get_shader("basic");
	drawable->draw(view_id, model_mat, s.program);
	bgfx::submit(view_id);
}
