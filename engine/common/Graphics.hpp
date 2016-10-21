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
					INDEXED_MESH, DYNAMIC_TERRAIN, BILLBOARD, POINT_SPRITE, INVALID
				};
				
				model() noexcept(true) : type(noob::graphics::model::geom_type::INVALID), interleaved_vbo(0), indices_vbo(0), num_indices(0), num_vertices(0) {}
				
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
				uint32_t interleaved_vbo, indices_vbo, num_indices, num_vertices;
			};

			// Represents the client-side info used to control the instances of a given model
			class instanced_model
			{
				friend class graphics;

				public:

				instanced_model() noexcept(true) : num_instances(0), max_instances(0), colours_vbo(0), matrices_vbo(0) {}
				
				struct info
				{
					noob::vec4 colour;
					noob::mat4 mvp_mat;
					noob::mat4 normal_mat;					
				};

				noob::graphics::model m_model;

				// void set_colour(uint32_t i, const noob::vec4&) const noexcept(true);
				// void set_mvp_mat(uint32_t i, const noob::mat4&) const noexcept(true);
				// void set_normal_mat(uint32_t i, const noob::mat4&) const noexcept(true);

				protected:

				uint32_t num_instances, max_instances, colours_vbo, matrices_vbo;
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

			noob::graphics::model_handle model(noob::graphics::model::geom_type geom, const noob::basic_mesh&) noexcept(true);

			std::tuple<bool, noob::graphics::instanced_model> model_instanced(const noob::basic_mesh&, const std::vector<noob::graphics::instanced_model::info>&) noexcept(true);

			noob::graphics::texture reserve_textures_2d(uint32_t width, uint32_t height, uint32_t slots, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type) noexcept(true);

			noob::graphics::texture texture_3d(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			noob::graphics::texture texture_cube(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			void draw(const noob::graphics::instanced_model&, uint32_t num) noexcept(true);

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

			//void draw_inner(const noob::graphics::instanced_model) const noexcept(true);

			std::vector<noob::graphics::instanced_model> instanced_models;
			// glDrawElementsInstanced ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, ( const void * ) NULL, NUM_INSTANCES );
			std::vector<noob::vec4> colour_storage;
			std::vector<noob::mat4> matrices_storage;
	};
}
