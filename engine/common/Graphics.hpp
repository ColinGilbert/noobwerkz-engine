#pragma once

#include <string>

#include <noob/component/component.hpp>
#include <noob/singleton/singleton.hpp>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Model.hpp"
#include "GpuBuffer.hpp"
#include "Texture.hpp"
#include "Attrib.hpp"
#include "ReturnType.hpp"

namespace noob
{
	class graphics
	{
			public:
			typedef noob::handle<uint32_t> shader_handle;
			typedef noob::handle<uint32_t> program_handle;

			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			void use_program(noob::graphics::program_handle) noexcept(true);

			noob::model_handle model_instanced(const noob::basic_mesh&, uint32_t num_instances) noexcept(true);

			void reset_instances(noob::model_handle, uint32_t num_instances) noexcept(true);

			// TODO: Implement (not GLES3-native)
			// noob::texture_1d_handle reserve_texture_1d(uint32_t length, bool mips, bool compressed, noob::texture_channels, noob::attrib::unit_type) noexcept(true);	

			noob::texture_2d_handle reserve_texture_2d(uint32_t width, uint32_t height, bool mips, bool compressed, noob::texture_channels channels_arg, noob::attrib::unit_type depth_arg) noexcept(true);
		
			// Create a 2D array texture, making the correct number of slots based on width, height, and amount of data given.
			noob::texture_array_2d_handle reserve_array_texture_2d(uint32_t width, uint32_t height, uint32_t indices, bool mips, bool compressed, noob::texture_channels, noob::attrib::unit_type) noexcept(true);

			noob::texture_3d_handle reserve_texture_3d(uint32_t width, uint32_t height, uint32_t depth, bool mips, bool compressed, noob::texture_channels, noob::attrib::unit_type) noexcept(true);

			// noob::texture_handle reserve_texture_cube(uint32_t dims, bool mips, noob::texture_channels, noob::attrib::unit_type, const std::string& data) noexcept(true);	

			void texture_data(noob::texture_1d_handle, const std::string&) const noexcept(true);	

			void texture_data(noob::texture_2d_handle, const std::string&) const noexcept(true);

			void texture_data(noob::texture_array_2d_handle, uint32_t index, const std::string&) const noexcept(true);

			void texture_data(noob::texture_3d_handle, const std::string&) const noexcept(true);				

			void draw(const noob::model_handle, uint32_t num) const noexcept(true);

			void frame(uint32_t width, uint32_t height) const noexcept(true);

			// TODO: These are VBO buffer mapping/unmapping methods. Ensure we know that by renaming.
			noob::gpu_write_buffer map_buffer(noob::model_handle, noob::model::instanced_data_type, uint32_t min, uint32_t max) const noexcept(true);

			// NOTE: MUST be called as soon as you're finished using a buffer!
			void unmap_buffer() const noexcept(true);

			noob::graphics::program_handle get_instanced() const noexcept(true);
			
			// TODO: Replace with more generic uniform setting method(s)
			void eye_pos(const noob::vec3&) const noexcept(true);
			
			void light_direction(const noob::vec3&) const noexcept(true);

		protected:

			noob::component<noob::model> models;
			std::vector<noob::texture_1d> textures_1d;
			std::vector<noob::texture_2d> textures_2d;
			std::vector<noob::texture_array_2d> textures_array_2d;			
			std::vector<noob::texture_3d> textures_3d;

			noob::graphics::program_handle instanced_shader;

			int32_t u_eye_pos, u_light_directional;
	};

	static noob::singleton<noob::graphics> gfx_instance;
	static noob::graphics& get_graphics() { return gfx_instance.get(); }
}
