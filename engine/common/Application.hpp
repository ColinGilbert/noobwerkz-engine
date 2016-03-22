#pragma once

#include <assert.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//#define PI (float)3.14159265
#include <tuple>
#include <time.h>
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

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "Globals.hpp"
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
#include "TransformHelper.hpp"
#include "Keys.hpp"
#include "format.h"
#include "FileSystem.hpp"
#include "Body.hpp"
#include "RandomGenerator.hpp"


#include <btBulletDynamicsCommon.h>
#include <angelscript.h>
#include "AngelVector.hpp"
#include "scriptstdstring.h"

namespace noob
{
	class application
	{
		public:
			application();
			virtual ~application();
			static application& get();

			void set_init_script(const std::string& name);
			void eval(const std::string& string_to_eval);

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

		protected:

			void update(double delta);
			void draw();

			// Overload these if you're writing a game that is setup and/or developing the engine in C++.
			bool user_init();
			void user_update(double);
			bool load_init_script();

			asIScriptEngine* script_engine;
			asIScriptModule* script_module; //  = engine->GetModule("module", asGM_ALWAYS_CREATE);
			asIScriptContext* script_context;

			static application* app_pointer;
			std::unique_ptr<std::string> prefix;
			bool paused, input_has_started, ui_enabled;
			std::atomic<bool> started;
			uint64_t time;
			uint32_t window_width, window_height;
			noob::gui gui;
			std::vector<noob::vec2> finger_positions;
			noob::voxel_world voxels;
			noob::stage stage;
			noob::mat4 view_mat;
			noob::random_generator randomz;
			std::string script_name;
			noob::globals global_storage;
	};
}
