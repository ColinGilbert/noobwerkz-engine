#include "TriplanarGradientMap.hpp"




void noob::triplanar_gradient_map_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_gradient_map_triplanar", "fs_gradient_map_triplanar");

	logger::log(fmt::format("[TriPlanar] is program valid? {0}", bgfx::invalidHandle != program_handle.idx));

	shader.program = program_handle;

	shader.samplers.push_back(noob::graphics::texture_0);

	noob::graphics::add_shader("gradient_map_triplanar", shader);
}


void noob::triplanar_gradient_map_renderer::draw(const noob::drawable* model, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::triplanar_gradient_map_renderer::uniform_info& info, uint8_t view_id) const
{
	bgfx::setTexture(0, noob::graphics::blend_0.handle, info.texture_map.handle); // noob::graphics::get_texture("grad_map"));

	bgfx::setUniform(graphics::colour_0.handle, &info.colours[0].v[0]);
	bgfx::setUniform(graphics::colour_1.handle, &info.colours[1].v[0]);
	bgfx::setUniform(graphics::colour_2.handle, &info.colours[2].v[0]);
	bgfx::setUniform(graphics::colour_3.handle, &info.colours[3].v[0]);
	bgfx::setUniform(noob::graphics::blend_0.handle, &info.blend.v[0]);
	bgfx::setUniform(noob::graphics::blend_1.handle, &info.colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::scales.handle, &info.scales.v[0]);
	bgfx::setUniform(noob::graphics::light_direction_0.handle, &info.light_dir[0].v[0]);
	bgfx::setUniform(noob::graphics::light_direction_1.handle, &info.light_dir[1].v[0]);
	bgfx::setUniform(noob::graphics::light_direction_2.handle, &info.light_dir[2].v[0]);
	bgfx::setUniform(noob::graphics::light_direction_3.handle, &info.light_dir[3].v[0]);
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);

	// noob::graphics::shader s = noob::graphics::get_shader("gradient_map_triplanar");
	model->draw(view_id, world_mat, normal_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
