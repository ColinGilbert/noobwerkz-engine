#pragma once

#include <assert.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//#define PI (float)3.14159265
#include <tuple>
// #include <time.h>
#include <assert.h>
#include <memory>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <map>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <atomic>

#include <btBulletDynamicsCommon.h>
#include <angelscript.h>

#include "NoobDefines.hpp"
#include "Globals.hpp"
#include "Controls.hpp"
#include "Logger.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "VoxelWorld.hpp"
#include "GUI.hpp"
#include "BasicMesh.hpp"
#include "ActiveMesh.hpp"
#include "Camera.hpp"
#include "NDOF.hpp"
#include "Stage.hpp"
#include "Keys.hpp"
#include "format.h"
#include "FileSystem.hpp"
#include "Body.hpp"
#include "RandomGenerator.hpp"
#include "NetworkClient.hpp"
#include "ProfilingInfo.hpp"


#include "AngelVector.hpp"
#include "scriptstdstring.h"

#include "Shiny.h"
#include "ShinyMacros.h"

namespace noob
{
	class application final
	{
		public:
			application();
			~application();
			static application& get();


			bool eval(const std::string& name, const std::string& string_to_eval, bool reset = false);

			uint32_t get_height() const { return static_cast<uint32_t>(window_height); }
			uint32_t get_width() const { return static_cast<uint32_t>(window_width); }

			// Init must always be called.
			void init();

			// Those three allow an app to be controlled better by an external environment (ie: Android/iOS)
			void pause();
			void resume();
			void gc();

			typedef std::tuple<const std::array<int, 2>&, const std::array<float,2>&> touch_instance;

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void touch(const touch_instance& t);
			void set_archive_dir(const std::string & filepath);
			void window_resize(uint32_t w, uint32_t h);
			void key_input(char c);

			void accept_ndof_data(const noob::ndof::data& info);

			// step() is called by the target platform, which calculates the delta-time and drives the update() function. It is an ugly hack that had to be made public in order to be callable from the main app.
			void step();

			noob::controls controller;

		protected:

			static application* app_pointer;

			void update(double delta);
			void draw();

			// See these in sandbox/UserApp.cpp if you're writing a game that is setup or developing the engine in C++.
			bool user_init();
			void user_update(double);

			void remove_shapes();

			noob::time last_step;

			std::atomic<bool> ready_for_next_script;
			asIScriptEngine* script_engine;
			asIScriptModule* script_module;
			asIScriptContext* script_context;

			noob::profiler_snap current_timings;

			noob::time last_ui_update;

			bool paused, started, input_has_started, ui_enabled;

			// uint64_t time;

			uint32_t window_width, window_height;

			noob::gui gui;

			std::vector<noob::vec2> finger_positions;

			noob::voxel_world voxels;

			noob::stage stage;

			noob::network_client network;

			noob::random_generator randomz;

			std::string script_name;

			std::unique_ptr<std::string> prefix;
	};
}
