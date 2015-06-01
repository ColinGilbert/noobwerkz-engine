// Use-case is for a single voxel universe, as one would see in an editor environment
// Currently, polyvox-develop may be broken enough to not use. May need to downgrade to stable
#pragma once

#include <cstdint>
#include <string>

namespace noob
{
	// TODO: Replace long function sigs with structs
	class universe
	{
		public:
			enum op_type { ADD, SUB };
			struct region
			{
				region() {}
				region(int32_t _lower_x, int32_t _lower_y, int32_t _lower_z, int32_t _upper_x, int32_t _upper_y, int32_t _upper_z);

				int32_t lower_x;
				int32_t lower_y;
				int32_t lower_z;
				int32_t upper_x;
				int32_t upper_y;
				int32_t upper_z;
			};
			// bool set_universe(const std::string& name);
			void init();
			// Note: Replaces old saves
			// void save(const std::string& name);

			// OFF format mesh
			void extract_region(const noob::universe::region& reg, const std::string& filename);
			// ---------------- basic shapes --------------
			
			// Polyvox is currently broken :(
			void sphere(uint32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::universe::op_type op);
			void cube(const noob::universe::region& reg, noob::universe::op_type op);
			// Cylinder is aligned up/down the y-axis (standing up)
			void cylinder(uint32_t radius, uint32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::universe::op_type op);

			noob::universe::region get_acceptable_region(const noob::universe::region& reg);
			
			void apply_value(int32_t x, int32_t y, int32_t z, int8_t value);
			void clear_universe();

			//  ---------- management functions -----------
			//  TODO: Determine if needed
			// Mostly useful for when you've copied a smaller universe into a larger one
			// void global_translate(int32_t x_direction, int32_t y_direction, int32_t z_direction);
			
			// bool copy_region(const std::array<int32_t,6>& region_from, const std::array<int32_t,6>& region_to);

			uint64_t memory_consumption();


		protected:
			// Used to check if the universe can receive data without condition checks
			// bool bounds_clean(const std::string& donor, const std::string& target);

			// std::map<std::string, std::shared_ptr<PolyVox::RawVolume<int8_t>>> volumes;
	
			// void copy_a_to_b();

			// Currently-held universe 
			
			// std::unique_ptr<PolyVox::RawVolume<int8_t>> universe_b;
	};
}
