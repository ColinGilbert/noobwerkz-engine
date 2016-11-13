#pragma once

#include <atomic>
#include <stdint.h>
#include <cmath>
#include <btBulletDynamicsCommon.h>


#include <noob/logger/logger.hpp>

#include "NoobDefines.hpp"
#include "Globals.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "GUI.hpp"
#include "BasicMesh.hpp"
#include "Camera.hpp"
#include "NDOF.hpp"
#include "Stage.hpp"
#include "Keys.hpp"
#include "FileSystem.hpp"
#include "Body.hpp"
#include "RandomGenerator.hpp"
#include "ProfilingInfo.hpp"

#if defined(NOOB_USE_SHINY)
#include "Shiny.h"
#include "ShinyMacros.h"
#endif

namespace noob
{
	class application
	{
		public:
			// Init must always be called.
			void init(uint32_t width, uint32_t height, const std::string filepath);

			void set_archive_dir(const std::string & filepath);
			
			// Those three allow an app to be controlled better by an external environment (ie: Android/iOS)
			void pause();
			void resume();
			void gc();

			typedef std::tuple<const std::array<int, 2>&, const std::array<float,2>&> touch_instance;

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void touch(const touch_instance& t);
			
			void window_resize(uint32_t w, uint32_t h);
			void key_input(char c);

			// TODO: Reimplement
			bool eval(const std::string& name, const std::string& string_to_eval, bool reset = false);

			uint32_t get_height() const { return window_height; }
			uint32_t get_width() const { return window_width; }

			void accept_ndof_data(const noob::ndof::data& info) noexcept(true);

			// step() is called by the target platform. It calculates the delta-time and drives the app's main logic loop.
			void step();

		protected:

			void update(double delta);
			void draw();

			// You these were needed, as See these in sandbox/UserApp.cpp if you're writing a game that is setup or developing the engine in C++.
			bool user_init();
			void user_update(double);

			// TODO: Make a method of noob::globals
			void remove_shapes();

			noob::time last_step;

			noob::profiler_snap current_timings;

			noob::time last_ui_update;

			bool paused, started, input_has_started, ui_enabled;

			uint32_t window_width, window_height;

			noob::gui gui;

			// noob::vec3 eye_pos, eye_target, eye_up;

			std::vector<noob::vec2> finger_positions;

			noob::stage stage;

			std::string script_name;

			std::unique_ptr<std::string> prefix;
	};
}
