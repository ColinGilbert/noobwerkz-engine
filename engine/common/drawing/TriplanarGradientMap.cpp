#include "TriplanarGradientMap.hpp"

void noob::triplanar_gradient_map_renderer::init()
{
	noob::graphics& gfx = noob::graphics::get_instance();

	bgfx::ProgramHandle program_handle = gfx.load_program("vs_gradient_map_triplanar", "fs_gradient_map_triplanar");
	program_valid = (bgfx::invalidHandle != program_handle.idx);
	shader.program = program_handle;
	shader.samplers.push_back(gfx.get_texture_0());
	gfx.add_shader("gradient_map_triplanar", shader);
	fmt::MemoryWriter ww;
	ww << "[TriplanarGradientMap] Program valid? " << program_valid;
	logger::log(ww.str());
}


void noob::triplanar_gradient_map_renderer::draw(const noob::drawable* model, const noob::mat4& w_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer::uniform& uni, const noob::reflectance& reflect, const std::array<noob::light, MAX_LIGHTS>& lights, uint8_t view_id) const
{
	// Older, unlit code path. Kept as ad-hocumentation ;)
	// bgfx::setTexture(0, gfx.get_blend_0.handle, uni.texture_map.handle);
	// bgfx::setUniform(gfx.get_colour_0.handle, &uni.colours[0].v[0]);
	// bgfx::setUniform(gfx.get_colour_1.handle, &uni.colours[1].v[0]);
	// bgfx::setUniform(gfx.get_colour_2.handle, &uni.colours[2].v[0]);
	// bgfx::setUniform(gfx.get_colour_3.handle, &uni.colours[3].v[0]);
	// bgfx::setUniform(gfx.get_blend_0.handle, &uni.blend.v[0]);
	// bgfx::setUniform(gfx.get_blend_1.handle, &uni.colour_positions.v[0]);
	// bgfx::setUniform(gfx.get_tex_scales.handle, &uni.scales.v[0]);
	// bgfx::setUniform(gfx.get_normal_mat.handle, &normal_mat.m[0]);
	// model->draw(view_id, w_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);


	noob::graphics& gfx = noob::graphics::get_instance();

	bgfx::setUniform(gfx.get_normal_mat().handle, &normal_mat.m[0]);

	bgfx::setTexture(0, gfx.get_blend_0().handle, uni.texture_map.handle);
	
	bgfx::setUniform(gfx.get_colour_0().handle, &uni.colours[0].v[0]);
	bgfx::setUniform(gfx.get_colour_1().handle, &uni.colours[1].v[0]);
	bgfx::setUniform(gfx.get_colour_2().handle, &uni.colours[2].v[0]);
	bgfx::setUniform(gfx.get_colour_3().handle, &uni.colours[3].v[0]);
	bgfx::setUniform(gfx.get_blend_0().handle, &uni.blend.v[0]);
	bgfx::setUniform(gfx.get_blend_1().handle, &uni.colour_positions.v[0]);
	bgfx::setUniform(gfx.get_tex_scales().handle, &uni.scales.v[0]);

	// noob::mat4 normal_mat = noob::transpose(noob::inverse(w_mat));
	bgfx::setUniform(gfx.get_normal_mat().handle, &normal_mat.m[0]);
	
	bgfx::setUniform(gfx.get_specular_shine().handle, &reflect.specular_shine.v[0]);
	bgfx::setUniform(gfx.get_diffuse().handle, &reflect.diffuse.v[0]);
	// bgfx::setUniform(gfx.get_ambient.handle, &reflect.ambient.v[0]);
	bgfx::setUniform(gfx.get_emissive().handle, &reflect.emissive.v[0]);
	bgfx::setUniform(gfx.get_rough_albedo_fresnel().handle, &reflect.rough_albedo_fresnel.v[0]);
	
	bgfx::setUniform(gfx.get_fog().handle, &noob::vec4(0.01, 0.01, 0.01, 0.0).v[0]);
	

	std::array<noob::vec4, MAX_LIGHTS> lights_rgbi;
	for (size_t i = 0 ; i < MAX_LIGHTS; ++i)
	{
		lights_rgbi[i] = lights[i].rgb_falloff;
	}

	std::array<noob::vec4, MAX_LIGHTS> lights_pos_radii;
	for (size_t i = 0 ; i < MAX_LIGHTS; ++i)
	{
		lights_pos_radii[i] = lights[i].pos_radius;
	}

	bgfx::setUniform(gfx.get_light_rgb_falloff().handle, &lights_rgbi[0].v[0], MAX_LIGHTS);
	bgfx::setUniform(gfx.get_light_pos_radius().handle, &lights_pos_radii[0].v[0], MAX_LIGHTS);
	

	model->draw(view_id, w_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);


}
