#pragma once

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include <LinearMath/btConvexHull.h>

#include "Vec3.hpp"
#include "format.h"

namespace noob
{
	enum class collision_type
	{
		NOTHING = 0,
		TERRAIN = 1 << 1,
		CHARACTER = 1 << 2,
		TEAM_A = 1 << 3,
		TEAM_B = 1 << 4,
		POWERUP = 1 << 5,
	};

	enum class pos_type
	{
		GHOST = 0, PHYSICAL = 1, ANIM = 2
	};

	struct body_variant
	{
		pos_type type;
		uint32_t index;

		std::string to_string() const noexcept(true)
		{
			fmt::MemoryWriter ww;

			switch(type)
			{
				case (noob::pos_type::GHOST):
					{
						ww << "ghost, ";
						break;
					}
				case (noob::pos_type::PHYSICAL):
					{
						ww << "body, ";
						break;
					}
				case (noob::pos_type::ANIM):
					{
						ww << "anim, ";
						break;
					}
			}

			ww << index;
			return ww.str();
		}
	};

	struct contact_point
	{
		noob::body_variant handle;
		noob::vec3 pos_a, pos_b, normal_on_b;

		std::string to_string() const noexcept(true)
		{
			fmt::MemoryWriter ww;

			ww << handle.to_string() << ", pos a " << pos_a.to_string() << ", pos b" << pos_b.to_string() << ", normal on b " << normal_on_b.to_string();
			return ww.str();
		}
	};


	inline uint32_t get_bullet_stride_multiplier(PHY_ScalarType stride_width)
	{
		uint32_t vertex_multiplier = 1;
		switch (stride_width)
		{
			case(PHY_FLOAT):
				{
					vertex_multiplier = 4;
					break;
				}
			case (PHY_DOUBLE):
				{
					vertex_multiplier = 8;
					break;
				}
			case (PHY_INTEGER):
				{
					vertex_multiplier = 4;
					break;
				}
			case (PHY_SHORT):
				{
					vertex_multiplier = 2;
					break;
				}
			case (PHY_FIXEDPOINT88):
				{
					vertex_multiplier = 2;
					break;
				}
			case (PHY_UCHAR):
				{
					vertex_multiplier = 1;
					break;
				}

		}
		return vertex_multiplier;
	}
}
