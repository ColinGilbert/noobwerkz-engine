// The database class represents the interface to our datastore. Noobwerkz uses databases with thoughtfully-defined schemas for its data in order to facilitate bug-free information storage and retrieval.
// Currently, we're running with SQLite but we will be adding support to Postgres
#pragma once


#include <string>
#include <array>

#include <noob/math/math_funcs.hpp>

#include "sqlite3.h"
#include "Results.hpp"
#include "BodyInfo.hpp"
#include "Mesh3D.hpp"
#include "Body.hpp"
#include "Shape.hpp"

namespace noob
{
	class database
	{
		public:
			bool init_file(const std::string& FileName) noexcept(true);

			bool close() noexcept(true);

			uint32_t vec2fp_add(const noob::vec2d Vec) const noexcept(true);
			noob::results<noob::vec2d> vec2fp_get(uint32_t Idx) const noexcept(true);

			uint32_t vec3fp_add(const noob::vec3d Vec) const noexcept(true);
			noob::results<noob::vec3d> vec3fp_get(uint32_t Idx) const noexcept(true);

			uint32_t vec4fp_add(const noob::vec4d Vec) const noexcept(true);
			noob::results<noob::vec4d> vec4fp_get(uint32_t Idx) const noexcept(true);

			uint32_t mat4fp_add(const noob::mat4d Mat) const noexcept(true);
			noob::results<noob::mat4d> mat4fp_get(uint32_t Idx) const noexcept(true);
			
			noob::results<uint32_t> mesh3d_add(const noob::mesh_3d& Mesh, const std::string& Name) const noexcept(true);
			noob::results<noob::mesh_3d> mesh3d_get(const uint32_t Idx) const noexcept(true);
			noob::results<noob::mesh_3d> mesh3d_get(const std::string& Name) const noexcept(true);

			uint32_t body_add(const noob::body_info& Body, const std::string& Name) const noexcept(true);
			noob::results<noob::body_info> body_get(uint32_t Idx) const noexcept(true);
			std::vector<noob::body_info> body_get(const std::string& Name) const noexcept(true);

			uint32_t shape_add(const noob::shape& Shape) const noexcept(true);
			noob::results<noob::shape> shape_get(uint32_t Idx) const noexcept(true);


		protected:
			// TODO: Replace with some kind of metaprogramming.
			enum class statement : uint32_t
			{
				vec2d_add = 0, vec2d_get = 1, vec3d_add = 2, vec3d_get = 3, vec4d_add = 4, vec4d_get = 5, mat4d_add = 6, mat4d_get = 7, mesh3d_exists = 8, mesh3d_get = 9, mesh3d_add = 10, mesh3d_verts_add = 11, mesh3d_verts_get = 12, mesh3d_indices_add = 13, mesh3d_indices_get= 14, phyz_body_exists_by_name = 15, phyz_body_add = 16, phyz_body_get = 17, phyz_shape_lut_add = 18, phyz_shape_lut_get = 19, phyz_sphere_add = 20, phyz_sphere_get = 21, phyz_box_add = 22, phyz_box_get = 23, phyz_cone_add = 24, phyz_cone_get = 25, phyz_cylinder_add = 26, phyz_cylinder_get = 27, phyz_hull_add = 28, phyz_hull_points_add = 29, phyz_hull_points_get = 30, phyz_mesh_add = 31, phyz_mesh_get_points = 32, phyz_mesh_get_mesh = 33

			};
// TODO: Implement mesh3d_exists, phyz_body_exists, phyz_mesh_exists
			bool init() noexcept(true);
			bool exec_single_step(const std::string& Sql) noexcept(true);
			bool prepare_statement(const std::string& Sql, noob::database::statement Index) noexcept(true);
			void clear_bindings(noob::database::statement) const noexcept(true);
			void reset_stmt(noob::database::statement) const noexcept(true);
			void log_error(const std::string& Sql, const std::string& Msg) const noexcept(true);
			sqlite3_stmt* get_stmt(noob::database::statement) const noexcept(true);



			sqlite3* db;
			std::array<sqlite3_stmt*, 64> prepped_statements;
			uint32_t stmt_count = 0; // This holds the number of prepared statements in the above array that have been properly set up. Should initialization fail, we can then free all prepared statements up to the counter's value before returning false.

		

	};

}
