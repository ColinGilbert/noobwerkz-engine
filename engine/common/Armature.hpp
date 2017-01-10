#pragma once

#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include <rdestl/hash_map.h>

#include "InstancedModel.hpp"
#include "Shape.hpp"
#include "Joint.hpp"

namespace noob
{
	// This represents a multipart model.
	class armature
	{
		public:
			
			

		protected:

			noob::digraph hierarchy; // The nodes' numbers index into the vector structures below.
			rde::vector<noob::body_handle> bodies;
			rde::vector<noob::joint_handle> joints;
			rde::vector<noob::vec4f> colours;
			rde::vector<bool> team_colours; // Refers the to the limbs to get coloured on a per-team basis.
	};

	typedef noob::handle<noob::armature> armature_handle;
}
