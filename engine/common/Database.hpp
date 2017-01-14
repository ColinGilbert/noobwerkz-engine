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

			void close() noexcept(true);

			uint32_t vec2f_add(const noob::vec2f) const noexcept(true);
			noob::results<noob::vec2f> vec2f_get(uint32_t) const noexcept(true);
			noob::results<noob::vec2f> vec2f_get(const std::string& Name) const noexcept(true);

			uint32_t vec3f_add(const noob::vec3f, const std::string& Name) const noexcept(true);
			noob::results<noob::vec3f> vec3f_get(uint32_t) const noexcept(true);
			noob::results<noob::vec3f> vec3f_get(const std::string& Name) const noexcept(true);

			uint32_t vec4f_add(const noob::vec4f, const std::string& Name) const noexcept(true);
			noob::results<noob::vec4f> vec4f_get(uint32_t) const noexcept(true);
			noob::results<noob::vec4f> vec4f_get(const std::string& Name) const noexcept(true);

			uint32_t mat4f_add(const noob::mat4f, const std::string& Name) const noexcept(true);
			noob::results<noob::mat4f> mat4f_get(uint32_t) const noexcept(true);
			noob::results<noob::mat4f> mat4f_get(const std::string& Name) const noexcept(true);
			
			uint32_t mesh3d_add(const noob::mesh_3d&, const std::string& Name) const noexcept(true);
			noob::results<noob::mesh_3d> mesh3d_get(uint32_t) const noexcept(true);
			noob::results<noob::mesh_3d> mesh3d_get(const std::string& Name) const noexcept(true);

			uint32_t body_add(const noob::body&, const std::string& Name) const noexcept(true);
			noob::results<noob::body_info> body_get(uint32_t) const noexcept(true);
			noob::results<noob::body_info> body_get(const std::string& Name) const noexcept(true);

			uint32_t shape_add(const noob::shape&, const std::string& Name) const noexcept(true);
			noob::results<noob::shape> shape_get(uint32_t) const noexcept(true);
			noob::results<noob::shape> shape_get(const std::string& Name) const noexcept(true);


		protected:

			enum class statement : uint32_t
			{
				vec2d_add = 0, vec2d_get = 1, vec3d_add = 2, vec3d_get = 3, vec4d_add = 4, vec4d_get = 5, mat4d_add = 6, mat4d_get = 7, mesh3d_add = 8, mesh3d_add_vert = 9, mesh3d_get_vert_by_index = 10, mesh3d_add_vert_index = 11, mesh3d_get_mesh_indices = 12, phyz_body_add = 13, phyz_body_get = 14, phyz_shape_add_generic = 15, phyz_shape_get_generic = 16, phyz_sphere_add = 17, phyz_sphere_get = 18, phyz_box_add = 19, phyz_box_get = 20, phyz_cone_add = 21, phyz_cone_get = 22, phyz_cylinder_add = 23, phyz_cylinder_get = 24, phyz_hull_add = 25, phyz_hull_get = 26, phyz_hull_add_point = 27, phyz_hull_get_points = 28, phyz_mesh_add = 29, phyz_mesh_get = 30
			};

			bool init() noexcept(true);
			bool exec_single_step(const std::string& Sql) noexcept(true);
			bool prepare_statement(const std::string& Sql, noob::database::statement Index) noexcept(true);
			void log_error(const std::string& Sql, const std::string& Msg) const noexcept(true);

			sqlite3* db;
			std::array<sqlite3_stmt*, 32> prepped_statements;

		

	};

}
