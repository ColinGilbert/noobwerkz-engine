#include "TriplanarGradientMap.hpp"

void noob::triplanar_gradient_map_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_gradient_map_triplanar", "fs_gradient_map_triplanar");

	logger::log(fmt::format("[TriPlanar] is program valid? {0}", bgfx::invalidHandle != program_handle.idx));

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_0_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_1_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_2_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_3_direction"), bgfx::UniformType::Enum::Vec4);

	shader.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shader.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("gradient_map_triplanar", shader);
	noob::graphics::load_texture("grad_map", "gradient_map.dds", BGFX_TEXTURE_NONE);
}


void noob::triplanar_gradient_map_renderer::draw(const noob::drawable* model, const noob::mat4& world_mat, const noob::triplanar_gradient_map_renderer::uniform_info& info, uint8_t view_id) const
{

	// const uint64_t state = 0 | BGFX_STATE_CULL_CW | BGFX_STATE_DEPTH_TEST_LESS;// | BGFX_STATE_MSAA;
	// bgfx::setState(state);
	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &info.colours[0].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &info.colours[1].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &info.colours[2].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &info.colours[3].v[0]);

	noob::vec4 map_blends(info.mapping_blends, 0.0);

	bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &map_blends.v[0]);

	noob::vec4 colour_pos(info.colour_positions, 0.0, 0.0);

	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_pos.v[0]);

	//noob::vec4 scalings(info.scales, 1.0);
	//bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scalings.v[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("gradient_map_triplanar");

	noob::graphics::add_uniform(std::string("light_0_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_1_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_2_direction"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("light_3_direction"), bgfx::UniformType::Enum::Vec4);

	noob::vec4 light_dir_0(info.light_dir[0], 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("light_0_direction").handle, &light_dir_0.v[0]);
	noob::vec4 light_dir_1(info.light_dir[1], 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("light_1_direction").handle, &light_dir_1.v[0]);
	noob::vec4 light_dir_2(info.light_dir[2], 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("light_2_direction").handle, &light_dir_2.v[0]);
	noob::vec4 light_dir_3(info.light_dir[3], 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("light_3_direction").handle, &light_dir_3.v[0]);


	model->draw(view_id, world_mat, s.program,  0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_WRITE | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
}
