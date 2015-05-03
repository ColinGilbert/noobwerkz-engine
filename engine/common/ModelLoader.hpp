// This class loads 3D files using Assimp
#pragma once

#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics.hpp"
#include "Drawable.hpp"
#include "Logger.hpp"

namespace noob
{
	// Forward declaration of drawable. Required for compile.
	
	class drawable;
	class model_loader
	{
		public:
			model_loader(const std::string& filename, const std::string& filepath);
			~model_loader(void);

		protected:
			std::unique_ptr<std::string> path;
			std::vector<noob::drawable*> drawable_entries;
	};
}
