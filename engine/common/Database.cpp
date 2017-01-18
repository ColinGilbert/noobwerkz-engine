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
	int rc = bind_double(noob::database::statement::vec2fp_add, 1, Vec[0]);
	rc = bind_double(noob::database::statement::vec2fp_add, 2, Vec[1]);

	rc = step(noob::database::statement::vec2fp_add);

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec2d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec2fp_add);
	clear_bindings(noob::database::statement::vec2fp_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec2d> noob::database::vec2fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = bind_int64(noob::database::statement::vec2fp_get, 1, Idx);
	rc = step(noob::database::statement::vec2fp_get);

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec2d>::make_invalid();
	}

	const noob::vec2d result(column_double(noob::database::statement::vec2fp_get, 0), column_double(noob::database::statement::vec2fp_get, 1));

	reset_stmt(noob::database::statement::vec2fp_get);
	clear_bindings(noob::database::statement::vec2fp_get);

	return noob::results<noob::vec2d>::make_valid(result);
}


uint32_t noob::database::vec3fp_add(const noob::vec3d Vec) const noexcept(true)
{
	int rc = bind_double(noob::database::statement::vec3fp_add, 1, Vec[0]);
	rc = bind_double(noob::database::statement::vec3fp_add, 2, Vec[1]);
	rc = bind_double(noob::database::statement::vec3fp_add, 3, Vec[2]);

	rc = step(noob::database::statement::vec3fp_add);

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec3d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec3fp_add);
	clear_bindings(noob::database::statement::vec3fp_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec3d> noob::database::vec3fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = bind_int64(noob::database::statement::vec3fp_get, 1, Idx);
	rc = step(noob::database::statement::vec3fp_get);

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec3d>::make_invalid();
	}

	const noob::vec3d result(column_double(noob::database::statement::vec3fp_get, 0), column_double(noob::database::statement::vec3fp_get, 1), column_double(noob::database::statement::vec3fp_get, 2));

	reset_stmt(noob::database::statement::vec3fp_get);
	clear_bindings(noob::database::statement::vec3fp_get);

	return noob::results<noob::vec3d>::make_valid(result);
}


uint32_t noob::database::vec4fp_add(const noob::vec4d Vec) const noexcept(true)
{
	int rc = bind_double(noob::database::statement::vec4fp_add, 1, Vec[0]);
	rc = bind_double(noob::database::statement::vec4fp_add, 2, Vec[1]);
	rc = bind_double(noob::database::statement::vec4fp_add, 3, Vec[2]);
	rc = bind_double(noob::database::statement::vec4fp_add, 4, Vec[3]);

	rc = step(noob::database::statement::vec4fp_add);

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting vec4d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::vec4fp_add);
	clear_bindings(noob::database::statement::vec4fp_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::vec4d> noob::database::vec4fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = bind_int64(noob::database::statement::vec4fp_get, 1, Idx);
	rc = step(noob::database::statement::vec4fp_get);

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::vec4d>::make_invalid();
	}

	const noob::vec4d result(column_double(noob::database::statement::vec4fp_get, 0), column_double(noob::database::statement::vec4fp_get, 1), column_double(noob::database::statement::vec4fp_get, 2), column_double(noob::database::statement::vec4fp_get, 3));

	reset_stmt(noob::database::statement::vec4fp_get);
	clear_bindings(noob::database::statement::vec4fp_get);

	return noob::results<noob::vec4d>::make_valid(result);
}


uint32_t noob::database::mat4fp_add(const noob::mat4d Mat) const noexcept(true)
{
	int rc;

	for(uint32_t i = 0; i < 16; ++i)
	{
		rc = bind_double(noob::database::statement::mat4fp_add, i+1, Mat[i]);
	}

	rc = step(noob::database::statement::mat4fp_add);

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting mat4d: ", sqlite3_errmsg(db)));
		return 0;
	}

	const int64_t rowid = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::mat4fp_add);
	clear_bindings(noob::database::statement::mat4fp_add);

	return static_cast<uint32_t>(std::fabs(rowid));
}


