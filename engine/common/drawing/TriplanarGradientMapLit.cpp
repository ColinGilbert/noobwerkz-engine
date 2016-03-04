#include "TriplanarGradientMapLit.hpp"

void noob::triplanar_gradient_map_renderer_lit::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_current", "fs_current");
	program_valid = (bgfx::invalidHandle != program_handle.idx);
	shader.program = program_handle;
	shader.samplers.push_back(noob::graphics::texture_0);
	noob::graphics::add_shader("gradient_map_triplanar_lit", shader);
	fmt::MemoryWriter ww;
	ww << "[TriplanarGradientMapLit] Program valid? " << program_valid;
	logger::log(ww.str());
}


void noob::triplanar_gradient_map_renderer_lit::draw(const noob::drawable* model, const noob::mat4& w_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer_lit::uniform& uni, const noob::reflectance& reflect, const std::array<noob::light, 4>& lights, uint8_t view_id) const
{
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);
	bgfx::setUniform(noob::graphics::eye_pos.handle, &eye_pos.v[0]);	
	bgfx::setUniform(noob::graphics::eye_pos_normalized.handle, &(noob::normalize(eye_pos)).v[0]);	

	bgfx::setTexture(0, noob::graphics::blend_0.handle, uni.texture_map.handle);
	
	bgfx::setUniform(graphics::colour_0.handle, &uni.colours[0].v[0]);
	bgfx::setUniform(graphics::colour_1.handle, &uni.colours[1].v[0]);
	bgfx::setUniform(graphics::colour_2.handle, &uni.colours[2].v[0]);
	bgfx::setUniform(graphics::colour_3.handle, &uni.colours[3].v[0]);
	bgfx::setUniform(noob::graphics::blend_0.handle, &uni.blend.v[0]);
	bgfx::setUniform(noob::graphics::blend_1.handle, &uni.colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::tex_scales.handle, &uni.scales.v[0]);

	noob::mat4 normal_model_mat = noob::transpose(noob::inverse(w_mat));
	bgfx::setUniform(noob::graphics::normal_mat_modelspace.handle, &normal_model_mat.m[0]);
	
	bgfx::setUniform(noob::graphics::specular_shine.handle, &reflect.specular_shine.v[0]);
	bgfx::setUniform(noob::graphics::diffuse.handle, &reflect.diffuse.v[0]);
	bgfx::setUniform(noob::graphics::ambient.handle, &reflect.ambient.v[0]);
	bgfx::setUniform(noob::graphics::emissive.handle, &reflect.emissive.v[0]);
	bgfx::setUniform(noob::graphics::fog.handle, &noob::vec4(0.01, 0.01, 0.01, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_rgb_inner_r.handle, &lights[0].rgb_inner_r.v[0]);
	bgfx::setUniform(noob::graphics::light_pos_radius.handle, &lights[0].pos_radius.v[0]);

	model->draw(view_id, w_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
