#include "TriPlanar.hpp"

void noob::triplanar_renderer::init()
{

	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_triplanar", "fs_triplanar");

	std::string ss;
	if (bgfx::invalidHandle != program_handle.idx)
	{
		ss =  "true";
	}
	else
	{
		ss = "false";
	}

	logger::log(fmt::format("[TriPlanar] is program valid? {0}", ss));

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Vec4);

	shader.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shader.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("triplanar", shader);
	noob::graphics::load_texture("grad_map", "gradient_map.dds");
}


void noob::triplanar_renderer::draw(const noob::drawable& drawable, const noob::mat4& model_mat, const noob::triplanar_renderer::uniform_info& info, uint8_t view_id) const
{
	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &info.colours[0].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &info.colours[1].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &info.colours[2].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &info.colours[3].v[0]);
	
	noob::vec4 map_blends(info.mapping_blends, 0.0);
	// map_blends[0] = info.mapping_blends[0];
	// map_blends[1] = info.mapping_blends[1];
	// map_blends[2] = info.mapping_blends[2];
	bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &map_blends.v[0]);

	noob::vec4 colour_pos(info.colour_positions, 0.0, 0.0);
	// colour_pos[0] = info.colour_positions[0];
	// colour_pos[1] = info.colour_positions[1];
	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_pos.v[0]);
	
	noob::vec4 scalings(info.scales, 0.0);
	//scalings[0] = info.scales[0];
	//scalings[1] = info.scales[1];
	//scalings[2] = info.scales[2];
	
	bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scalings.v[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("triplanar");
	drawable.draw(view_id, model_mat, s.program);
	bgfx::submit(view_id);
}
