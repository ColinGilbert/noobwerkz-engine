#pragma once

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "Vec3.hpp"

namespace noob
{
	enum class shader_type
	{       
		BASIC = 0,
		TRIPLANAR = 1
	};

	struct shader_variant
	{
		shader_variant() noexcept(true) : type(shader_type::BASIC), handle(0) {}
		bool operator==(const noob::shader_variant& other) const noexcept(true)
		{
			return (type == other.type && handle == other.handle);
		}
		shader_type type;
		size_t handle;
	};

	struct shading
	{
		noob::shader_type type;
		uint32_t shader_index;
		uint32_t reflect;
	};

	typedef noob::component<noob::shading> shading_holder;
	typedef noob::handle<noob::shading> shading_handle;

	typedef rde::fixed_array<noob::shading, 32> character_colour_scheme;
	typedef rde::vector<noob::shading> boss_colour_scheme;

	typedef noob::component<noob::character_colour_scheme> character_shading_holder;
	typedef noob::component_dynamic<noob::boss_colour_scheme> boss_shading_holder;

	typedef handle<noob::character_colour_scheme> character_shading_handle;
	typedef handle<noob::boss_colour_scheme> boss_shading_handle;

	enum class collision_type
	{
		NOTHING = 0,
		TERRAIN = 1 << 1,
		CHARACTER = 1 << 2,
		TEAM_A = 1 << 3,
		TEAM_B = 1 << 4,
		POWERUP = 1 << 5,
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
