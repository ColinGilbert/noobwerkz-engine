#pragma once

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
		body_variant() noexcept(true) : index(std::numeric_limits<uint32_t>::max()) {}
		pos_type type;
		uint32_t index;
/*
		std::string to_string() const noexcept(true)
		{
			switch(type)
			{
				case (noob::pos_type::GHOST):
					{
						return "ghost";
					}
				case (noob::pos_type::PHYSICAL):
					{
						ww << "body";
						break;
					}
				case (noob::pos_type::ANIM):
					{
						ww << "anim";
						break;
					}
				default:
				{
					ww << "invalid";
				}
			}

			return ww.str();
		}
*/	};



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
