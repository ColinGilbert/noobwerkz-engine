#pragma once

#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include <rdestl/hash_map.h>

#include "Model.hpp"
#include "Shape.hpp"

namespace noob
{
	// This represents a multipart model.
	struct armature
	{
		// noob::digraph hierarchy;
		std::vector<noob::shape_handle> shapes;
		std::vector<noob::model_handle> models;
		std::vector<noob::vec4f> base_colours;
		// This last vector represents the models that get a custom colour based on team affiliation.
		std::vector<bool> team_colours;
		// rde::hash_map<std::string. uint32_t> part_names;
	};

	typedef noob::handle<noob::armature> armature_handle;
}
