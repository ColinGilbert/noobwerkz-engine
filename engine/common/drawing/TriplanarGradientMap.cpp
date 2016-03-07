#include "TriplanarGradientMap.hpp"

void noob::triplanar_gradient_map_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_gradient_map_triplanar", "fs_gradient_map_triplanar");
	program_valid = (bgfx::invalidHandle != program_handle.idx);
	shader.program = program_handle;
	shader.samplers.push_back(noob::graphics::texture_0);
	noob::graphics::add_shader("gradient_map_triplanar", shader);
	fmt::MemoryWriter ww;
	ww << "[TriplanarGradientMap] Program valid? " << program_valid;
	logger::log(ww.str());
}


void noob::triplanar_gradient_map_renderer::draw(const noob::drawable* model, const noob::mat4& wvp_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer::uniform& uni, const noob::reflectance& reflect, const std::array<noob::light, MAX_LIGHTS>& lights, uint8_t view_id) const
{
	bgfx::setTexture(0, noob::graphics::blend_0.handle, uni.texture_map.handle);

	bgfx::setUniform(graphics::colour_0.handle, &uni.colours[0].v[0]);
	bgfx::setUniform(graphics::colour_1.handle, &uni.colours[1].v[0]);
	bgfx::setUniform(graphics::colour_2.handle, &uni.colours[2].v[0]);
	bgfx::setUniform(graphics::colour_3.handle, &uni.colours[3].v[0]);
	bgfx::setUniform(noob::graphics::blend_0.handle, &uni.blend.v[0]);
	bgfx::setUniform(noob::graphics::blend_1.handle, &uni.colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::tex_scales.handle, &uni.scales.v[0]);
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);
	model->draw(view_id, wvp_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