noob::results<noob::mat4d> noob::database::mat4fp_get(uint32_t Idx) const noexcept(true)
{
	int rc = bind_int64(noob::database::statement::mat4fp_get, 1, Idx);
	rc = step(noob::database::statement::mat4fp_get);

	if (rc != SQLITE_DONE)
	{
		return noob::results<noob::mat4d>::make_invalid();
	}

	// Ewww...
	const noob::mat4d result( 
			column_double(noob::database::statement::mat4fp_get, 0),
			column_double(noob::database::statement::mat4fp_get, 1),
			column_double(noob::database::statement::mat4fp_get, 2),
			column_double(noob::database::statement::mat4fp_get, 3),
			column_double(noob::database::statement::mat4fp_get, 4),
			column_double(noob::database::statement::mat4fp_get, 5),
			column_double(noob::database::statement::mat4fp_get, 6),
			column_double(noob::database::statement::mat4fp_get, 7),
			column_double(noob::database::statement::mat4fp_get, 8),
			column_double(noob::database::statement::mat4fp_get, 9),
			column_double(noob::database::statement::mat4fp_get, 10),
			column_double(noob::database::statement::mat4fp_get, 11),
			column_double(noob::database::statement::mat4fp_get, 12),
			column_double(noob::database::statement::mat4fp_get, 13),
			column_double(noob::database::statement::mat4fp_get, 14),
			column_double(noob::database::statement::mat4fp_get, 15) );

	reset_stmt(noob::database::statement::mat4fp_get);
	clear_bindings(noob::database::statement::mat4fp_get);

	return noob::results<noob::mat4d>::make_valid(result);
}

//mesh3d_get_id mesh3d_add, mesh3d_verts_add, mesh3d_verts_get, mesh3d_indices_add, mesh3d_indices_get,
noob::results<uint32_t> noob::database::mesh3d_add(const noob::mesh_3d& Mesh, const std::string& Name) const noexcept(true)
{
	// First, check the name to see if it already exists or not
	int rc;

	if (Name.empty())
	{
		rc = bind_null(noob::database::statement::mesh3d_add, 1);
	}
	else
	{
		rc = bind_text(noob::database::statement::mesh3d_add, 1, Name);
	}

	rc = step(noob::database::statement::mesh3d_add);

	if (rc != SQLITE_DONE)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting mesh3d: ", sqlite3_errmsg(db)));
		return noob::results<uint32_t>::make_invalid();
	}

	const int64_t rowid_named_mesh = sqlite3_last_insert_rowid(db);

	reset_stmt(noob::database::statement::mesh3d_add);
	clear_bindings(noob::database::statement::mesh3d_add);

	// Local-to-database vert index mappings.
	std::vector<int64_t> indices_mapping;

	// First, put in the verts...
	for (uint32_t i = 0; i < Mesh.vertices.size(); ++i)
	{
		const uint32_t pos_idx = vec3fp_add(noob::convert<float, double>(Mesh.vertices[i].position));
		const uint32_t normal_idx = vec3fp_add(noob::convert<float, double>(Mesh.vertices[i].normal));
		const uint32_t texcoords_idx = vec3fp_add(noob::convert<float, double>(Mesh.vertices[i].texcoords));
		const uint32_t colour_idx = vec4fp_add(noob::convert<float, double>(Mesh.vertices[i].colour));

		rc = bind_int64(noob::database::statement::mesh3d_verts_add, 1, pos_idx);
		rc = bind_int64(noob::database::statement::mesh3d_verts_add, 2, normal_idx);
		rc = bind_int64(noob::database::statement::mesh3d_verts_add, 3, texcoords_idx);
		rc = bind_int64(noob::database::statement::mesh3d_verts_add, 4, colour_idx);

		rc = step(noob::database::statement::mesh3d_verts_add);

		if (rc != SQLITE_DONE)
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting mesh3d vertex: ", sqlite3_errmsg(db)));
			return noob::results<uint32_t>::make_invalid();
		}

		const int64_t rowid = sqlite3_last_insert_rowid(db);

		indices_mapping.push_back(rowid);

		reset_stmt(noob::database::statement::mesh3d_verts_add);
	}

	clear_bindings(noob::database::statement::mesh3d_verts_add);

	// Now, put in the indices.

	for (uint32_t i : Mesh.indices)
	{
		rc = bind_int64(noob::database::statement::mesh3d_indices_add, 1, indices_mapping[i]);
		rc = step(noob::database::statement::mesh3d_indices_add);

		if (rc != SQLITE_DONE)
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error while inserting mesh3d index: ", sqlite3_errmsg(db)));
			return noob::results<uint32_t>::make_invalid();
		}

		reset_stmt(noob::database::statement::mesh3d_indices_add);
	}

	clear_bindings(noob::database::statement::mesh3d_indices_add);


	return noob::results<uint32_t>::make_valid(std::fabs(rowid_named_mesh));
}


