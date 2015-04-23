#pragma once

#include <map>

namespace noob
{
	class graphics
	{
		public:
			enum texture_type { DIFFUSE_MAP, SPECULAR_MAP, AMBIENT_MAP, EMISSIVE_MAP, NORMAL_MAP, OPACITY_MAP, DISPLACEMENT_MAP, HEIGHT_MAP, DATA };

			struct texture
			{
				// GLuint id;
				texture_type type;
			};

			static std::map<std::string, noob::graphics::texture> global_textures;
	};
}
