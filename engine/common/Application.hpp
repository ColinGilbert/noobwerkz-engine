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

#include "Logger.hpp"
#include "Untracked.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "World.hpp"
#include "UIFont.hpp"

#include <bgfx.h>

#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectInterface.h>
#include <RuntimeCompiler/AUArray.h>

#include "StdioLogSystem.h"
#include "AppState.h"
#include "InterfaceIds.h"

namespace noob
{
	class app : public IObjectFactoryListener
	{

		public:
			app();
			virtual ~app();
			static app& get();

			uint32_t get_height() const { return static_cast<uint32_t>(window_height); }
			uint32_t get_width() const { return static_cast<uint32_t>(window_width); }
			
			// May be removed soon
			bool has_started() const { return started; }



			// These three functions are played with by user (aka: programmer)
			bool init();
			void update(double delta);
			// void update(double delta);
			// void draw(float width, float height, double delta);

			// step() is called by the target platform every frame while pause() and resume() are used by whoever needs them :P
			void step();
			void pause();
			void resume();

			// Callbacks
			void touch(int pointerID, float x, float y, int action);
			void set_archive_dir(const std::string & filepath);
			void window_resize(uint32_t w, uint32_t h);

			virtual void OnConstructorsAdded();

		protected:
			static app* app_pointer;
			std::unique_ptr<std::string> prefix;
			bool paused, input_has_started, ui_enabled;
			std::atomic<bool> started;
			//std::unique_ptr<noob::ui_font> droid_font;
			uint64_t time;
			float window_width, window_height;
			std::vector<noob::vec2> finger_positions;
			// std::unique_ptr<noob::drawable> sphere;
			noob::world world;
			ICompilerLogger* m_pCompilerLogger;
			IRuntimeObjectSystem* m_pRuntimeObjectSystem;
			AppState* app_state;
			ObjectId m_ObjectId;
	};
}
