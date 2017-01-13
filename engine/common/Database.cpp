#include "Database.hpp"


#include <noob/fast_hashtable/fast_hashtable.hpp>


#include "NoobUtils.hpp"

bool noob::database::init_file(const std::string& FileName) noexcept(true)
{
	///////////////////////////////////
	// OPEN DATABASE
	///////////////////////////////////

	int rc = sqlite3_open(FileName.c_str(), &db);

	if (rc)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Cannot open \"", FileName, "\"", ". Error: ", sqlite3_errmsg(db)));
		return false;
	}

	///////////////////////////////////
	// BUILD OUR SCHEMA
	///////////////////////////////////

	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec2d(id INTEGER PRIMARY KEY, x REAL, y REAL)")) 
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec3d(id INTEGER PRIMARY KEY, x REAL, y REAL, z REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec4d(id INTEGER PRIMARY KEY, x REAL, y REAL, z REAL, w REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mat4d(id INTEGER PRIMARY KEY, first INTEGER REFERENCES vec4d, second INTEGER REFERENCES vec4d, third INTEGER REFERENCES vec4d, fourth INTEGER REFERENCES vec4d)"))
	{
		return false;
	}
	// Triangle meshes are rather "fun": They potentially share info among each other while often needing names to make any sense of their use. We use three tables:
	// 	The first for naming and a mesh and offering a primary key to identify it.
	//	The second for the actual vertex information (position, colour, uv, and whatnot...)
	//	The third for the vertex indices of each mesh, using a foreign key to identify which mesh each index belongs to.
	//	Ordering is enforced on all tables (though this may change to only enforcing on the mesh3d_verts_indices table
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_index(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_verts_info(id INTEGER PRIMARY KEY AUTOINCREMENT, pos INTEGER REFERENCES vec3d, colour INTEGER REFERENCES vec4d, uv INTEGER REFERENCES vec2d)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_verts_indices(id INTEGER PRIMARY KEY AUTOINCREMENT, vert INTEGER REFERENCES mesh3d_verts_info, belongs_to INTEGER REFERENCES mesh3d_index)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS body(id INTEGER PRIMARY KEY, pos REFERENCES vec3d, orient REFERENCES vec4d, type UNSIGNED INT8, mass REAL, friction REAL, restitution REAL, linear_vel REAL, angular_vel REAL, linear_factor REAL, angular_factor REAL, ccd BOOLEAN, name TEXT)"))
	{
		return false;
	}
	// In order to be able to polymorphically store shapes, we use a a level of indirection and rely on client code to find the correct shape based on the same enum values as used in noob::shape::type.
	// We may one day soon define multiple columns of foreign keys to the various item types and enforce a constraint that all but one must be NULL.
	// However, we'd also need to treat each case differently anyhow in application code, so for simple serialization it might just be better to do things via client-code.
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_index(id INTEGER PRIMARY KEY, type UNSIGNED INT8, foreign_id INTEGER)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_sphere(id INTEGER PRIMARY KEY, radius REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_box(id INTEGER PRIMARY KEY, half_width REAL, half_height REAL, half_depth REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_cone(id INTEGER PRIMARY KEY, radius REAL, height REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_cylinder(id INTEGER PRIMARY KEY, radius REAL, height REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_hull_index(id INTEGER PRIMARY KEY, name TEXT)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_hull(pos_id INTEGER REFERENCES vec3d, hull_index_id INTEGER REFERENCES phyz_shapes_hull_index)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_trimesh(id INTEGER PRIMARY KEY, mesh_id INTEGER REFERENCES mesh3d_index, name TEXT)"))
	{
		return false;
	}

	///////////////////////////////////
	// PREPARED STATEMENTS
	///////////////////////////////////
	
	if (!prepare_statement("INSERT INTO vec2d(x, y) VALUES (?, ?)", noob::database::statement::vec2d_add))
	{
		return false;
	}
	if (!prepare_statement("SELECT x, y FROM vec2d WHERE id = ?", noob::database::statement::vec2d_get))
	{
		return false;
	}
	if (!prepare_statement("INSERT INTO vec3d(x, y, z) VALUES (?, ?, ?)", noob::database::statement::vec3d_add))
	{
		return false;
	}
	if(!prepare_statement("SELECT x, y, z FROM vec3d WHERE id = ?", noob::database::statement::vec3d_add))
	{
		return false;
	}
	if (!prepare_statement("INSERT INTO vec4d(x, y, z, w) VALUES (?, ?, ?, ?)", noob::database::statement::vec4d_add))
	{
		return false;
	}
	if (!prepare_statement("SELECT x, y, z, w FROM vec4d WHERE id = ?", noob::database::statement::vec4d_get))
	{
		return false;
	}
	if(!prepare_statement("INSERT INTO mat4d(first, second, third, fourth) VALUES (?, ?, ?, ?)", noob::database::statement::mat4d_add))
	{
		return false;
	}
	if(!prepare_statement("SELECT first, second, third, fourth FROM mat4d WHERE id = ?", noob::database::statement::mat4d_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::mesh3d_add_vert))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::mesh3d_get_vert))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::mesh3d_add_index))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::mesh3d_get_indices))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_body_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_body_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_shape_add_generic))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_shape_get_generic))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_sphere_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_sphere_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_box_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_box_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_cone_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_cone_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_cylinder_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_cylinder_get))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_hull_add_point))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_hull_get_points))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_mesh_add))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_mesh_get_by_key))
	{
		return false;	
	}
	if(!prepare_statement("", noob::database::statement::phyz_mesh_get_by_name))
	{
		return false;	
	}


	return true;
}


bool noob::database::exec_single_step(const std::string& Sql) noexcept(true)
{
	char* err_msg = 0;
	if (sqlite3_exec(db, Sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK)
	{
		log_error(Sql, err_msg);
		return false;
	}
	return true;
}


bool noob::database::prepare_statement(const std::string& Sql, noob::database::statement Index) noexcept(true)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, Sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
	{
		log_error(Sql, sqlite3_errmsg(db));
		return false;
	}
	prepped_statements[static_cast<uint32_t>(Index)] = stmt;
	return true;
}

void noob::database::log_error(const std::string& Sql, const std::string& Msg) const noexcept(true)
{
	noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Statement \"", Sql, "\" failed with error \"", Msg, "\""));
}
