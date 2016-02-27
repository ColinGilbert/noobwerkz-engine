#include "TriplanarGradientMapLit.hpp"

void noob::triplanar_gradient_map_renderer_lit::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_current", "fs_current");
	program_valid = (bgfx::invalidHandle != program_handle.idx);
	shader.program = program_handle;
	shader.samplers.push_back(noob::graphics::texture_0);
	noob::graphics::add_shader("gradient_map_triplanar_lit", shader);
	fmt::MemoryWriter ww;
	ww << "[TriplanarGradientMap] Program valid? " << program_valid;
	logger::log(ww.str());
}


void noob::triplanar_gradient_map_renderer_lit::draw(const noob::drawable* model, const noob::mat4& wvp_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer_lit::uniform& uni, const std::array<noob::light, 4>& lights, uint8_t view_id) const
{
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);
	bgfx::setUniform(noob::graphics::eye_pos.handle, &eye_pos.v[0]);	

	bgfx::setTexture(0, noob::graphics::blend_0.handle, uni.texture_map.handle);
	bgfx::setUniform(graphics::colour_0.handle, &uni.colours[0].v[0]);
	bgfx::setUniform(graphics::colour_1.handle, &uni.colours[1].v[0]);
	bgfx::setUniform(graphics::colour_2.handle, &uni.colours[2].v[0]);
	bgfx::setUniform(graphics::colour_3.handle, &uni.colours[3].v[0]);
	bgfx::setUniform(noob::graphics::blend_0.handle, &uni.blend.v[0]);
	bgfx::setUniform(noob::graphics::blend_1.handle, &uni.colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::tex_scales.handle, &uni.scales.v[0]);

	// noob::vec3 position, direction, colour, emissive, ambient, diffuse, specular;

	bgfx::setUniform(noob::graphics::global_ambient.handle, &noob::vec4(0.6, 0.6, 0.6, 0.0).v[0]);
	

	bgfx::setUniform(noob::graphics::light_colour_0.handle, &noob::vec4(lights[0].colour, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_direction_0.handle, &noob::vec4(lights[0].direction, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_pos_0.handle, &noob::vec4(lights[0].position, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::emissive_coeff_0.handle, &noob::vec4(lights[0].emissive, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::diffuse_coeff_0.handle, &noob::vec4(lights[0].diffuse, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::ambient_coeff_0.handle, &noob::vec4(lights[0].ambient, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::specular_coeff_0.handle, &noob::vec4(lights[0].specular, 0.0).v[0]);

	bgfx::setUniform(noob::graphics::light_colour_1.handle, &noob::vec4(lights[1].colour, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_direction_1.handle, &noob::vec4(lights[1].direction, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_pos_1.handle, &noob::vec4(lights[1].position, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::emissive_coeff_1.handle, &noob::vec4(lights[1].emissive, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::diffuse_coeff_1.handle, &noob::vec4(lights[1].diffuse, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::ambient_coeff_1.handle, &noob::vec4(lights[1].ambient, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::specular_coeff_1.handle, &noob::vec4(lights[1].specular, 0.0).v[0]);

	bgfx::setUniform(noob::graphics::light_colour_2.handle, &noob::vec4(lights[2].colour, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_direction_2.handle, &noob::vec4(lights[2].direction, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_pos_2.handle, &noob::vec4(lights[2].position, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::emissive_coeff_2.handle, &noob::vec4(lights[2].emissive, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::diffuse_coeff_2.handle, &noob::vec4(lights[2].diffuse, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::ambient_coeff_2.handle, &noob::vec4(lights[2].ambient, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::specular_coeff_2.handle, &noob::vec4(lights[2].specular, 0.0).v[0]);

	bgfx::setUniform(noob::graphics::light_colour_3.handle, &noob::vec4(lights[3].colour, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_direction_3.handle, &noob::vec4(lights[3].direction, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::light_pos_3.handle, &noob::vec4(lights[3].position, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::emissive_coeff_3.handle, &noob::vec4(lights[3].emissive, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::diffuse_coeff_3.handle, &noob::vec4(lights[3].diffuse, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::ambient_coeff_3.handle, &noob::vec4(lights[3].ambient, 0.0).v[0]);
	bgfx::setUniform(noob::graphics::specular_coeff_3.handle, &noob::vec4(lights[3].specular, 0.0).v[0]);


	noob::vec4 specular_powers_temp;
	noob::vec4 spot_powers_temp;
	for (size_t i = 0; i < 4; ++i)
	{
		specular_powers_temp.v[i] = lights[i].specular_power;	
		spot_powers_temp.v[i] = lights[i].spot_power;
	}

	bgfx::setUniform(noob::graphics::specular_powers.handle, &specular_powers_temp.v[0]);
	bgfx::setUniform(noob::graphics::spot_powers.handle, &spot_powers_temp.v[0]);


	// uniform vec4 emissive_coeff_0;
	// uniform vec4 diffuse_coeff_0;
	// uniform vec4 ambient_coeff_0;
	// uniform vec4 specular_coeff_0;
	// uniform vec4 light_direction_0;
	// uniform vec4 light_pos_0;
	// uniform vec4 light_colour_0;

	// uniform vec4 specular_powers;
	// uniform vec4 spot_powers;

	model->draw(view_id, wvp_mat, shader.program, 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
