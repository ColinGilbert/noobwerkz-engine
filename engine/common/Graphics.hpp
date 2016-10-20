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
			typedef noob::handle<uint32_t> texture_handle;
			typedef noob::handle<uint32_t> shader_handle;
			typedef noob::handle<uint32_t> program_handle;

			// TODO: Remove
			struct scaled_model
			{
				scaled_model() noexcept(true) : scales(noob::vec3(1.0, 1.0, 1.0)) {}
				noob::graphics::model_handle model_h;
				noob::vec3 scales;
			};

			class attrib
			{
				friend class graphics;
			
				public:

					enum class unit_type
					{
						HALF_FLOAT, FLOAT, INT16, INT32, UINT16, UINT32
					};

					enum class packing_type
					{
						VEC4, MAT4
					};
					
					noob::graphics::attrib::unit_type get_unit_type() const noexcept(true)
					{
						return unit;
					}

					noob::graphics::attrib::packing_type get_packing_type() const noexcept(true)
					{
						return packing;
					}

				protected:
					noob::graphics::attrib::unit_type unit;
					noob::graphics::attrib::packing_type packing;
			};

			class model
			{
				friend class graphics;

				public:

				enum class geom_type
				{
					INDEXED_MESH, DYNAMIC_TERRAIN, BILLBOARD, POINT_SPRITE
				};

				noob::graphics::model_handle get_handle() const noexcept(true)
				{
					return handle;
				}
				
				noob::graphics::model::geom_type get_type() const noexcept(true)
				{
					return type;
				}

				protected:
				noob::graphics::model_handle handle;
				noob::graphics::model::geom_type type;
			};

			class texture
			{
				friend class graphics;

				public:

				enum class storage_type
				{
					TEX_1D, TEX_2D_ARRAY, TEX_3D, TEX_CUBE
				};

				enum class compression_type
				{
					NONE, ETC2, PVRTC, ADST
				};

				noob::graphics::texture_handle get_handle() const noexcept(true)
				{
					return handle;
				}

				noob::graphics::texture::storage_type get_storage_type() const noexcept(true)
				{
					return storage;
				}

				noob::graphics::texture::compression_type get_compression_type() const noexcept(true)
				{
					return compression;
				}

				protected:

				noob::graphics::texture_handle handle;
				noob::graphics::texture::storage_type storage;
				noob::graphics::texture::compression_type compression;
			};



			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			noob::graphics::model_handle model(const noob::basic_mesh&) noexcept(true);

			noob::graphics::texture reserve_texture_1d(uint32_t width, noob::graphics::attrib::unit_type) noexcept(true);

			noob::graphics::texture reserve_texture_2d_array(uint32_t width, uint32_t height, uint32_t slots, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type) noexcept(true);

			noob::graphics::texture texture_3d(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			noob::graphics::texture texture_cube(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			void frame(uint32_t width, uint32_t height) noexcept(true);

			// Static getter. Sorry.
			// TODO: Make more maintainable
			static graphics& get_instance() noexcept(true)
			{
				static graphics the_instance;
				ptr_to_instance = &the_instance;

				return *ptr_to_instance;
			}

		protected:

			// static constexpr uint32_t vert_pos_index = 0;
			// static constexpr uint32_t vert_normal_index = 1;
			// static constexpr uint32_t vert_colour_index = 2;
			// static constexpr uint32_t vert_texture_index = 3;
			// static constexpr uint32_t vert_matrix_index = 4;


			// Static getter-related stuff.
			// TODO: Test the output that a proper compiler gives on an inherited type or try to templatize this.
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
