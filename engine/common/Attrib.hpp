#pragma once

namespace noob
{
	struct attrib
	{
		enum class unit_type
		{
			INT8, UINT8, INT16, UINT16, INT32, UINT32, HALF_FLOAT, FLOAT
		};

		enum class packing_type
		{
			SCALAR, VEC2, VEC3, VEC4, MAT3, MAT4
		};

		noob::attrib::unit_type unit;
		noob::attrib::packing_type packing;
	};
}
