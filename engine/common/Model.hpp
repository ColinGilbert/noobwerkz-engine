// This class loads 3D files using Assimp
#pragma once

#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <bgfx.h>

#include "Graphics.hpp"
#include "Drawable.hpp"
#include "Logger.hpp"

namespace noob
{
	// Forward declaration of drawable. Required to compile.
	class drawable;
	
	class model
	{
		public:
			model(const std::string& filename, const std::string& filepath);
			~model(void);
			void draw(uint8_t view_id, const float* transform, bgfx::ProgramHandle program_handle);
		protected:
			std::unique_ptr<std::string> path;
			std::vector<noob::drawable*> drawable_entries;
	};
}
