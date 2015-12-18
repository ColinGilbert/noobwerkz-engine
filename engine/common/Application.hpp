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

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "Logger.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "VoxelWorld.hpp"
#include "GUI.hpp"
#include "BasicMesh.hpp"
#include "Camera.hpp"
#include "NDOF.hpp"
#include "Stage.hpp"
#include "TransformHelper.hpp"
#include "Keys.hpp"
#include "format.h"
//#include "AnimatedModel.hpp"
//#include "ShaderVariant.hpp"
//#include "MeshUtils.hpp"
#include "FileSystem.hpp"

// #include <btBulletDynamicsCommon.h>
// #include <chaiscript/chaiscript.hpp>
// #include <chaiscript/chaiscript_stdlib.hpp>
#include <angelscript.h>

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

			void init();
			void pause();
			void resume();
			
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
			
			// Overload these if you're writing a game that is setup and/or updated in C++.
			void user_init();
			void user_update(double);
			
			asIScriptEngine* script_engine;
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
			std::string script_name;
	};
}
