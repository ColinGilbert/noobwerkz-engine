#pragma once

#include <string>

#include <noob/component/component.hpp>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"

namespace noob
{
	class graphics
	{
		public:
			typedef noob::handle<uint32_t> model_handle;
			typedef noob::handle<uint32_t> texture_2d_handle;
			typedef noob::handle<uint32_t> texture_3d_handle;
			typedef noob::handle<uint32_t> texture_cube_handle;
			typedef noob::handle<uint32_t> shader_handle;
			typedef noob::handle<uint32_t> program_handle;

			struct scaled_model
			{
				scaled_model() noexcept(true) : scales(noob::vec3(1.0, 1.0, 1.0)) {}
				noob::graphics::model_handle model_h;
				noob::vec3 scales;
			};

			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			noob::graphics::model_handle model(const noob::basic_mesh&) noexcept(true);

			enum class tex_compression
			{
				NONE, ETC2, PVRTC, ADST
			};

			noob::graphics::texture_2d_handle texture_2d(const std::string&, noob::graphics::tex_compression, uint32_t width, uint32_t height) noexcept(true);	

			noob::graphics::texture_3d_handle texture_3d(const std::string&, noob::graphics::tex_compression, uint32_t width, uint32_t height) noexcept(true);	

			noob::graphics::texture_cube_handle texture_cube(const std::string&, noob::graphics::tex_compression, uint32_t width, uint32_t height) noexcept(true);	
			
			void frame(uint32_t width, uint32_t height) noexcept(true);

			// Static getter. Sorry.
			static graphics& get_instance() noexcept(true)
			{
				static graphics the_instance;
				ptr_to_instance = &the_instance;

				return *ptr_to_instance;
			}

		protected:

			static constexpr uint32_t vert_pos_index = 0;
			static constexpr uint32_t vert_normal_index = 1;
			static constexpr uint32_t vert_colour_index = 2;
			static constexpr uint32_t vert_texture_index = 3;
			static constexpr uint32_t vert_matrix_index = 4;	
			




			// Static getter-related stuff. TODO: Test the output that a proper compiler gives on an inherited type or try to templatize this.
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

	};
}
