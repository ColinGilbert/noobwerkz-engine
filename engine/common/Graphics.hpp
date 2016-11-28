#pragma once

#include <string>

#include <noob/component/component.hpp>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Model.hpp"
#include "GpuBuffer.hpp"
#include "Texture.hpp"
#include "Attrib.hpp"

namespace noob
{
	class graphics
	{

		protected:

			// Static getter-related stuff.
			// TODO: Find cleaner way to do this.
			static graphics* ptr_to_instance;

			graphics() noexcept(true) {}

			graphics(const graphics& rhs) noexcept(true)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}

			graphics& operator=(const graphics& rhs) noexcept(true)
			{
				if (this != &rhs)
				{
					ptr_to_instance = rhs.ptr_to_instance;
				}
				return *this;
			}

			~graphics() noexcept(true) {}

		public:
			static graphics& get_instance() noexcept(true)
			{
				static graphics the_instance;
				ptr_to_instance = &the_instance;

				return *ptr_to_instance;
			}

			typedef noob::handle<uint32_t> shader_handle;
			typedef noob::handle<uint32_t> program_handle;

			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			void use_program(noob::graphics::program_handle) noexcept(true);

			noob::model_handle model_instanced(const noob::basic_mesh&, uint32_t num_instances) noexcept(true);

			void reset_instances(noob::model_handle, uint32_t num_instances) noexcept(true);

			noob::texture_handle texture_2d(uint32_t width, uint32_t height, bool mips, noob::texture::format_type, noob::texture::numeric_type, noob::texture::compression_type, const std::string& data) noexcept(true);	

			// Create a 2D array texture, making the correct number of slots based on width, height, and amount of data given.
			noob::texture_handle array_texture(uint32_t width, uint32_t height, bool mips, noob::texture::format_type format_arg, noob::texture::numeric_type numeric_arg, noob::texture::compression_type compress_arg, const std::string& data) noexcept(true);

			noob::texture_handle texture_3d(uint32_t width, uint32_t height, uint32_t depth, bool mips, noob::texture::format_type format_arg, noob::texture::numeric_type numeric_arg, noob::texture::compression_type compress_arg, const std::string& data) noexcept(true);

			noob::texture_handle texture_cube(uint32_t dim, bool mips, noob::texture::format_type format_arg, noob::texture::numeric_type numeric_arg, noob::texture::compression_type compress_arg, const std::string& data) noexcept(true);	

			void draw(const noob::model_handle, uint32_t num) noexcept(true);

			void frame(uint32_t width, uint32_t height) noexcept(true);

			noob::gpu_write_buffer map_buffer(noob::model_handle, noob::model::instanced_data_type, uint32_t min, uint32_t max) noexcept(true);

			// NOTE: MUST be called as soon as you're finished using the buffer!
			void unmap_buffer() noexcept(true);

			noob::graphics::program_handle get_default_instanced() const noexcept(true) { return instanced_shader; }

			void push_colours(noob::model_handle, uint32_t offset, const std::vector<noob::vec4>& colours) noexcept(true);

			void push_matrices(noob::model_handle, uint32_t offset, const std::vector<noob::mat4>& mats) noexcept(true);

			void eye_pos(const noob::vec3&) noexcept(true);

			void light_direction(const noob::vec3&) noexcept(true);

		protected:

			noob::component<noob::model> models;
			noob::component<noob::texture> textures;

			noob::graphics::program_handle instanced_shader;
			
			int32_t u_eye_pos, u_light_directional;

			// noob::mat4 view_mat, proj_mat;
	};
}
