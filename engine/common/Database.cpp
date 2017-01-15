#include "Database.hpp"


#include <noob/fast_hashtable/fast_hashtable.hpp>


#include "Logger.hpp"

bool noob::database::init_file(const std::string& FileName) noexcept(true)
{
	int rc = sqlite3_open(FileName.c_str(), &db);

	if (rc)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Cannot open \"", FileName, "\"", ". Error: ", sqlite3_errmsg(db)));
		return false;
	}

	noob::logger::log(noob::importance::INFO, noob::concat("[Database] Opening SQLite database. Filename: ", FileName));

	return init();
}


bool noob::database::close() noexcept(true)
{
	for(uint32_t i = 0; i < stmt_count; ++i)
	{
		if (sqlite3_finalize(prepped_statements[i]) != SQLITE_OK)
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Could not finalize prepared statement at index ", noob::to_string(i)));
		}
	}
	if (sqlite3_close(db) != SQLITE_OK)
	{
		noob::logger::log(noob::importance::ERROR, "[Database] Error closing connection!");
		return false;
	}

	noob::logger::log(noob::importance::INFO, "[Database] Database closed.");

	return true;
}


uint32_t noob::database::vec2fp_add(const noob::vec2d Vec) const noexcept(true)
{
	int rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec2d_add), 1, Vec[0]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec2d_add), 2, Vec[1]);

	rc = sqlite3_step(get_stmt(noob::database::statement::vec2d_add));

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec2d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec2d_add);
	clear_bindings(noob::database::statement::vec2d_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec2d> noob::database::vec2fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = sqlite3_bind_int64(get_stmt(noob::database::statement::vec2d_get), 1, Idx);
	rc = sqlite3_step(get_stmt(noob::database::statement::vec2d_get));

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec2d>::make_invalid();
	}

	const noob::vec2d result(sqlite3_column_double(get_stmt(noob::database::statement::vec2d_get), 0), sqlite3_column_double(get_stmt(noob::database::statement::vec2d_get), 1));

	reset_stmt(noob::database::statement::mat4d_add);
	clear_bindings(noob::database::statement::mat4d_add);

	return noob::results<noob::vec2d>::make_valid(result);
}


uint32_t noob::database::vec3fp_add(const noob::vec3d Vec) const noexcept(true)
{
	int rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec3d_add), 1, Vec[0]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec3d_add), 2, Vec[1]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec3d_add), 3, Vec[2]);

	rc = sqlite3_step(get_stmt(noob::database::statement::vec3d_add));

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec3d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec3d_add);
	clear_bindings(noob::database::statement::vec3d_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec3d> noob::database::vec3fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = sqlite3_bind_int64(get_stmt(noob::database::statement::vec3d_get), 1, Idx);
	rc = sqlite3_step(get_stmt(noob::database::statement::vec3d_get));

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec3d>::make_invalid();
	}

	const noob::vec3d result(sqlite3_column_double(get_stmt(noob::database::statement::vec3d_get), 0), sqlite3_column_double(get_stmt(noob::database::statement::vec3d_get), 1), sqlite3_column_double(get_stmt(noob::database::statement::vec3d_get), 2));

	reset_stmt(noob::database::statement::mat4d_add);
	clear_bindings(noob::database::statement::mat4d_add);

	return noob::results<noob::vec3d>::make_valid(result);
}


uint32_t noob::database::vec4fp_add(const noob::vec4d Vec) const noexcept(true)
{
	int rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec4d_add), 1, Vec[0]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec4d_add), 2, Vec[1]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec4d_add), 3, Vec[2]);
	rc = sqlite3_bind_double(get_stmt(noob::database::statement::vec4d_add), 4, Vec[3]);

	rc = sqlite3_step(get_stmt(noob::database::statement::vec4d_add));

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec4d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec4d_add);
	clear_bindings(noob::database::statement::vec4d_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec4d> noob::database::vec4fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = sqlite3_bind_int64(get_stmt(noob::database::statement::vec4d_get), 1, Idx);
	rc = sqlite3_step(get_stmt(noob::database::statement::vec4d_get));

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec4d>::make_invalid();
	}

	const noob::vec4d result(sqlite3_column_double(get_stmt(noob::database::statement::vec4d_get), 0), sqlite3_column_double(get_stmt(noob::database::statement::vec4d_get), 1), sqlite3_column_double(get_stmt(noob::database::statement::vec4d_get), 2), sqlite3_column_double(get_stmt(noob::database::statement::vec4d_get), 3));

	reset_stmt(noob::database::statement::mat4d_add);
	clear_bindings(noob::database::statement::mat4d_add);

	return noob::results<noob::vec4d>::make_valid(result);
}


