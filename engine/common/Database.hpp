// The database class represents the interface to our datastore. Noobwerkz uses databases with thoughtfully-defined schemas for its data in order to facilitate bug-free information storage and retrieval.
// Currently, we're running with SQLite but we will be adding support to Postgres
#pragma once


#include <string>
#include <array>

#include "sqlite3.h"
#include "Results.hpp"
namespace noob
{
	class database
	{
		public:
			bool init_file(const std::string& FileName) noexcept(true);

			void close() noexcept(true);

		protected:

			enum class statement : uint32_t
			{
				vec2d_add = 0, vec2d_get = 1, vec3d_add = 2, vec3d_get = 3, vec4d_add = 4, vec4d_get = 5, mat4d_add = 6, mat4d_get = 7, mesh3d_add = 8, mesh3d_add_vert = 9, mesh3d_get_vert_by_index = 10, mesh3d_add_vert_index = 11, mesh3d_get_mesh_indices = 12, phyz_body_add = 13, phyz_body_get = 14, phyz_shape_add_generic = 15, phyz_shape_get_generic = 16, phyz_sphere_add = 17, phyz_sphere_get = 18, phyz_box_add = 19, phyz_box_get = 20, phyz_cone_add = 21, phyz_cone_get = 22, phyz_cylinder_add = 23, phyz_cylinder_get = 24, phyz_hull_add = 25, phyz_hull_get = 26, phyz_hull_add_point = 27, phyz_hull_get_points = 28, phyz_mesh_add = 29, phyz_mesh_get = 30
			};

			bool exec_single_step(const std::string& Sql) noexcept(true);
			bool prepare_statement(const std::string& Sql, noob::database::statement Index) noexcept(true);
			void log_error(const std::string& Sql, const std::string& Msg) const noexcept(true);

			sqlite3* db;
			std::array<sqlite3_stmt*, 32> prepped_statements;

		

	};

}
