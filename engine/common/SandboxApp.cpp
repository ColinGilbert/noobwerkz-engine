#include <atomic>
#include <array>

#include <bx/timer.h>
#include <bx/string.h>
#include <bx/fpumath.h>

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
/*
#include <RuntimeCompiler/AUArray.h>
#include <RuntimeCompiler/BuildTool.h>
#include <RuntimeCompiler/ICompilerLogger.h>
#include <RuntimeCompiler/FileChangeNotifier.h>
#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.h>
#include <RuntimeObjectSystem/RuntimeObjectSystem.h>
#include <RuntimeObjectSystem/IObject.h>

#include "StdioLogSystem.h"

#include "EditorUtils.hpp"
#include "Application.hpp"
*/
#include "AppState.h"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "UIFont.hpp"
#include "World.hpp"

/*
bool init()
{
	logger::log("");
	//Initialise the RuntimeObjectSystem
	m_pRuntimeObjectSystem = new RuntimeObjectSystem;
	m_pCompilerLogger = new StdioLogSystem(); //ew StdioLogSystem();
	if( !m_pRuntimeObjectSystem->Initialise(m_pCompilerLogger, 0) )
	{
		m_pRuntimeObjectSystem = 0;
		return false;
	}
	m_pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);


	// construct first object
	IObjectConstructor* pCtor = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor( "AppState" );
	if( pCtor )
	{
		IObject* pObj = pCtor->Construct();
		pObj->GetInterface( &app_state );
		if( 0 == app_state )
		{
			delete pObj;
			m_pCompilerLogger->LogError("Error - no updateable interface found\n");
			return false;
		}
		m_ObjectId = pObj->GetObjectId();

	}
*/

bool AppState::init()
{
	// TODO: Move out of editor program
	// noob::editor_utils::blend_channels();
	noob::data = std::unique_ptr<noob:user_data>(new noob::user_data);
	
	noob::data->droid_font = std::unique_ptr<noob:ui_font>(new noob::ui_font());
	noob::data->droid_font->init("fonts/droidsans.tff");

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

	noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_4"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("mapping_blend"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("colour_positions"), bgfx::UniformType::Enum::Vec4);
	noob::graphics::add_uniform(std::string("scales"), bgfx::UniformType::Enum::Vec4);

	noob::graphics::shader shad;
	shad.program = program_handle;

	noob::graphics::add_sampler("u_texture");
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));

	noob::graphics::add_shader("current", shad);
	noob::graphics::load_texture("grad_map", "gradient_map.dds");

	data.droid_font = std::unique_ptr<noob::ui_font>(new noob::ui_font());

	std::string fontfile_path = *prefix + "/font/droidsans.ttf";
	/*
	   fontManager = new FontManager(1024);
	   textBufferManager = new TextBufferManager(fontManager);

	   ttf_handle = loadTtf(fontManager, fontfile_path.c_str());
	   font_handle = fontManager->createFontByPixelSize(ttf_handle, 0, 32);

	   fontManager->preloadGlyph(font_handle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!1234567890-=_+[]{}!@#$%^&*()`~,<>/?\\'\";:. \n");

	   fontManager->destroyTtf(ttf_handle);

	   transientText = textBufferManager->createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient);
	 */

	data.droid_font->init(fontfile_path, 32, window_width, window_height);
	data.droid_font->set_colour(0xFFFFFFFF);
	data.world.init();

	/*	
		noob::world::cubic_region cube_reg;
		cube_reg.lower_x = 70;
		cube_reg.lower_y = 70;
		cube_reg.lower_z = 70;
		cube_reg.upper_x = 140;
		cube_reg.upper_y = 140;
		cube_reg.upper_z = 140;
		*#/

	noob::world::cubic_region reg;
	reg.lower_x = 0;
	reg.lower_y = 0;
	reg.lower_z = 0;
	reg.upper_x = 255;
	reg.upper_y = 255;
	reg.upper_z = 255;
*/
	data.sphere = std::unique_ptr<noob::drawable>(new noob::drawable());
	std::string meshFile = *prefix + "/models/test.off";

	data.sphere->set_mesh_file(meshFile);
	data.sphere->load_mesh();
	logger::log("Done init");
	return true;
	return true;
}

void AppState::update(double delta)
{
/*
	//check status of any compile
	if( m_pRuntimeObjectSystem->GetIsCompiledComplete() )
	{
		// load module when compile complete
		m_pRuntimeObjectSystem->LoadCompiledModule();
	}

	if( !m_pRuntimeObjectSystem->GetIsCompiling() )
	{
		//		logger::log("RuntimeCompiledCPP test. Woot.");
	}
*/
	droid_font->set_window_dims(window_width, window_height);
	if (started == false)
	{

	}
	else started = true;
}


void AppState::draw(float width, float height, double delta)
{
	noob::vec3 cam_up(0.0, 1.0, 0.0);
	noob::vec3 cam_target(0.0, 0.0, 0.0);
	noob::vec3 cam_pos(180.0, 180.0, 400.0);
	noob::mat4 view_mat = noob::look_at(cam_pos, cam_target, cam_up);
	noob::mat4 proj = noob::perspective(60.0f, data.window_width / data.window_height, 0.1f, 2000.0f);
	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, data.window_width, data.window_height);
	/*
	   float at[3] = { 0, 0, 0.0f };
	   float eye[3] = { 0, 0, -1.0f };
	   float view[16];
	   bx::mtxLookAt(view, eye, at);
	   const float centering = 0.5f;
	   float ortho[16];
	   bx::mtxOrtho(ortho, centering, window_width + centering, window_height + centering, centering, -1.0f, 1.0f);
	   bgfx::setViewTransform(1, view, ortho);
	   bgfx::setViewRect(1, 0, 0, window_width, window_height);
	   */
	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());

	noob::vec4 colour_1(0.0, 0.3, 0.3, 1.0);
	noob::vec4 colour_2(0.0, 0.5, 0.5, 1.0);
	noob::vec4 colour_3(0.3, 0.3, 0.3, 1.0);
	noob::vec4 colour_4(0.2, 0.6, 0.4, 1.0);
	noob::vec4 mapping_blend(0.0, 2.0, 1.0, 0.0);
	noob::vec4 colour_positions(0.3, 0.6, 0.0, 0.0);
	noob::vec4 scales(4, 4, 4, 0);

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
	data.sphere->draw(0, model_mat, s.program);
	bgfx::submit(0);


	data.droid_font->draw_text(1, "The goat stumbled upon the cheese", 150.0, 50.0);
	/*
	   textBufferManager->clearTextBuffer(transientText);
	   textBufferManager->setPenPosition(transientText, 150.0f, 10.0f);
	   textBufferManager->appendText(transientText, font_handle, L"Transient\n");
	   textBufferManager->appendText(transientText, font_handle, L"text buffer\n");
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(transientText, 1);
	bgfx::submit(1);
	*/
}

