#include "GameView.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite
{
	void assertion_failed(const char* apFile, const long apLine, const char* apFunc, const char* apExpr, const char* apMsg)
	{
		// Print a message to the standard error output stream, and abort the program.
		logger::log(fmt::format("{0} : {1} : Error: Assertion failed ({2}) in {3}() with message {4}", apFile, apLine, apExpr, apFunc, apMsg));
		//std::abort();
	}
}
#endif

void noob::game_view::init()
{
	scene.init();
}


void noob::game_view::draw()
{
	scene.draw();
}


void noob::game_view::input(const std::string& in)
{

}


void noob::game_view::load(const std::string& filename)
{
	try
	{
		SQLite::Database db(filename.c_str());
		db.exec("pragma foreign_keys = ON;");
		
		// Compile a SQL query, containing one parameter (index 1)
		SQLite::Statement terrain_query(db, "SELECT models.path, position_x, position_y, position_z, shader_id FROM terrains WHERE terrains.model_id = models.id");

		while (terrain_query.executeStep())
		{
			//noob::actor a;

			noob::scene::terrain_info* t = new noob::scene::terrain_info;
			std::string path = terrain_query.getColumn(0);
			double x = terrain_query.getColumn(1);
			double y = terrain_query.getColumn(2);
			double z = terrain_query.getColumn(3);
			int shader_id = terrain_query.getColumn(4);
			
			t->model.translation[0] = static_cast<float>(x);
			t->model.translation[1] = static_cast<float>(y);
			t->model.translation[2] = static_cast<float>(z);

			//a.drawable_ptr = std::unique_ptr<noob::drawable>(new noob::drawable());
			t->model.drawable_ptr = new noob::drawable();
			noob::mesh m;
			m.load(path);
			t->model.drawable_ptr->init(m);
			
			SQLite::Statement shader_query(db, "SELECT colour_1_x, colour_1_y, colour_1_z, colour_1_w, colour_2_x, colour_2_y, colour_2_z, colour_2_w, colour_3_x, colour_3_y, colour_3_z, colour_3_w, colour_4_x, colour_4_y, colour_4_z, colour_4_w, blend_x, blend_y, blend_z, scale_x, scale_y, scale_z, gradient_pos_x, gradient_pos_y WHERE triplanars.id = (?)");
			shader_query.bind(1, shader_id);
			noob::triplanar_renderer::uniform_info uniform;
			while (shader_query.executeStep())
			{
				double colour_1_x = shader_query.getColumn(0);
				uniform.colours[0][0] = static_cast<float>(colour_1_x);
				double colour_1_y = shader_query.getColumn(1);
				uniform.colours[0][1] = static_cast<float>(colour_1_y);
				double colour_1_z = shader_query.getColumn(2);
				uniform.colours[0][2] = static_cast<float>(colour_1_z);
				double colour_1_w = shader_query.getColumn(3);
				uniform.colours[0][3] = static_cast<float>(colour_1_w);

				double colour_2_x = shader_query.getColumn(4);
				uniform.colours[1][0] = static_cast<float>(colour_2_x);
				double colour_2_y = shader_query.getColumn(5);
				uniform.colours[1][1] = static_cast<float>(colour_2_y);
				double colour_2_z = shader_query.getColumn(6);
				uniform.colours[1][2] = static_cast<float>(colour_2_z);
				double colour_2_w = shader_query.getColumn(7);
				uniform.colours[1][3] = static_cast<float>(colour_2_w);

				double colour_3_x = shader_query.getColumn(8);
				uniform.colours[2][0] = static_cast<float>(colour_3_x);
				double colour_3_y = shader_query.getColumn(9);
				uniform.colours[2][1] = static_cast<float>(colour_3_y);
				double colour_3_z = shader_query.getColumn(10);
				uniform.colours[2][2] = static_cast<float>(colour_3_z);
				double colour_3_w = shader_query.getColumn(11);
				uniform.colours[2][3] = static_cast<float>(colour_3_w);

				double colour_4_x = shader_query.getColumn(12);
				uniform.colours[3][0] = static_cast<float>(colour_4_x);
				double colour_4_y = shader_query.getColumn(13);
				uniform.colours[3][1] = static_cast<float>(colour_4_y);
				double colour_4_z = shader_query.getColumn(14);
				uniform.colours[3][2] = static_cast<float>(colour_4_z);
				double colour_4_w = shader_query.getColumn(15);
				uniform.colours[3][3] = static_cast<float>(colour_4_w);

				double blend_x = shader_query.getColumn(16);
				uniform.mapping_blends[0] = static_cast<float>(blend_x);
				double blend_y = shader_query.getColumn(17);
				uniform.mapping_blends[1] = static_cast<float>(blend_y);
				double blend_z = shader_query.getColumn(18);
				uniform.mapping_blends[2] = static_cast<float>(blend_z);

				double scale_x = shader_query.getColumn(19);
				uniform.scales[0] = static_cast<float>(scale_x);
				double scale_y = shader_query.getColumn(20);
				uniform.scales[1] = static_cast<float>(scale_y);
				double scale_z = shader_query.getColumn(21);
				uniform.scales[2] = static_cast<float>(scale_z);

				double grad_pos_x = shader_query.getColumn(22);
				uniform.colour_positions[0] = static_cast<float>(grad_pos_x);
				double grad_pos_y = shader_query.getColumn(23);
				uniform.colour_positions[1] = static_cast<float>(grad_pos_y);
			}
			t->colouring_info = uniform;
			scene.terrains.push_back(t);

		}

	}
	catch (std::exception& e)
	{
		logger::log(fmt::format("[GameView] - create_db() failed - {0}", e.what()));
	}

}


void noob::game_view::save(const std::string& file)
{

}


void noob::game_view::create_db(const std::string& filename)
{
	try
	{
		SQLite::Database db(filename.c_str(), SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);

		db.exec("pragma foreign_keys = ON;");
		db.exec("CREATE TABLE `models` (`id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `path` TEXT NOT NULL UNIQUE );");
		db.exec("CREATE TABLE `triplanars` (`id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `colour_1_x` REAL NOT NULL, `colour_1_y` REAL NOT NULL, `colour_1_z` REAL NOT NULL, `colour_1_w` REAL NOT NULL, `colour_2_x` REAL NOT NULL, `colour_2_y` REAL NOT NULL, `colour_2_z` REAL NOT NULL, `colour_2_w` REAL NOT NULL, `colour_3_x` REAL NOT NULL, `colour_3_y` REAL NOT NULL, `colour_3_z` REAL NOT NULL, `colour_3_w` REAL NOT NULL, `colour_4_x` REAL NOT NULL, `colour_4_y` REAL NOT NULL ,`colour_4_z` REAL NOT NULL, `colour_4_w` REAL NOT NULL, `blend_x` REAL NOT NULL, `blend_y` REAL NOT NULL, `blend_z` REAL NOT NULL, `scale_x` REAL NOT NULL, `scale_y` REAL NOT NULL, `scale_z` REAL NOT NULL, `gradient_pos_x` REAL NOT NULL, `gradient_pos_y` REAL NOT NULL,);");
		db.exec("CREATE TABLE `terrains` (`id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `position_x` REAL NOT NULL, `position_y` REAL NOT NULL, `position_z` REAL NOT NULL, `model_id` INTEGER NOT NULL, `shader_id` INTEGER NOT NULL, FOREIGN KEY (model_id) REFERENCES models(id), FOREIGN KEY (shader_id) REFERENCES triplanars(id));");
	}
	catch (std::exception& e)
	{
		logger::log(fmt::format("[GameView] - create_db() failed - {0}", e.what()));
	}
}
