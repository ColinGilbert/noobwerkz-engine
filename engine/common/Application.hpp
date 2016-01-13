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

#include "GlobalComponents.hpp"
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
#include "FileSystem.hpp"
#include "Body.hpp"

#include <btBulletDynamicsCommon.h>
#include <angelscript.h>

#include "AngelVector.hpp"
#include "scriptstdstring.h"
//#include "scripthelper.h"

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

			static void as_stage_constructor_wrapper(uint8_t* memory)
			{
				new(memory) noob::stage();
			}

			static void as_stage_destructor_wrapper(uint8_t* memory)
			{
				((noob::stage*)memory)->~stage();
			}

			static void as_basic_mesh_constructor_wrapper(uint8_t* memory)
			{
				new(memory) noob::basic_mesh();
			}

			static void as_basic_mesh_destructor_wrapper(uint8_t* memory)
			{
				((noob::basic_mesh*)memory)->~basic_mesh();
			}

			static void as_vec2_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::vec2();
			}
			
			static void as_vec2_constructor_wrapper_float_2(uint8_t* memory, float x, float y)
			{
				new(memory) noob::vec2(x, y);
			}

			static void as_vec2_destructor_wrapper(uint8_t* memory)
			{
				((noob::vec2*)memory)->~vec2();
			}

			static void as_vec3_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::vec3();
			}
		
			static void as_vec3_constructor_wrapper_float_3(uint8_t* memory, float x, float y, float z)
			{
				new(memory) noob::vec3(x, y, z);
			}

			static void as_vec3_constructor_wrapper_vec2_float(uint8_t* memory, const noob::vec2& vv,  float z)
			{
				new(memory) noob::vec3(vv, z);
			}

			static void as_vec3_constructor_wrapper_vec4(uint8_t* memory, const noob::vec4& vv)
			{
				new(memory) noob::vec3(vv);
			}

			static void as_vec3_constructor_wrapper_vec3(uint8_t* memory, const noob::vec3& vv)
			{
				new(memory) noob::vec3(vv);
			}

			static void as_vec3_destructor_wrapper(uint8_t* memory)
			{
				((noob::vec3*)memory)->~vec3();
			}

			static void as_vec4_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::vec4();
			}
		
			static void as_vec4_constructor_wrapper_float_4(uint8_t* memory, float x, float y, float z, float w)
			{
				new(memory) noob::vec4(x, y, z, w);
			}

			static void as_vec4_constructor_wrapper_vec2_float_2(uint8_t* memory, const noob::vec2& vv,  float z, float w)
			{
				new(memory) noob::vec4(vv, z, w);
			}

			static void as_vec4_constructor_wrapper_vec3_float(uint8_t* memory, const noob::vec3& vv, float w)
			{
				new(memory) noob::vec4(vv, w);
			}

			static void as_vec4_destructor_wrapper(uint8_t* memory)
			{
				((noob::vec4*)memory)->~vec4();
			}

			static void as_mat3_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::mat3();
			}
			
			static void as_mat3_constructor_wrapper_float_9(uint8_t* memory, float a, float b, float c, float d, float e, float f, float g, float h, float i)
			{
				new(memory) noob::mat3(a, b, c, d, e, f, g, h, i);
			}

			static void as_mat3_destructor_wrapper(uint8_t* memory)
			{
				((noob::mat3*)memory)->~mat3();
			}

			static void as_mat4_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::mat4();
			}

			static void as_mat4_constructor_wrapper_float_16(uint8_t* memory, float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float mm, float n, float o, float p)
			{
				new(memory) noob::mat4(a, b, c, d, e, f, g, h, i, j, k, l, mm, n, o, p);
			}

			static void as_mat4_destructor_wrapper(uint8_t* memory)
			{
				((noob::mat4*)memory)->~mat4();
			}

			static void as_versor_constructor_wrapper_basic(uint8_t* memory)
			{
				new(memory) noob::versor();
			}

			static void as_versor_constructor_wrapper_float_4(uint8_t* memory, float x, float y, float z, float w)
			{
				new(memory) noob::versor(x, y, z, w);
			}

			static void as_versor_destructor_wrapper(uint8_t* memory)
			{
				((noob::versor*)memory)->~versor();
			}

			static void as_voxel_world_constructor_wrapper_basic(uint8_t* memory)
			{
				new (memory) noob::voxel_world();
			}
			
			static void as_voxel_world_constructor_wrapper_uint_3(uint8_t* memory, size_t x, size_t y, size_t z)
			{
				new(memory) noob::voxel_world(x, y, z);
			}

			static void as_voxel_world_destructor_wrapper(uint8_t* memory)
			{
				((noob::voxel_world*)memory)->~voxel_world();
			}


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
			std::string script_name;
	};
}
