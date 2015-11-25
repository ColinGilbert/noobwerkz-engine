#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;

noob::application::application() 
{
	logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("./"));
	script_engine = asCreateScriptEngine();
	if (script_engine == 0) logger::log("[ERROR]: Failed to create script engine.");
	set_init_script("init.as");
	view_mat = noob::look_at(noob::vec3(0, 50.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
}	


noob::application::~application()
{
	logger::log("~application()");
	script_engine->ShutDownAndRelease();
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}

void noob::application::set_init_script(const std::string& name)
{
	script_name = name;
}


void noob::application::eval(const std::string& string_to_eval)
{

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
	ww << msg->section << ", " << msg->row << ", " << msg->col << ", " << message_type << ", " << msg->message;
	logger::log(ww.str());
}


void noob::application::init()
{
	logger::log("");

	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	voxels.init();
	stage.init();

	//r = script_engine->RegisterGlobalFunction("", asFUNCTION(func_name), asCALL_CDECL);
	script_engine->SetMessageCallback(asFUNCTION(angel_message_callback), 0, asCALL_CDECL);

	logger::log("[Application] done init.");
	user_init();
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);
	stage.update(delta);
	user_update(delta);

	static double time_elapsed = 0.0;
	time_elapsed += delta;


	// TODO: Replace with AngelScript
	if (time_elapsed > 0.25)
	{
		boost::filesystem::path p;

		p += *prefix;
		p += script_name;

		boost::system::error_code ec;

		static std::time_t last_write = 0;
		std::time_t t = boost::filesystem::last_write_time(p, ec);
		if (ec != 0)
		{
			logger::log(fmt::format("[Application] - update() - error reading {0}: {1}", p.generic_string(), ec.message()));
		}	
		else if (last_write != t)
		{
			init();

			try
			{
				// chai->eval_file(p.generic_string());
			}
			catch(std::exception e)
			{
				logger::log(fmt::format("[Application]. Caught AngelScript exception: ", e.what()));
			}
			last_write = t;
		}
		time_elapsed = 0.0;
	}
}


void noob::application::draw()
{

	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	stage.draw();

	// gui.text("The goat stumbled upon the cheese", 150.0, 50.0);
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		float damping = 360.0;
		noob::vec3 rotation(info.rotation);
		noob::vec3 translation(info.translation);
		view_mat = noob::rotate_x_deg(view_mat, -rotation[0]/damping);
		view_mat = noob::rotate_y_deg(view_mat, -rotation[1]/damping);
		view_mat = noob::rotate_z_deg(view_mat, -rotation[2]/damping);
		view_mat = noob::translate(view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
	}
}


void noob::application::step()
{
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	uint64_t uNowNano = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	double delta = (uNowNano - time) * 0.000000001f;

	time = uNowNano;

	if (!paused)
	{
		update(delta);
	}

	draw();
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
