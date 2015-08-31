#pragma once
#include "Mesh.hpp"

#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>


namespace noob
{
	struct physics_mesh
	{
		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(mesh, hulls);
		}

		void init(const noob::basic_mesh& mm)
		{
			mesh = mm;
			hulls = mesh.convex_decomposition();
		}


		noob::basic_mesh mesh;
		std::vector<noob::basic_mesh> hulls;
	};
}
