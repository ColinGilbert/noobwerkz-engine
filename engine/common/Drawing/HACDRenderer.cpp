#include "HACDRenderer.hpp"

void noob::hacd_renderer::init()
{
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_basic", "fs_basic");

	std::string ss;
	if (bgfx::invalidHandle != program_handle.idx)
	{
		ss =  "true";
	}
	else
	{
		ss = "false";
	}

	logger::log(fmt::format("[HACDRenderer] is program valid? {0}", ss));

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	shader.program = program_handle;

	noob::graphics::add_shader("basic", shader);
}

// Lovingly ripped out of the V-HACD example
noob::vec4 compute_random_colour()
{
	noob::vec4 colour(0.0, 0.0, 0.0, 1.0);

	while (colour[0] == colour[1] || colour[2] == colour[1] || colour[2] == colour[0])
	{
		colour[0] = (rand() % 100) / 100.0f;
		colour[1] = (rand() % 100) / 100.0f;
		colour[2] = (rand() % 100) / 100.0f;
	}

	return colour;
}


void noob::hacd_renderer::set_items(const std::vector<noob::basic_mesh>& hulls)
{
	drawable3ds.clear();
	colours.clear();

	for (noob::basic_mesh current_mesh : hulls)
	{
		noob::vec4 c = compute_random_colour();
		current_mesh.snapshot("temp/hull-temp.off");
		std::unique_ptr<noob::drawable3d> d = std::unique_ptr<noob::drawable3d>(new noob::drawable3d());
		d->init(current_mesh);//"temp/hull-temp.off", "");

		// info.drawable3d.load(current_mesh.snapshot(), "hull-temp");
		drawable3ds.push_back(std::move(d));
		colours.push_back(c);
	}
}


void noob::hacd_renderer::set_items(const std::vector<std::vector<noob::vec3>>& hulls)
{

}

void noob::hacd_renderer::draw(const noob::mat4& model_mat, uint8_t view_id)
{
	for (size_t i = 0; i < drawable3ds.size(); i++)
	{
		bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &colours[i].v[0]);
		noob::graphics::shader s = noob::graphics::get_shader("basic");
		drawable3ds[i]->draw(view_id, model_mat, s.program);
		bgfx::submit(view_id);
	}
}
