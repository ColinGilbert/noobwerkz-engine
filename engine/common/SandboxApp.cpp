#include "EditorUtils.hpp"
#include "Application.hpp"

void noob::application::init()
{

	logger::log("");

	noob::editor_utils::blend_channels();

	// noob::graphics::pos_norm_uv_bones_vertex::init();
	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_basic", "fs_basic");
	
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

	noob::graphics::programs.insert(std::make_pair(std::string("basic"), program_handle));

	sphere = std::unique_ptr<noob::drawable>(new noob::drawable());

	std::string meshFile = *prefix + "/models/sphere.off";

	sphere->set_mesh_file(meshFile);
	sphere->load_mesh();

	std::string fontfile = *prefix + "/font/droidsans.ttf";
	droid_font->init(fontfile);
	logger::log("Done init");
}

void noob::application::update(double delta)
{


}

void noob::application::draw()
{
	// Compute view matrix
	noob::vec3 at(0.0f, 0.0f, 0.0f);
	noob::vec3 eye(0.0f, 0.0f, -2.5f);
	noob::vec3 up(0.0f, 1.0f, 0.0f);
	noob::mat4 view = noob::look_at(eye, at, up);

	// Compute projection matrix
	if (height == 0) height = 1;
	noob::mat4 proj = noob::perspective(60.0f, (float)width/(float)height, 0.1f, 100.f);
	
	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());
//	noob::mat4 model_view_mat = view * model_mat;
	bgfx::setViewTransform(0, &view.m[0], &proj.m[0]);


	// bgfx::setProgram(noob::graphics::programs.find("basic")->second);
	bgfx::ProgramHandle prog = noob::graphics::programs.find("basic")->second;
		

//	droid_font->change_colour(0xFFFF00FF);
//	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
//	droid_font->change_colour(0xFFFFFFFF);
//	droid_font->drawtext(std::string("Font test 2"), 100.0f, 100.0f, (int)width, (int)height);

	sphere->draw(model_mat, prog);
}
