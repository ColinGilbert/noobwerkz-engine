#pragma once

namespace noob
{
	struct attrib
	{
		enum class unit_type
		{
			HALF_FLOAT, FLOAT, INT16, INT32, UINT16, UINT32
		};

		enum class packing_type
		{
			VEC4, MAT4
		};

		noob::attrib::unit_type unit;
		noob::attrib::packing_type packing;
	};
}
