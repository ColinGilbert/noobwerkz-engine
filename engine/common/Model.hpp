#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	class graphics;
	class model
	{
		friend class graphics;
		public:
		static constexpr uint32_t materials_stride = sizeof(noob::vec4f);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4f) * 2;
		static constexpr uint32_t terrain_stride = sizeof(noob::vec4f) * 3;

		model() noexcept(true) : valid(false), type(noob::model::geom_type::INVALID), n_vertices(0), n_indices(0), n_instances(0), vao(0), vertices_vbo(0), indices_vbo(0), instanced_colours_vbo(0), instanced_matrices_vbo(0) {}

		enum class geom_type
		{
			INDEXED_MESH, TERRAIN, POINT_SPRITE, BILLBOARD, INVALID
		};

		enum class instanced_data_type
		{
			COLOUR, MATRICES
		};

		enum class buffer_type
		{
			VAO, MATRICES, MATERIALS, VERTICES, INDICES
		};

		bool is_instanced() const noexcept(true)
		{
			return n_instances > 1;
		}

		bool is_indexed() const noexcept(true)
		{
			return n_indices > 0;
		}

		uint32_t aggregate_buffer() const noexcept(true)
		{
			return vao;
		}

		uint32_t matrices_buffer() const noexcept(true)
		{
			return instanced_matrices_vbo;
		}

		uint32_t materials_buffer() const noexcept(true)
		{
			return instanced_colours_vbo;
		}

		protected:
		bool valid;
		noob::model::geom_type type;
		uint32_t  n_vertices, n_indices, n_instances, vao, vertices_vbo, indices_vbo, instanced_matrices_vbo, instanced_colours_vbo;
	};

	typedef noob::handle<noob::model> model_handle;
}
