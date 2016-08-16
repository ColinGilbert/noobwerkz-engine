
#include "Application.hpp"

#include "RegisterScripts.hpp"


noob::application* noob::application::app_pointer = nullptr;

noob::application::application() 
{
	app_pointer = this;
	ready_for_next_script = false;
}


noob::application::~application()
{
	script_engine->ShutDownAndRelease();
	remove_shapes();
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}


void angel_message_callback(const asSMessageInfo *msg, void *param)
{
	std::string message_type;
	switch (msg->type)
	{
		case (asMSGTYPE_WARNING):
			{
				message_type = "WARN";
				break;
			}

		case (asMSGTYPE_INFORMATION):
			{
				message_type = "INFO";
				break;
			}

		default:
			{
				message_type = "ERR";
			}
	}
	fmt::MemoryWriter ww;
	ww << "[AngelScript callback] " << message_type << ": " << msg->message << ", script section: " << msg->section << " row = " << msg->row << " col: " << msg->col;
	noob::logger::log(ww.str());
}


void noob::application::init()
{
	logger::log("[Application] Begin init.");

	started = paused = input_has_started = false;

	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };

	prefix = std::unique_ptr<std::string>(new std::string("./"));

	script_engine = asCreateScriptEngine();

	assert(script_engine > 0);

	// TODO: Uncomment once noob::filesystem is fixed
	// noob::filesystem::init(*prefix);

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);

	controller.set_eye_pos(noob::vec3(0.0, 300.0, -100.0));
	controller.set_eye_target(noob::vec3(0.0, 0.0, 0.0));
	controller.set_eye_up(noob::vec3(0.0, 1.0, 0.0));

	noob::globals& g = noob::globals::get_instance();

	if (g.init())
	{
		stage.init();
	}
	else 
	{
		logger::log("[Application] Global storage init failed :(");
	}

	voxels.init(512, 512, 512);

	// Used by AngelScript to capture the result of the last registration.
	int r;
	// Example: r = script_engine->RegisterGlobalFunction("", asFUNCTION(func_name), asCALL_CDECL);
	script_engine->SetMessageCallback(asFUNCTION(angel_message_callback), 0, asCALL_CDECL);

	RegisterStdString(script_engine);

	// Does not work! TODO: Report on forums.
	// RegisterVector<bool>("bool", script_engine);

	// Neither those two seem to work. They register just fine but there's literally no output to the commandline.
	r = script_engine->RegisterGlobalFunction("void log(const string& in)", asFUNCTION(logger::log), asCALL_CDECL); assert (r >= 0);
	// r = script_engine->RegisterGlobalFunction("void log(const string& in)", asFUNCTIONPR(logger::log, (const std::string&), void), asCALL_CDECL); assert (r >= 0);

	register_handles(script_engine);
	register_math(script_engine);
	register_plane(script_engine);
	register_graphics(script_engine);
	register_controls(script_engine);
	r = script_engine->RegisterGlobalProperty("controls controller", &controller); assert (r >= 0);
	register_basic_mesh(script_engine);
	register_active_mesh(script_engine);
	r = script_engine->RegisterGlobalFunction("double random()", asMETHOD(noob::random_generator, get), asCALL_THISCALL_ASGLOBAL, &randomz); assert( r >= 0 );
	register_body(script_engine);
	register_joint(script_engine);
	// register_ghost(script_engine);
	register_light(script_engine);
	register_reflectance(script_engine);
	register_actor(script_engine);
	register_uniforms(script_engine);
	register_scaled_model(script_engine);
	register_stage(script_engine);
	r = script_engine->RegisterGlobalProperty("stage default_stage", &stage); assert (r >= 0);
	register_voxels(script_engine);
	r = script_engine->RegisterGlobalProperty("voxel_world voxels", &voxels); assert (r >= 0);

	register_globals(script_engine);

	logger::log("[Application] Done basic init.");

	bool b = user_init();

	ready_for_next_script = b;

	if (!b) 
	{
		logger::log("[Application] User C++ init failed!");
	}

	network.init(3);
	network.connect("localhost", 4242);

}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);

	network.tick();

	while (network.has_message())
	{
		std::string s = network.get_message();
		if (s.compare(0, 6, "INIT: ") == 0)
		{
			stage.tear_down();
			eval("init", s.substr(6, std::string::npos), true);
		}
		else if (s.compare(0, 8, "UPDATE: ") == 0)
		{
			// TODO: Implement
		}
		else if (s.compare(0, 5, "CMD: ") == 0)
		{
			eval("cmd", s.substr(5, std::string::npos), true);
		}
	}

	stage.update(delta);
	user_update(delta);
}


