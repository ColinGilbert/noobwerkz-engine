#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	class graphics;
	class model
	{
		friend class graphics;
		public:
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

		/*
		   model(noob::model::geom_type type_arg, uint32_t n_instances_arg, uint32_t n_indices_arg, uint32_t n_vertices_arg, uint32_t vao_arg, uint32_t instanced_matrices_vbo_arg, uint32_t instanced_colour_vbo_arg, uint32_t vertices_vbo_arg, uint32_t indices_vbo_arg) noexcept(true) : type(type_arg), n_instances(n_instances_arg), n_indices(n_indices_arg), n_vertices(n_vertices_arg), vao(vao_arg), instanced_matrices_vbo(instanced_matrices_vbo_arg), instanced_colour_vbo(instanced_colour_vbo_arg), vertices_vbo(vertices_vbo_arg), indices_vbo(indices_vbo_arg) {}

		   model(const noob::model&)  noexcept(true) = default;

		   noob::model::geom_type get_type() const noexcept(true)
		   {
		   return type;
		   }
		   */
		bool is_instanced() const noexcept(true)
		{
			return n_instances > 1;
		}

		bool is_indexed() const noexcept(true)
		{
			return n_indices > 0;
		}

		static constexpr uint32_t materials_stride = sizeof(noob::mat4);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4) * 2;
		/*
		   uint32_t num_instances() const noexcept(true)
		   {
		   return n_instances;
		   }

		   uint32_t num_indices() const noexcept(true)
		   {
		   return n_indices;
		   }

		   uint32_t num_vertices() const noexcept(true)
		   {
		   return n_indices;
		   }

		   uint32_t gpu_handle(noob::model::buffer_type arg) const noexcept(true)
		   {
		   switch(arg)
		   {
		   case(noob::model::buffer_type::VAO):
		   {
		   return vao;
		   }
		   case(noob::model::buffer_type::MATRICES):
		   {
		   return instanced_matrices_vbo;
		   }
		   case(noob::model::buffer_type::MATERIALS):
		   {
		   return instanced_colour_vbo;
		   }
		   case(noob::model::buffer_type::VERTICES):
		   {
		   return vertices_vbo;
		   }
		   case(noob::model::buffer_type::INDICES):
		   {
		   return indices_vbo;
		   }
		   };
		   }
		   */

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
