#include "BasicRenderer.hpp"

void noob::basic_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_basic", "fs_basic");

	std::string ss;

	program_valid = (bgfx::invalidHandle != program_handle.idx);
	
	// noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	
	shader.program = program_handle;
	noob::graphics::add_shader("basic", shader);
	fmt::MemoryWriter ww;
	ww <<"[BasicRenderer] Program valid? " << program_valid;
	logger::log(ww.str());
}


void noob::basic_renderer::draw(const noob::drawable* model, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos,  const noob::basic_renderer::uniform& uni, const std::array<noob::light, 4 >& lights, uint8_t view_id) const
{
	bgfx::setUniform(noob::graphics::get_uniform("colour_0").handle, &uni.colour.v[0]);
	//noob::graphics::shader s = noob::graphics::get_shader("basic");
	model->draw(view_id, world_mat, shader.program);
}
