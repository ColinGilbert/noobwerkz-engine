// Use-case is for a single voxel world, as one would see in an editor environment
// Currently, polyvox-develop may be broken enough to not use. May need to downgrade to stable
#pragma once

#include <cstdint>
#include <string>
#include "MathFuncs.hpp"

namespace noob
{
	// TODO: Replace long function sigs with structs
	class world
	{
		public:
			enum op_type { UNION, INTERSECT, DIFFERENCE };
			struct cubic_region
			{
				cubic_region() {}
				cubic_region(int32_t _lower_x, int32_t _lower_y, int32_t _lower_z, int32_t _upper_x, int32_t _upper_y, int32_t _upper_z);

				int32_t lower_x;
				int32_t lower_y;
				int32_t lower_z;
				int32_t upper_x;
				int32_t upper_y;
				int32_t upper_z;
			};
			noob::mat4 cube_to_mat4(const noob::world::cubic_region* reg);
			void init();
			void sphere(uint32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::world::op_type op);
			void cube(const noob::world::cubic_region& reg, noob::world::op_type op);
			void cylinder(uint32_t radius, uint32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::world::op_type op);
			
			void clear_world();

		protected:
	};
}
