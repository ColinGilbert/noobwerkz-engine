#pragma once

namespace noob
{
	class graphics;
	// Represents the client-side info used to hold relevant info about a given model
	class model
	{
		friend class graphics;

		public:

		enum class geom_type
		{
			INDEXED_MESH, BILLBOARD, POINT_SPRITE, DYNAMIC_TERRAIN, INVALID
		};

		enum class instanced_data_type
		{
			COLOUR, MATRICES
		};

		noob::model::geom_type get_type() const noexcept(true)
		{
			return type;
		}

		bool is_instanced() const noexcept(true)
		{
			return num_instances > 1;
		}

		bool is_indexed() const noexcept(true)
		{
			return num_indices > 0;
		}

		uint32_t index() const noexcept(true)
		{
			return vao;
		}

		static constexpr uint32_t instanced_colour_stride = sizeof(noob::vec4);
		static constexpr uint32_t instanced_matrices_stride = sizeof(noob::mat4) * 2;

		protected:

		noob::model::geom_type type;
		uint32_t num_instances, num_indices, num_vertices, vao, instanced_colour_vbo, instanced_matrices_vbo, vertices_vbo, indices_vbo;
	};

	typedef noob::handle<noob::model> model_handle;

}
