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


void noob::basic_renderer::draw(const noob::drawable* model, const noob::mat4& w_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::basic_renderer::uniform& uni, const noob::reflectance& reflect, const std::array<noob::light, MAX_LIGHTS>& lights, uint8_t view_id) const
{
	// bgfx::setUniform(noob::graphics::colour_0.handle, &uni.colour.v[0]);
	// noob::graphics::shader s = noob::graphics::get_shader("basic");
	// model->draw(view_id, world_mat, shader.program);
	
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);


	bgfx::setUniform(noob::graphics::colour_0.handle, &uni.colour.v[0]);

	// noob::mat4 normal_mat = noob::transpose(noob::inverse(w_mat));
	bgfx::setUniform(noob::graphics::normal_mat.handle, &normal_mat.m[0]);
	
	bgfx::setUniform(noob::graphics::specular_shine.handle, &reflect.specular_shine.v[0]);
	bgfx::setUniform(noob::graphics::diffuse.handle, &reflect.diffuse.v[0]);
	// bgfx::setUniform(noob::graphics::ambient.handle, &reflect.ambient.v[0]);
	bgfx::setUniform(noob::graphics::emissive.handle, &reflect.emissive.v[0]);
	bgfx::setUniform(noob::graphics::rough_albedo_fresnel.handle, &reflect.rough_albedo_fresnel.v[0]);
	
	// bgfx::setUniform(noob::graphics::fog.handle, &noob::vec4(0.01, 0.01, 0.01, 0.0).v[0]);

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

	bgfx::setUniform(noob::graphics::light_rgb_falloff.handle, &lights_rgbi[0].v[0], MAX_LIGHTS);
	bgfx::setUniform(noob::graphics::light_pos_radius.handle, &lights_pos_radii[0].v[0], MAX_LIGHTS);
	

	model->draw(view_id, w_mat, shader.program,  0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);

}
