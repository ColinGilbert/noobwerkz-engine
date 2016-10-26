#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	class graphics;
	class model
	{
		friend class graphics;
		public:
		static constexpr uint32_t materials_stride = sizeof(noob::vec4);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4) * 2;

		enum class geom_type
		{
			INDEXED_MESH, DYNAMIC_TERRAIN, POINT_SPRITE, BILLBOARD, INVALID
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
			return instanced_colour_vbo;
		}

		protected:
		noob::model::geom_type type;
		uint32_t n_instances, n_indices, n_vertices, vao, instanced_matrices_vbo, instanced_colour_vbo, vertices_vbo, indices_vbo;
	};

	typedef noob::handle<noob::model> model_handle;
}
