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

#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include "font_manager.h"
#include "text_buffer_manager.h"

#include "Logger.hpp"
#include "Untracked.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "Voxels.hpp"
// #include "Font.hpp"

#include "nanovg/nanovg.h"
#include <bgfx.h>

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

			// These three functions are played with by user (aka: programmer)
			void init();
			void update(double delta);
			void draw();

			// step() is called by the target platform every frame while pause() and resume() are used by whoever needs them :P
			void step();
			void pause();
			void resume();

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void set_archive_dir(const std::string & filepath);
			void window_resize(uint32_t w, uint32_t h);

		protected:
			static application* app_pointer;
			std::unique_ptr<std::string> prefix;
			bool paused, input_has_started, ui_enabled;
			std::atomic<bool> started;
			// std::unique_ptr<noob::font> droid_font;
			uint64_t time;
			uint32_t window_width, window_height;
			std::vector<noob::vec2> finger_positions;
			std::unique_ptr<noob::drawable> sphere;
			noob::universe world;
			
			FontManager* fontManager; // = new FontManager(512);
			TextBufferManager* textBufferManager; // = new TextBufferManager(fontManager);
			TextBufferHandle transientText;
			TrueTypeHandle ttf_handle;
			FontHandle font_handle;

			NVGcontext* nvg;
	};
}