uint32_t noob::database::mat4fp_add(const noob::mat4d Mat) const noexcept(true)
{
	int rc;

	for(uint32_t i = 0; i < 16; ++i)
	{
		rc = sqlite3_bind_double(get_stmt(noob::database::statement::mat4d_add), i+1, Mat[i]);
	}

	rc = sqlite3_step(get_stmt(noob::database::statement::mat4d_add));

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting mat4d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::mat4d_add);
	clear_bindings(noob::database::statement::mat4d_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::mat4d> noob::database::mat4fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = sqlite3_bind_int64(get_stmt(noob::database::statement::mat4d_get), 1, Idx);
	rc = sqlite3_step(get_stmt(noob::database::statement::mat4d_get));

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::mat4d>::make_invalid();
	}

	// Ewww...
	const noob::mat4d result(sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 0), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 1), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 2), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 3), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 4), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 5), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 6), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 7), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 8), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 9), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 10), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 11), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 12), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 13), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 14), sqlite3_column_double(get_stmt(noob::database::statement::mat4d_get), 15));

	reset_stmt(noob::database::statement::mat4d_add);
	clear_bindings(noob::database::statement::mat4d_add);

	return noob::results<noob::mat4d>::make_valid(result);
}


uint32_t noob::database::mesh3d_add(const noob::mesh_3d& Mesh, const std::string& Name) const noexcept(true)
{
	return 0;
}


noob::results<noob::mesh_3d> noob::database::mesh3d_get(uint32_t Idx) const noexcept(true)
{
	return noob::results<noob::mesh_3d>::make_invalid();
}


noob::results<noob::mesh_3d> noob::database::mesh3d_get(const std::string& Name) const noexcept(true)
{
	return noob::results<noob::mesh_3d>::make_invalid();
}


uint32_t noob::database::body_add(const noob::body& Body, const std::string& Name) const noexcept(true)
{
	return 0;
}


noob::results<noob::body_info> noob::database::body_get(uint32_t Idx) const noexcept(true)
{
	return noob::results<noob::body_info>::make_invalid();
}


noob::results<noob::body_info> noob::database::body_get(const std::string& Name) const noexcept(true)
{
	return noob::results<noob::body_info>::make_invalid();
}


uint32_t noob::database::shape_add(const noob::shape& Shape, const std::string& Name) const noexcept(true)
{
	return 0;
}


noob::results<noob::shape> noob::database::shape_get(uint32_t Idx) const noexcept(true)
{
	return noob::results<noob::shape>::make_invalid();
}


noob::results<noob::shape> noob::database::shape_get(const std::string& Name) const noexcept(true)
{
	return noob::results<noob::shape>::make_invalid();
}


