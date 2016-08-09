#pragma once

namespace noob
{
	enum class shader_type
	{       
		BASIC = 0,
		TRIPLANAR = 1
	};

	struct drawing_info
	{
		noob::shader_type type;
		uint32_t shader_index;
		noob::reflectance_handle reflect;
	};

	struct contact_point
	{
		size_t handle;
		noob::vec3 pos_a, pos_b, normal_on_b;
	};

	struct ghost_intersection_results
	{
		noob::ghost_handle ghost;
		std::vector<noob::body_handle> bodies;
		std::vector<noob::ghost_handle> ghosts;
	};

}
