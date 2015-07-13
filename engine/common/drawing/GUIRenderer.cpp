/*#include "GUIRenderer.hpp"


void noob::gui_renderer::init(size_t width, size_t height)
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
	
	set_dims(width, height);
}


void noob::gui_renderer::draw(const noob::drawable2d& shape, float x, float y, const noob::gui_renderer::uniform_info& info, uint8_t view_id) const
{
	bgfx::setViewTransform(view_id, &view_matrix.m[0], &ortho.m[0]);
	bgfx::setViewRect(view_id, 0, 0, window_width, window_height);

	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &info.colours[0].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &info.colours[1].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &info.colours[2].v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &info.colours[3].v[0]);
	
	noob::vec4 map_blends(info.mapping_blends, 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &map_blends.v[0]);

	noob::vec4 colour_pos(info.colour_positions, 0.0, 0.0);
	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_pos.v[0]);
	
	noob::vec4 scalings(info.scales, 0.0);
	
	bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scalings.v[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("triplanar");

	noob::mat4 model_mat = noob::identity_mat4();
	model_mat = noob::translate(model_mat, noob::vec3(x, y, 0.0));

	shape.draw(view_id, model_mat, s.program);
	bgfx::submit(view_id);
}


void noob::gui_renderer::set_dims(size_t width, size_t height)
{
	window_width = static_cast<float>(width);
	window_height = static_cast<float>(height);

	noob::vec3 at(0.0, 0.0, 0.0);
	noob::vec3 eye(0, 0, -1.0f);
	noob::vec3 up(0.0, 1.0, 0.0); 
	view_matrix = noob::look_at(eye, at, up);
	const float centering = 0.5f;
	
	ortho = noob::ortho(centering, window_width + centering, window_height + centering, centering, -1.0, 1.0);


}
*/
