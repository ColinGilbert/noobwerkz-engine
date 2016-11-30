#pragma once

namespace noob
{
	struct attrib
	{
		enum class unit_type
		{
			INT8, UINT8, INT16, INT32, UINT16, UINT32, HALF_FLOAT, FLOAT
		};

		enum class packing_type
		{
			VEC4, MAT4
		};

		noob::attrib::unit_type unit;
		noob::attrib::packing_type packing;
	};
}