noob::results<noob::mesh_3d> noob::database::mesh3d_get(uint32_t Idx) const noexcept(true)
{
	bool running = true;
	noob::mesh_3d results_holder;
	noob::fast_hashtable indices_mapping;

	//////////////////////////////
	// First, get the vertices
	//////////////////////////////
	int rc = bind_int64(noob::database::statement::mesh3d_verts_get, 1, Idx);
	
	while(running)
	{
		rc = step(noob::database::statement::mesh3d_verts_get);

		const std::string msg = noob::concat("Getting mesh vert ", noob::to_string(results_holder.vertices.size()), " - ");

		if (rc == SQLITE_DONE)
		{
			running = false;
		}
		else if (rc != SQLITE_ROW)
		{
			log_class_error(noob::concat(msg, "Invalid row :("));
			running = false;
		}
		else
		{
			const noob::results<noob::vec3d> pos = vec3fp_get(column_int64(noob::database::statement::mesh3d_verts_get, 1));
			if (!pos.valid)
			{
				log_class_error(noob::concat(msg, "Invalid position"));
				return noob::results<noob::mesh_3d>::make_invalid();
			}

			const noob::results<noob::vec3d> normal = vec3fp_get(column_int64(noob::database::statement::mesh3d_verts_get, 2));
			if (!normal.valid)
			{
				log_class_error(noob::concat(msg, "Invalid normal"));			
				return noob::results<noob::mesh_3d>::make_invalid();
			}

			const noob::results<noob::vec3d> uvw = vec3fp_get(column_int64(noob::database::statement::mesh3d_verts_get, 3));
			if (!uvw.valid)
			{
				log_class_error(noob::concat(msg, "Invalid uvw"));						
				return noob::results<noob::mesh_3d>::make_invalid();
			}

			const noob::results<noob::vec4d> colour = vec4fp_get(column_int64(noob::database::statement::mesh3d_verts_get, 4));
			if (!colour.valid)
			{
				log_class_error(noob::concat(msg, "Invalid colour"));				
				return noob::results<noob::mesh_3d>::make_invalid();
			}

			noob::mesh_3d::vert vv;

			vv.position = noob::convert<double, float>(pos.value);
			vv.normal = noob::convert<double, float>(normal.value);
			vv.texcoords = noob::convert<double, float>(uvw.value);
			vv.colour = noob::convert<double, float>(colour.value);

			results_holder.vertices.push_back(vv);

			const int64_t last_rowid = sqlite3_last_insert_rowid(db);

			auto c = indices_mapping.insert(last_rowid);
			c->value = results_holder.vertices.size() - 1;

			reset_stmt(noob::database::statement::mesh3d_verts_get);
		}

	}

	clear_bindings(noob::database::statement::mesh3d_verts_get);

	///////////////////////////
	// Now, get the indices
	///////////////////////////

	rc = bind_int64(noob::database::statement::mesh3d_indices_get, 1, Idx);

	running = true;
	while(running)
	{
		rc = step(noob::database::statement::mesh3d_indices_get);

		const std::string msg = noob::concat("Getting mesh index ", noob::to_string(results_holder.indices.size()), " - ");

		if (rc == SQLITE_DONE)
		{
			running = false;
		}
		else if (rc != SQLITE_ROW)
		{
			log_class_error(noob::concat(msg, "Invalid row"));
			running = false;
		}
		else
		{
			const uint32_t index_from_db = static_cast<uint32_t>(column_int64(noob::database::statement::mesh3d_indices_get, 1));
			const auto c = indices_mapping.lookup(index_from_db);
			
			if (!indices_mapping.is_valid(c))
			{
				log_class_error(noob::concat(msg, "Invalid vert index"));
				return noob::results<noob::mesh_3d>::make_invalid();
			}

			const uint32_t index = static_cast<uint32_t>(c->value);
			results_holder.indices.push_back(index);

			reset_stmt(noob::database::statement::mesh3d_indices_get);
		}
	}

	clear_bindings(noob::database::statement::mesh3d_indices_get);
	
	if (results_holder.indices.size() == 0 || results_holder.vertices.size() == 0)
	{
		return noob::results<noob::mesh_3d>::make_invalid();
	}

	return noob::results<noob::mesh_3d>::make_valid(results_holder);
}


