#pragma once

#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Model.hpp"
#include "Joint.hpp"
#include "Ghost.hpp"

namespace noob
{
	// This represents a (potentially) multipart model, with physics and joints and such. It will also include such goodies as trigger points
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
