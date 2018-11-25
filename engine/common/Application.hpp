#pragma once

#include <noob/logger/logger.hpp>
#include <noob/math/math_funcs.hpp>

#include "Globals.hpp"
#include "Logger.hpp"
#include "LoadFileAsString.hpp"
#include "GUI.hpp"
#include "Camera.hpp"
#include "NDOF.hpp"
#include "Stage.hpp"
#include "Keys.hpp"
#include "Body.hpp"
#include "RandomGenerator.hpp"
#include "ProfilingInfo.hpp"
#include "Database.hpp"

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
			void init(const noob::vec2ui Dims, const noob::vec2d Dpi, const std::string& FilePath);

			void pause();
			void resume();
			void gc();

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void window_resize(const noob::vec2ui);
			void key_input(char c);
			uint32_t get_height() const { return window_dims[0]; }
			uint32_t get_width() const { return window_dims[1]; }

			void accept_ndof_data(const noob::ndof::data& info) noexcept(true);

			// step() is called by the target platform. It calculates the delta-time and drives the app's main logic loop.
			void step();

		protected:

			void update(double delta);
			void draw();

			bool user_init();
			void user_update(double);

			// TODO: Make a method of noob::globals
			void remove_shapes();

			noob::time_point last_step;

			noob::profiler_snap current_timings;

			noob::time_point last_ui_update;

			bool paused, started, input_has_started, ui_enabled;

			noob::vec2ui window_dims;
			noob::vec2d dpi;
			noob::gui app_gui;

			// noob::vec3f eye_pos, eye_target, eye_up;

			std::vector<noob::vec2f> finger_positions;

			noob::stage stage;
			noob::database db;

			std::string script_name;

			std::unique_ptr<std::string> prefix;
	};
}