noob::results<noob::mesh_3d> noob::database::mesh3d_get(const std::string& Name) const noexcept(true)
{
	// if mesh3d exists by name
	int rc = bind_text(noob::database::statement::mesh3d_get_id, 1, Name);

	int64_t results = 0;
	uint32_t counter = 0;

	while ((rc = step(noob::database::statement::mesh3d_get_id)) == SQLITE_OK)
	{
		results = column_int64(noob::database::statement::mesh3d_get_id, 1);
		++counter;
	}

	if (counter > 0)
	{
		return mesh3d_get(static_cast<uint32_t>(results));
	}

	return noob::results<noob::mesh_3d>::make_invalid();
}


uint32_t noob::database::body_add(const noob::body_info& Body, const std::string& Name, uint32_t Generation) const noexcept(true)
{
	
	return 0;
}


noob::results<noob::body_info> noob::database::body_get(uint32_t Idx, uint32_t Generation) const noexcept(true)
{
	return noob::results<noob::body_info>::make_invalid();
}


std::vector<noob::body_info> noob::database::body_get(const std::string& Name, uint32_t Generation) const noexcept(true)
{
	std::vector<noob::body_info> results;

	return results;
}


uint32_t noob::database::shape_add(const noob::shape& Shape) const noexcept(true)
{
	return 0;
}


noob::results<noob::shape> noob::database::shape_get(uint32_t Idx) const noexcept(true)
{
	return noob::results<noob::shape>::make_invalid();
}