bool noob::database::init() noexcept(true)
{
	///////////////////////////////////
	// BUILD OUR SCHEMA
	///////////////////////////////////

	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec2d(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL)")) 
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec3d(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL, z REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec4d(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL, z REAL, w REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mat4d(id INTEGER PRIMARY KEY AUTOINCREMENT, a REAL, b REAL, c REAL, d REAL, e REAL, f REAL, g REAL, h REAL, i REAL, j REAL, k REAL, l REAL, m REAL, n REAL, o REAL, p REAL)"))
	{
		return false;
	}
	// Triangle meshes are rather "fun": They potentially share info among each other while often needing names to make any sense of their use. We use three tables:
	// 	The first for naming and a mesh and offering a primary key to identify it.
	//	The second for the actual vertex information (position, colour, uv, and whatnot...)
	//	The third for the vertex indices of each mesh, using a foreign key to identify which mesh each index belongs to.
	//	Ordering is enforced on all tables (though this may change to only enforcing on the mesh3d_verts_indices table
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_verts(id INTEGER PRIMARY KEY AUTOINCREMENT, pos INTEGER REFERENCES vec3d, colour INTEGER REFERENCES vec4d, uv INTEGER REFERENCES vec2d)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_indices(id INTEGER PRIMARY KEY AUTOINCREMENT, vert INTEGER REFERENCES mesh3d_verts, belongs_to INTEGER REFERENCES mesh3d)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_bodies(id INTEGER PRIMARY KEY AUTOINCREMENT, pos REFERENCES vec3d, orient REFERENCES vec4d, type UNSIGNED INT8, mass REAL, friction REAL, restitution REAL, linear_vel REAL, angular_vel REAL, linear_factor REAL, angular_factor REAL, ccd BOOLEAN, name TEXT)"))
	{
		return false;
	}
	// In order to be able to polymorphically store shapes, we use a a level of indirection and rely on client code to find the correct shape based on the same enum values as used in noob::shape::type.
	// We may one day soon define multiple columns of foreign keys to the various item types and enforce a constraint that all but one must be NULL.
	// However, we'd also need to treat each case differently anyhow in application code, so for simple serialization it might just be better to do things via client-code.
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_generic(id INTEGER PRIMARY KEY AUTOINCREMENT, type UNSIGNED INT8, foreign_id INTEGER, name TEXT)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_sphere(id INTEGER PRIMARY KEY AUTOINCREMENT, radius REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_box(id INTEGER PRIMARY KEY AUTOINCREMENT, half_width REAL, half_height REAL, half_depth REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_cone(id INTEGER PRIMARY KEY AUTOINCREMENT, radius REAL, height REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_cylinder(id INTEGER PRIMARY KEY AUTOINCREMENT, radius REAL, height REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_hull(id INTEGER PRIMARY KEY AUTOINCREMENT)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_hull_points(pos INTEGER REFERENCES vec3d, belongs_to INTEGER REFERENCES phyz_shapes_hull)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_trimesh(id INTEGER PRIMARY KEY AUTOINCREMENT, mesh_id INTEGER REFERENCES mesh3d)"))
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
	if(!prepare_statement("SELECT x, y, z FROM vec3d WHERE id = ?", noob::database::statement::vec3d_get))
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
	if(!prepare_statement("INSERT INTO mat4d(a, b, c, d, e, f, g, h, i, j, k, l, n, m, o, p) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", noob::database::statement::mat4d_add))
	{
		return false;
	}
	if(!prepare_statement("SELECT a, b, c, d, e, f, g, h, i, j, k, l, n, m, o, p FROM mat4d WHERE id = ?", noob::database::statement::mat4d_get))
	{
		return false;	
	}

	// Triangle meshes are rather "fun": They potentially share info among each other while often needing names to make any sense of their use. We use three tables:
	// 	The first for naming and a mesh and offering a primary key to identify it.
	//	The second for the actual vertex information (position, colour, uv, and whatnot...)
	//	The third for the vertex indices of each mesh, using a foreign key to identify which mesh each index belongs to.
	//	Ordering is enforced on all tables (though this may change to only enforcing on the mesh3d_verts_indices table
	// "CREATE TABLE IF NOT EXISTS mesh3d(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)"
	// "CREATE TABLE IF NOT EXISTS mesh3d_verts(id INTEGER PRIMARY KEY AUTOINCREMENT, pos INTEGER REFERENCES vec3d, colour INTEGER REFERENCES vec4d, uv INTEGER REFERENCES vec2d)"
	// "CREATE TABLE IF NOT EXISTS mesh3d_indices(id INTEGER PRIMARY KEY AUTOINCREMENT, vert INTEGER REFERENCES mesh3d_verts_info, belongs_to INTEGER REFERENCES mesh3d_index)"
	if(!prepare_statement("INSERT INTO mesh3d(name) VALUES (?)", noob::database::statement::mesh3d_add))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO mesh3d_verts(pos, colour, uv) VALUES (?, ?, ?)", noob::database::statement::mesh3d_add_vert))
	{
		return false;	
	}
	if(!prepare_statement("SELECT pos, colour, uv FROM mesh3d_verts JOIN mesh3d_indices ON mesh3d_verts.id = mesh3d_indices.vert", noob::database::statement::mesh3d_get_vert_by_index))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO mesh3d_indices(vert, belongs_to) VALUES (?, ?)", noob::database::statement::mesh3d_add_vert_index))
	{
		return false;	
	}
	if(!prepare_statement("SELECT vert FROM mesh3d_indices WHERE mesh3d_indices.belongs_to = ?", noob::database::statement::mesh3d_get_mesh_indices))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS body(id INTEGER PRIMARY KEY, pos REFERENCES vec3d, orient REFERENCES vec4d, type UNSIGNED INT8, mass REAL, friction REAL, restitution REAL, linear_vel REAL, angular_vel REAL, linear_factor REAL, angular_factor REAL, ccd BOOLEAN, name TEXT)"))
	if(!prepare_statement("INSERT INTO phyz_bodies(pos, orient, type, mass, restitution, linear_vel, angular_vel, linear_factor, angular_factor, ccd, name) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", noob::database::statement::phyz_body_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT pos, orient, type, mass, restitution, linear_vel, angular_vel, linear_factor, angular_factor, ccd FROM phyz_bodies WHERE phyz_bodies.id = ?", noob::database::statement::phyz_body_get))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_generic(id INTEGER PRIMARY KEY, type UNSIGNED INT8, foreign_id INTEGER)"
	if(!prepare_statement("INSERT INTO phyz_shapes_generic(type, foreign_id) VALUES (?, ?)", noob::database::statement::phyz_shape_add_generic))
	{
		return false;	
	}
	if(!prepare_statement("SELECT type, foreign_id FROM phyz_shapes_generic WHERE phyz_shapes_generic.id = ?", noob::database::statement::phyz_shape_get_generic))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_sphere(id INTEGER PRIMARY KEY, radius REAL)"	
	if(!prepare_statement("INSERT INTO phyz_shapes_sphere(radius) VALUES (?)", noob::database::statement::phyz_sphere_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT radius FROM phyz_shapes_sphere WHERE phyz_shapes_sphere.id = ?", noob::database::statement::phyz_sphere_get))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_box(id INTEGER PRIMARY KEY, half_width REAL, half_height REAL, half_depth REAL)"	
	if(!prepare_statement("INSERT INTO phyz_shapes_box(half_width, half_height, half_depth) VALUES (?, ?, ?)", noob::database::statement::phyz_box_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT half_width, half_height, half_depth FROM phyz_shapes_box WHERE phyz_shapes_box.id = ?", noob::database::statement::phyz_box_get))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_cone(id INTEGER PRIMARY KEY, radius REAL, height REAL)"
	if(!prepare_statement("INSERT INTO phyz_shapes_cone(radius, height) VALUES (?, ?)", noob::database::statement::phyz_cone_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT radius, height FROM phyz_shapes_cone WHERE phyz_shapes_cone.id = ?", noob::database::statement::phyz_cone_get))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_cylinder(id INTEGER PRIMARY KEY, radius REAL, height REAL)"
	if(!prepare_statement("INSERT INTO phyz_shapes_cylinder(radius, height) VALUES (?, ?)", noob::database::statement::phyz_cylinder_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT radius, height FROM phyz_shapes_cylinder WHERE phyz_shapes_cylinder.id = ?", noob::database::statement::phyz_cylinder_get))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_hulls(id INTEGER PRIMARY KEY, name TEXT)"
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_hull_points(pos INTEGER REFERENCES vec3d, belongs_to INTEGER REFERENCES phyz_shapes_hull_index)"
	if(!prepare_statement("INSERT INTO phyz_shapes_hull DEFAULT VALUES", noob::database::statement::phyz_hull_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT id FROM phyz_shapes_hull WHERE phyz_shapes_hull.id = ?", noob::database::statement::phyz_hull_get))
	{
		return false;	
	}	
	if(!prepare_statement("INSERT INTO phyz_shapes_hull_points(pos, belongs_to) VALUES (?, ?)", noob::database::statement::phyz_hull_add_point))
	{
		return false;	
	}
	if(!prepare_statement("SELECT x, y, z FROM vec3d JOIN phyz_shapes_hull_points ON phyz_shapes_hull_points.pos = vec3d.id WHERE phyz_shapes_hull_points.belongs_to = ?", noob::database::statement::phyz_hull_get_points))
	{
		return false;	
	}
	// "CREATE TABLE IF NOT EXISTS phyz_shapes_trimesh(id INTEGER PRIMARY KEY, mesh_id INTEGER REFERENCES mesh3d)"	
	if(!prepare_statement("INSERT INTO phyz_shapes_trimesh(mesh_id) VALUES (?)", noob::database::statement::phyz_mesh_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT mesh_id FROM phyz_shapes_trimesh WHERE phyz_shapes_trimesh.id = ?", noob::database::statement::phyz_mesh_get))
	{
		return false;	
	}

	noob::logger::log(noob::importance::INFO, noob::concat("[Database] SQLite database open successful."));

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


bool noob::database::prepare_statement(const std::string& Sql, noob::database::statement Stmt) noexcept(true)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, Sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
	{
		log_error(Sql, sqlite3_errmsg(db));
		return false;
	}
	++stmt_count;
	prepped_statements[static_cast<uint32_t>(Stmt)] = stmt;
	return true;
}


void noob::database::clear_bindings(noob::database::statement Statement) const noexcept(true)
{
	sqlite3_clear_bindings(get_stmt(Statement));
}


void noob::database::reset_stmt(noob::database::statement Statement) const noexcept(true)
{
	sqlite3_reset(get_stmt(Statement));
}


void noob::database::log_error(const std::string& Sql, const std::string& Msg) const noexcept(true)
{
	noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Statement \"", Sql, "\" failed with error \"", Msg, "\""));
}

sqlite3_stmt* noob::database::get_stmt(noob::database::statement Stmt) const noexcept(true)
{
	return prepped_statements[static_cast<uint32_t>(Stmt)];
}

