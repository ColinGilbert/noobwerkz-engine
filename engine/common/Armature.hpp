#pragma once

#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Model.hpp"
#include "Constraint.hpp"
#include "Ghost.hpp"

namespace noob
{
	// This represents a (potentially) multipart model, with physics and joints and such. It will also include such goodies as trigger points
	class armature
	{
		public:
			
			

		protected:

			noob::digraph hierarchy; // The nodes' numbers index into the vector structures below.
			std::vector<noob::body_handle> bodies;
			std::vector<noob::constraint_handle> constraints;
			std::vector<noob::vec4f> colours;
			std::vector<bool> team_colours; // Refers the to the limbs to get coloured on a per-team basis.
	};

	typedef noob::handle<noob::armature> armature_handle;
}
