#pragma once

#include <assert.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define PI (float)3.14159265

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

/*
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
*/

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

#include "Server.hpp"
#include "Logger.hpp"
#include "NoobUtils.hpp"
#include "Untracked.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "VoxelWorld.hpp"
#include "GUI.hpp"
#include "PhysicsWorld.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "NDOF.hpp"
#include "Scene.hpp"
#include "TransformHelper.hpp"
// #include "Noise.h"

//#include "nanovg/nanovg.h"
#include <bgfx.h>
#include "format.h"

//#include "TriPlanar.hpp"
#include "HACDRenderer.hpp"


#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

namespace noob
{
	class application
	{
		public:
			application();
			virtual ~application();
			static application& get();

			uint32_t get_height() const { return static_cast<uint32_t>(window_height); }
			uint32_t get_width() const { return static_cast<uint32_t>(window_width); }

			void init();
			void update(double delta);
			void draw();
			
			// step() is called by the target platform, which calculates the delta-time and calls update().
			// pause() and resume() are used by whoever needs them :P
			void step();
			void pause();
			void resume();

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void set_archive_dir(const std::string & filepath);
			void window_resize(uint32_t w, uint32_t h);
			void character_input(char c);
			void accept_ndof_data(const noob::ndof::data& info);

		protected:
			static application* app_pointer;
			std::unique_ptr<std::string> prefix;
			bool paused, input_has_started, ui_enabled;
			std::atomic<bool> started;
			uint64_t time;
			uint32_t window_width, window_height;
			noob::gui gui;
			std::vector<noob::vec2> finger_positions;
			noob::voxel_world voxels;
			noob::physics_world physics;
			noob::scene scene;
			noob::mat4 view_mat;
			noob::transform_helper current_cam_transform;
			chaiscript::ChaiScript chai;
			lemon::StaticDigraph keystrokes_graph;
	};
}
