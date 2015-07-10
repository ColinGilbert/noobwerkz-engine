#pragma once

#include "View.hpp"
#include "Scene.hpp"


namespace noob
{
	class game_view : public view
	{
		public:
			void init();
			void draw();
			void input(const std::string& in);
			void load(const std::string& file);
			void save(const std::string& file);
			noob::voxel_world voxels;
			noob::scene scene;	
		protected:
			void create_db(const std::string& file);


	};
}
