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
			INDEXED_MESH, DYNAMIC_TERRAIN, BILLBOARD, POINT_SPRITE, INVALID
		};

		//	model(noob::model::geom_type type_arg, uint32_t num_instances_arg, uint32_t num_indices_arg, uint32_t num_vertices_arg, uint32_t vao_arg, uint32_t colours_vbo_arg, uint32_t matrices_vbo_arg, uint32_t vertices_vbo_arg, uint32_t indices_vbo_arg) noexcept(true) : type(noob::model::geom_type::INVALID), num_instances(num_instances_arg), num_indices(num_indices_arg), num_vertices(num_vertices_arg), vao(vao_arg), colours_vbo(colours_vbo_arg), matrices_vbo(matrices_vbo_arg), vertices_vbo(vertices_vbo_arg), indices_vbo(indices_vbo_arg)  {}

		enum class instanced_data_type
		{
			COLOUR, MATRICES
		};

		struct info
		{
			noob::vec4 colour;
			noob::mat4 mvp_mat;
			noob::mat4 normal_mat;					
		};

		// void set_colour(uint32_t i, const noob::vec4&) const noexcept(true);
		// void set_mvp_mat(uint32_t i, const noob::mat4&) const noexcept(true);
		// void set_normal_mat(uint32_t i, const noob::mat4&) const noexcept(true);

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
