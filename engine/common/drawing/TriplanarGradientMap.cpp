#include "TriplanarGradientMap.hpp"

void noob::triplanar_gradient_map_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_gradient_map_triplanar", "fs_gradient_map_triplanar");

	logger::log(fmt::format("[TriPlanar] is program valid? {0}", bgfx::invalidHandle != program_handle.idx));

	shader.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shader.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("gradient_map_triplanar", shader);
	noob::graphics::load_texture("grad_map", "gradient_map.dds", BGFX_TEXTURE_NONE);
}


void noob::triplanar_gradient_map_renderer::draw(const noob::drawable* model, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::triplanar_gradient_map_renderer::uniform_info& info, uint8_t view_id) const
{

	// const uint64_t state = 0 | BGFX_STATE_CULL_CW | BGFX_STATE_DEPTH_TEST_LESS;// | BGFX_STATE_MSAA;
	// bgfx::setState(state);
	bgfx::setUniform(noob::graphics::get_uniform("colour_0").handle, &info.colours[0].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &info.colours[1].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &info.colours[2].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &info.colours[3].v[0]);

	bgfx::setUniform(noob::graphics::get_uniform("normal_mat").handle, &normal_mat.m[0]);
	bgfx::setUniform(noob::graphics::get_uniform("texture_blend").handle, &info.blend.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &info.colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &info.scales.v[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));

	bgfx::setUniform(noob::graphics::get_uniform("light_direction_0").handle, &info.light_dir[0].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("light_direction_1").handle, &info.light_dir[1].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("light_direction_2").handle, &info.light_dir[2].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("light_direction_3").handle, &info.light_dir[3].v[0]);

	noob::graphics::shader s = noob::graphics::get_shader("gradient_map_triplanar");
	model->draw(view_id, world_mat, normal_mat, s.program,  0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