bool noob::database::init() noexcept(true)
{
	///////////////////////////////////
	// BUILD OUR SCHEMA
	///////////////////////////////////

	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec2fp(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL)")) 
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec3fp(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL, z REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS vec4fp(id INTEGER PRIMARY KEY AUTOINCREMENT, x REAL, y REAL, z REAL, w REAL)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mat4fp(id INTEGER PRIMARY KEY AUTOINCREMENT, a REAL, b REAL, c REAL, d REAL, e REAL, f REAL, g REAL, h REAL, i REAL, j REAL, k REAL, l REAL, m REAL, n REAL, o REAL, p REAL)"))
	{
		return false;
	}
	// Triangle meshes are rather "fun": They potentially share info among each other while often needing names to make any sense of their use. We use three tables:
	// 	The first for naming and a mesh and offering a primary key to identify it.
	//	The second for the actual vertex information (position, colour, uv, and whatnot...)
	//	The third for the vertex indices of each mesh, using a foreign key to identify which mesh each index belongs to.
	//	Ordering is enforced on all tables (though this may change to only enforcing on the mesh3d_verts_indices table
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_verts(id INTEGER PRIMARY KEY AUTOINCREMENT, pos INTEGER REFERENCES vec3fp, normal INTEGER REFERENCES vec3fp, uv INTEGER REFERENCES vec3fp, colour INTEGER REFERENCES vec4fp)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS mesh3d_indices(id INTEGER PRIMARY KEY AUTOINCREMENT, vert INTEGER REFERENCES mesh3d_verts, belongs_to INTEGER REFERENCES mesh3d)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_bodies(id INTEGER PRIMARY KEY AUTOINCREMENT, pos REFERENCES vec3fp, orient REFERENCES vec4fp, type UNSIGNED INT8, mass REAL, friction REAL, restitution REAL, linear_vel REAL, angular_vel REAL, linear_factor REAL, angular_factor REAL, ccd BOOLEAN, name TEXT)"))
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
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_hull_points(pos INTEGER REFERENCES vec3fp, belongs_to INTEGER NOT NULL REFERENCES phyz_shapes_hull)"))
	{
		return false;
	}
	if (!exec_single_step("CREATE TABLE IF NOT EXISTS phyz_shapes_trimesh(id INTEGER PRIMARY KEY AUTOINCREMENT, mesh_id INTEGER NOT NULL UNIQUE REFERENCES mesh3d)"))
	{
		return false;
	}

	///////////////////////////////////
	// PREPARED STATEMENTS
	///////////////////////////////////

	if (!prepare_statement("INSERT INTO vec2fp(x, y) VALUES (?, ?)", noob::database::statement::vec2fp_add))
	{
		return false;
	}
	if (!prepare_statement("SELECT x, y FROM vec2fp WHERE id = ?", noob::database::statement::vec2fp_get))
	{
		return false;
	}
	if (!prepare_statement("INSERT INTO vec3fp(x, y, z) VALUES (?, ?, ?)", noob::database::statement::vec3fp_add))
	{
		return false;
	}
	if(!prepare_statement("SELECT x, y, z FROM vec3fp WHERE id = ?", noob::database::statement::vec3fp_get))
	{
		return false;
	}
	if (!prepare_statement("INSERT INTO vec4fp(x, y, z, w) VALUES (?, ?, ?, ?)", noob::database::statement::vec4fp_add))
	{
		return false;
	}
	if (!prepare_statement("SELECT x, y, z, w FROM vec4fp WHERE id = ?", noob::database::statement::vec4fp_get))
	{
		return false;
	}
	if(!prepare_statement("INSERT INTO mat4fp(a, b, c, d, e, f, g, h, i, j, k, l, n, m, o, p) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", noob::database::statement::mat4fp_add))
	{
		return false;
	}
	if(!prepare_statement("SELECT a, b, c, d, e, f, g, h, i, j, k, l, n, m, o, p FROM mat4fp WHERE id = ?", noob::database::statement::mat4fp_get))
	{
		return false;	
	}
	if(!prepare_statement("SELECT id FROM mesh3d WHERE mesh3d.name = ?", noob::database::statement::mesh3d_get_id))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO mesh3d(name) VALUES (?)", noob::database::statement::mesh3d_add))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO mesh3d_verts(pos, colour, uv) VALUES (?, ?, ?)", noob::database::statement::mesh3d_verts_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT pos, colour, uv FROM mesh3d_verts JOIN mesh3d_indices ON mesh3d_verts.id = mesh3d_indices.vert WHERE mesh3d_indices.belongs_to = ?", noob::database::statement::mesh3d_verts_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO mesh3d_indices(vert, belongs_to) VALUES (?, ?)", noob::database::statement::mesh3d_indices_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT vert FROM mesh3d_indices WHERE mesh3d_indices.belongs_to = ?", noob::database::statement::mesh3d_indices_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_bodies(pos, orient, type, mass, restitution, linear_vel, angular_vel, linear_factor, angular_factor, ccd, name) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", noob::database::statement::phyz_body_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT pos, orient, type, mass, restitution, linear_vel, angular_vel, linear_factor, angular_factor, ccd FROM phyz_bodies WHERE phyz_bodies.name = ?", noob::database::statement::phyz_body_get_by_name))
	{
		return false;	
	}
	if(!prepare_statement("SELECT pos, orient, type, mass, restitution, linear_vel, angular_vel, linear_factor, angular_factor, ccd FROM phyz_bodies WHERE phyz_bodies.id = ?", noob::database::statement::phyz_body_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_generic(type, foreign_id) VALUES (?, ?)", noob::database::statement::phyz_shape_lut_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT type, foreign_id FROM phyz_shapes_generic WHERE id = ?", noob::database::statement::phyz_shape_lut_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_sphere(radius) VALUES (?)", noob::database::statement::phyz_sphere_add))
	{
		return false;	
	} 
	if(!prepare_statement("SELECT radius FROM phyz_shapes_sphere WHERE phyz_shapes_sphere.id = ?", noob::database::statement::phyz_sphere_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_box(half_width, half_height, half_depth) VALUES (?, ?, ?)", noob::database::statement::phyz_box_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT half_width, half_height, half_depth FROM phyz_shapes_box WHERE id = ?", noob::database::statement::phyz_box_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_cone(radius, height) VALUES (?, ?)", noob::database::statement::phyz_cone_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT radius, height FROM phyz_shapes_cone WHERE id = ?", noob::database::statement::phyz_cone_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_cylinder(radius, height) VALUES (?, ?)", noob::database::statement::phyz_cylinder_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT radius, height FROM phyz_shapes_cylinder WHERE id = ?", noob::database::statement::phyz_cylinder_get))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_hull DEFAULT VALUES", noob::database::statement::phyz_hull_add))
	{
		return false;	
	}
	if(!prepare_statement("INSERT INTO phyz_shapes_hull_points(pos, belongs_to) VALUES (?, ?)", noob::database::statement::phyz_hull_points_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT x, y, z FROM vec3fp JOIN phyz_shapes_hull_points ON phyz_shapes_hull_points.pos = vec3fp.id WHERE phyz_shapes_hull_points.belongs_to = ?", noob::database::statement::phyz_hull_points_get))
	{
		return false;	
	}
	/*	if(!prepare_statement("SELECT EXISTS(SELECT 1 FROM phyz_shapes_trimesh JOIN mesh3d ON phyz_shapes_trimesh.mesh_id = ?)", noob::database::statement::phyz_mesh_exists_by_mesh3d_id))
		{
		return false;	
		}
		*/
	if(!prepare_statement("INSERT INTO phyz_shapes_trimesh(mesh_id) VALUES (?)", noob::database::statement::phyz_mesh_add))
	{
		return false;	
	}
	if(!prepare_statement("SELECT mesh_id FROM phyz_shapes_trimesh WHERE phyz_shapes_trimesh.id = ?", noob::database::statement::phyz_mesh_get_mesh))
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
		log_sql_error(Sql, err_msg);
		return false;
	}
	return true;
}


bool noob::database::prepare_statement(const std::string& Sql, noob::database::statement Stmt) noexcept(true)
{
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, Sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
	{
		log_sql_error(Sql, sqlite3_errmsg(db));
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


void noob::database::log_sql_error(const std::string& Sql, const std::string& Msg) const noexcept(true)
{
	noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Statement \"", Sql, "\" failed with error \"", Msg, "\""));
}


void noob::database::log_class_error(const std::string& Msg) const noexcept(true)
{
	noob::logger::log(noob::importance::ERROR, noob::concat("[Database] ", Msg));
}


void noob::database::log_warning(const std::string& Msg) const noexcept(true)
{
	noob::logger::log(noob::importance::WARNING, noob::concat("[Database] ", Msg));
}


int noob::database::step(noob::database::statement Statement) const noexcept(true)
{
	return sqlite3_step(get_stmt(Statement));
}


sqlite3_stmt* noob::database::get_stmt(noob::database::statement Stmt) const noexcept(true)
{
	return prepped_statements[static_cast<uint32_t>(Stmt)];
}


int64_t noob::database::column_int64(noob::database::statement Statement, uint32_t Pos) const noexcept(true)
{
	return sqlite3_column_int64(get_stmt(Statement), Pos);
}


double noob::database::column_double(noob::database::statement Statement, uint32_t Pos) const noexcept(true)
{
	return sqlite3_column_double(get_stmt(Statement), Pos);
}


std::string noob::database::column_text(noob::database::statement Statement, uint32_t Pos) const noexcept(true)
{
	return std::string(reinterpret_cast<const char*>(sqlite3_column_text(get_stmt(Statement), Pos)));
}


int noob::database::bind_int64(noob::database::statement Statement, uint32_t Pos, int64_t Arg) const noexcept(true)
{
	return sqlite3_bind_int64(get_stmt(Statement), Pos, Arg);
}


int noob::database::bind_double(noob::database::statement Statement, uint32_t Pos, double Arg) const noexcept(true)
{
	return sqlite3_bind_double(get_stmt(Statement), Pos, Arg);
}


int noob::database::bind_text(noob::database::statement Statement, uint32_t Pos, const std::string& Arg) const noexcept(true)
{
	return sqlite3_bind_text(get_stmt(Statement), Pos, Arg.c_str(), Arg.size(), nullptr);
}


int noob::database::bind_null(noob::database::statement Statement, uint32_t Pos) const noexcept(true)
{
	return sqlite3_bind_null(get_stmt(Statement), Pos);
}


void noob::database::log_sql_error(const std::string& Message) const noexcept(true)
{
	noob::logger::log(noob::importance::ERROR, noob::concat("[Database] Error. User: ", Message, " DB: ", sqlite3_errmsg(db)));
}
