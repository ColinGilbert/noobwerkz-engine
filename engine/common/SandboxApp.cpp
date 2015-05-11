#include "EditorUtils.hpp"
#include "Application.hpp"

void noob::application::init()
{

	logger::log("");

	// noob::editor_utils::blend_channels();

	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_current", "fs_current");

	{
		std::stringstream ss;
		ss << "Is program valid? ";
		if (bgfx::invalidHandle != program_handle.idx)
		{
			ss << "true";
		}
		else
		{
			ss << "false";
		}

		logger::log(ss.str());
	}

	noob::graphics::shader shad;

	noob::graphics::add_sampler("u_texture");

	shad.program = program_handle;
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));


	noob::graphics::add_shader("current", shad);
	sphere = std::unique_ptr<noob::drawable>(new noob::drawable());


	std::string meshFile = *prefix + "/models/sphere.off";

	sphere->set_mesh_file(meshFile);
	sphere->load_mesh();

	noob::graphics::load_texture("test", "blended_textures.tga");
	std::string fontfile = *prefix + "/font/droidsans.ttf";
	droid_font->init(fontfile);
	logger::log("Done init");
}

void noob::application::update(double delta)
{


}

void noob::application::draw()
{
	noob::vec3 at(0.0f, 0.0f, 0.0f);
	noob::vec3 eye(0.0f, 0.0f, -2.5f);
	noob::vec3 up(0.0f, 1.0f, 0.0f);
	noob::mat4 view = noob::look_at(eye, at, up);

	// Compute projection matrix
	if (height == 0) height = 1;
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f);

	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());
	bgfx::setViewTransform(0, &view.m[0], &proj.m[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::TextureHandle tex = noob::graphics::get_texture("test");

	bgfx::setTexture(0, samp.handle, tex);
	noob::graphics::shader s = noob::graphics::get_shader("current");
	sphere->draw(model_mat, s.program);
/*
	droid_font->change_colour(0xFFFF00FF);
	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
	droid_font->change_colour(0xFFFFFFFF);
	droid_font->drawtext(std::string("Font test 2"), 100.0f, 100.0f, (int)width, (int)height);
*/
}
