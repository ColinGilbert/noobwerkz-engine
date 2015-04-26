#ifndef APPLICATION_H_
#define APPLICATION_H_

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

/*
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
*/

#include "Logger.hpp"
#include "Untracked.hpp"
#include "MathFuncs.hpp"
#include "Camera.hpp"

namespace noob
{
	class application
	{
		public:
			application();
			virtual ~application();
			static application& get();

			uint32_t get_height() const { return static_cast<uint32_t>(height); }
			uint32_t get_width() const { return static_cast<uint32_t>(width); }
			void touch(int pointerID, float x, float y, int action);
			void set_archive_dir(const std::string & filepath);

			// This may soon be changed/removed from API
			const std::unique_ptr<noob::camera> cam;
		
			void draw(double delta);
			void update(double delta);
			void step();
			void pause();
			void resume();

			void window_resize(int w,int h);
		protected:
	
			//These two functions are played with by user.

		
			void init_graphics();
			void update_cam(double delta);
			

			void init();


			static application* app_pointer;
			std::unique_ptr<std::string> prefix;
			bool paused, input_has_started;
			uint64_t time;
			float width, height;
			noob::mat4 proj_matrix;
			std::vector<noob::vec2> finger_positions;
	};
}
#endif