bool noob::application::eval(const std::string& name, const std::string& string_to_eval, bool reset)
{
	// PROFILE_END();
	// PROFILE_BEGIN(loading);

	noob::time prior_to_loading = noob::clock::now();


	logger::log("\n[Application] Begin loading script...");
	fmt::MemoryWriter to_log;
	to_log << "[Application] Script loaded? ";

	if (ready_for_next_script)
	{
		if (reset)
		{
			script_module = script_engine->GetModule(0, asGM_ALWAYS_CREATE);
		}
		else
		{
			script_module = script_engine->GetModule(0, asGM_CREATE_IF_NOT_EXISTS);
		}

		int r = script_module->AddScriptSection(name.c_str(), string_to_eval.c_str());
		if (r < 0)
		{
			to_log << "False - Add section failed.";
			logger::log(to_log.str());
			ready_for_next_script = true;
			return false;
		}

		r = script_module->Build();
		if (r < 0 )
		{

			to_log << "False - Compile failed.";
			logger::log(to_log.str());
			ready_for_next_script = true;
			return false;
			// The build failed. The message stream will have received  
			// compiler errors that shows what needs to be fixed
		}
		asIScriptContext* ctx = script_engine->CreateContext();
		if (ctx == 0)
		{

			to_log << "False - Failed to create context.";
			logger::log(to_log.str());
		}

		asIScriptFunction* func = script_engine->GetModule(0)->GetFunctionByDecl("void main()");

		if (func == 0)
		{
			to_log << "False - Function main() not found.";
			logger::log(to_log.str());
			ready_for_next_script = true;
			return false;
		}

		r = ctx->Prepare(func);

		if (r < 0) 
		{

			to_log << "False - Failed to prepare the context.";
			logger::log(to_log.str());
			ready_for_next_script = true;
			return false;
		}

		r = ctx->Execute();

		if (r != asEXECUTION_FINISHED)
		{
			// The execution didn't finish as we had planned. Determine why.
			if (r == asEXECUTION_ABORTED)
			{
				to_log << "False - Script aborted.";
				logger::log(to_log.str());
			}

			else if (r == asEXECUTION_EXCEPTION)
			{
				to_log << "False - Script ended with an exception: ";

				asIScriptFunction* exception_func = ctx->GetExceptionFunction();

				to_log << "function: " << exception_func->GetDeclaration() << ", ";
				to_log << "module: " << exception_func->GetModuleName() << ", ";
				to_log << "section: " << exception_func->GetScriptSectionName() << ", ";
				to_log << "line: " << ctx->GetExceptionLineNumber() << ", ";
				to_log << "description: " << ctx->GetExceptionString();

				logger::log(to_log.str());
			}

			else
			{
				to_log << "False - Ended with reason code: " << r;
				logger::log(to_log.str());
			}

			ready_for_next_script = true;
			return false;
		}


		noob::time post_loading = noob::clock::now();
		noob::duration loading_time = post_loading - prior_to_loading;

		to_log << "True. Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(loading_time).count() << " milliseconds (" << std::chrono::duration_cast<std::chrono::microseconds>(loading_time).count() << " microseconds)\n";
		logger::log(to_log.str());

		// PROFILE_END();
		// PROFILE_BEGIN(runtime);

		return true;
	}
	else 
	{
		return false;
	}
}

void noob::application::draw()
{
	noob::mat4 proj_mat = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 1.0, 2000.0);
	stage.draw(window_width, window_height, controller.get_eye_pos(), controller.get_eye_target(), controller.get_eye_up(), proj_mat);
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		// float damping = 360.0;
		// noob::vec3 rotation(info.rotation);
		// noob::vec3 translation(info.translation);
		// view_mat = noob::rotate_x_deg(stage.view_mat, -rotation[0]/damping);
		// view_mat = noob::rotate_y_deg(stage.view_mat, -rotation[1]/damping);
		// view_mat = noob::rotate_z_deg(stage.view_mat, -rotation[2]/damping);
		// view_mat = noob::translate(stage.view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
		// stage.eye_pos = stage.eye_pos - translation;//translation);
	}
}


// TODO: Refactor
void noob::application::step()
{
	// PROFILER_UPDATE();
	noob::time start_time = noob::clock::now();
	noob::duration time_since = last_step - start_time;

	if (!paused)
	{
		double d = (1.0 / static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(time_since).count()));
		update(d);
	}

	draw();

	noob::time end_time = noob::clock::now();
	last_step = end_time;
	noob::duration time_taken = end_time - start_time;

	noob::globals& g = noob::globals::get_instance();
	g.profile_run.total_time += time_taken;
}


void noob::application::pause()
{
	paused = true;
}


void noob::application::resume()
{
	paused = false;
}


void noob::application::set_archive_dir(const std::string& filepath)
{
	logger::log(fmt::format("[Application] Setting archive directory (\"{0}\")", filepath));
	prefix = std::unique_ptr<std::string>(new std::string(filepath));
	logger::log(fmt::format("[Application] Archive dir = {0}", *prefix));
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		fmt::MemoryWriter w;
		w << "[Application] Touch - pointer ID = " << pointerID << ", " << x << ", " << y << ", action = " << action;
		logger::log(w.str());

		if (pointerID < 3)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}


void noob::application::window_resize(uint32_t w, uint32_t h)
{
	window_width = w;
	window_height = h;
	if (window_height == 0) 
	{
		window_height = 1;
	}

	fmt::MemoryWriter ww;
	ww << "[Application] Resize window to (" << window_width << ", " << window_height << ")";
	logger::log(ww.str());
}


void noob::application::key_input(char c)
{

}

void noob::application::remove_shapes()
{
	noob::globals& g = noob::globals::get_instance();
	for (size_t i = 0; i < g.shapes.items.size(); ++i)
	{
		if (g.shapes.items[i].physics_valid)
		{
			delete g.shapes.items[i].inner_shape;
		}
	}
}


/*
   std::string noob::application::get_profiler_text()
   {
   std::ostringstream profile_out;
   PROFILER_OUTPUT(profile_out);
   return profile_out.str();
// *profiler_text = output_profiling();
// logger::log(s);
//logger::log(profile_out.str());
}

void noob::application::output_profiling()
{
logger::log(get_profiler_text());//PROFILER_OUTPUT());
}

*/
