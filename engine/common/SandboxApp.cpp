#include <atomic>
#include <array>

#include <bx/timer.h>
#include <bx/string.h>
#include <bx/fpumath.h>

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "EditorUtils.hpp"
#include "Application.hpp"

long int fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}

TrueTypeHandle loadTtf(FontManager* _fm, const char* _filePath)
{
	FILE* file = fopen(_filePath, "rb");
	if (NULL != file)
	{
		uint32_t size = (uint32_t)fsize(file);
		uint8_t* mem = (uint8_t*)malloc(size+1);
		size_t ignore = fread(mem, 1, size, file);
		// BX_UNUSED(ignore);
		fclose(file);
		mem[size-1] = '\0';
		TrueTypeHandle handle = _fm->createTtf(mem, size);
		free(mem);
		return handle;
	}
	TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
	return invalid;
}

void noob::application::init()
{

	logger::log("");

	// TODO: Move out of editor program
	// noob::editor_utils::blend_channels();

	ui_enabled = true;

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

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Uniform4fv);
	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Uniform3fv);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Uniform2fv);
	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Uniform3fv);

	noob::graphics::shader shad;
	shad.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("current", shad);
	noob::graphics::load_texture("grad_map", "gradient_map.dds");

	std::string fontfile_path = *prefix + "/font/droidsans.ttf";

	fontManager = new FontManager(1024);
	textBufferManager = new TextBufferManager(fontManager);

	ttf_handle = loadTtf(fontManager, fontfile_path.c_str());
	font_handle = fontManager->createFontByPixelSize(ttf_handle, 0, 32);

	fontManager->preloadGlyph(font_handle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!1234567890-=_+[]{}!@#$%^&*()`~,<>/?\\'\";:. \n");

	fontManager->destroyTtf(ttf_handle);

	transientText = textBufferManager->createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient);

	// droid_font->init(fontfile, 32, window_width, window_height);
	// droid_font->change_colour(0xFFFFFFFF);
	vox_world.init();

/*	
	   noob::voxel_world::region cube_reg;
	   cube_reg.lower_x = 70;
	   cube_reg.lower_y = 70;
	   cube_reg.lower_z = 70;
	   cube_reg.upper_x = 140;
	   cube_reg.upper_y = 140;
	   cube_reg.upper_z = 140;
*/

	   vox_world.sphere(50, 50, 50, 50, noob::voxel_world::op_type::ADD);

	   noob::voxel_world::region reg;
	   reg.lower_x = 0;
	   reg.lower_y = 0;
	   reg.lower_z = 0;
	   reg.upper_x = 255;
	   reg.upper_y = 255;
	   reg.upper_z = 255;

	vox_world.extract_region(reg, "models/test.off");

	sphere = std::unique_ptr<noob::drawable>(new noob::drawable());
	std::string meshFile = *prefix + "/models/test.off";

	sphere->set_mesh_file(meshFile);
	sphere->load_mesh();
	logger::log("Done init");
}

void noob::application::update(double delta)
{
	if (started == false)
	{


	}
	else started = true;
}

void noob::application::draw()
{

	// bgfx::setState(BGFX_STATE_BLEND_INDEPENDENT | BGFX_STATE_DEFAULT);
	// bgfx::setState(BGFX_STATE_BLEND_SCREEN | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_LESS);
		
	noob::vec3 cam_up(0.0, 1.0, 0.0);
	noob::vec3 cam_target(0.0, 0.0, 0.0);
	noob::vec3 cam_pos(180.0, 180.0, 400.0);
	noob::mat4 view_mat = noob::look_at(cam_pos, cam_target, cam_up);
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);
	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	float at[3] = { 0, 0, 0.0f };
	float eye[3] = { 0, 0, -1.0f };
	float view[16];
	bx::mtxLookAt(view, eye, at);
	const float centering = 0.5f;
	float ortho[16];
	bx::mtxOrtho(ortho, centering, window_width + centering, window_height + centering, centering, -1.0f, 1.0f);
	bgfx::setViewTransform(1, view, ortho);
	bgfx::setViewRect(1, 0, 0, window_width, window_height);


	
	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());

	noob::vec4 colour_1(0.0, 0.3, 0.3, 1.0);
	noob::vec4 colour_2(0.0, 0.5, 0.5, 1.0);
	noob::vec4 colour_3(0.3, 0.3, 0.3, 1.0);
	noob::vec4 colour_4(0.2, 0.6, 0.4, 1.0);
	noob::vec3 mapping_blend(0.0, 2.0, 1.0);
	noob::vec2 colour_positions(0.3, 0.6);
	noob::vec3 scales(4, 4, 4);

	bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &colour_1.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &colour_2.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &colour_3.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &colour_4.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &mapping_blend.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_positions.v[0]);
	bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scales.v[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("current");
	sphere->draw(0, model_mat, s.program);
	bgfx::submit(0);


	

	textBufferManager->clearTextBuffer(transientText);
	textBufferManager->setPenPosition(transientText, 150.0f, 10.0f);
	textBufferManager->appendText(transientText, font_handle, L"Transient\n");
	textBufferManager->appendText(transientText, font_handle, L"text buffer\n");
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(transientText, 1);
	bgfx::submit(1);

}
