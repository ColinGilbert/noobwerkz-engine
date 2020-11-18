#pragma once

#include <noob/utils/hash_map>
#include "Shape.hpp"

namespace noob
{
	struct compound_shape_child_info
	{
		noob::shape_handle shape;
		noob::vec3f pos;
		noob::versorf orient;
	};

	// This one is kept separate, due to the different manners in which compound shapes are drawn (the drawing code indexes the other shapes and draws *them* instead)
	class compound_shapee
	{
		public:
			
			
		protected:
		btCompoundShape * inner;
		// noob::hash_map shapes_to_masses;
	};
}

