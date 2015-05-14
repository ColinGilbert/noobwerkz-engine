// Use-case is for a single voxel world, as one would see in an editor environment
// TODO: Replace map with a tree structure that copies-on-write
#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <memory>

#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>
#include <PolyVox/RawVolume.h>

namespace noob
{
	class voxel_world
	{
		public:
			bool load_world(const std::string& name);
			// Replaces old saves
			void save_world(const std::string& name);

			// OFF format mesh
			void extract_world_mesh(const std::string& filename);
			void extract_region(uint32_t bottom_left_x, uint32_t bottom_left_y, uint32_t bottom_left_z, uint32_t upper_right_x, uint32_t upper_right_y, uint32_t upper_right_z);
			// ---------------- basic shapes --------------
			enum csg_type { UNION, INTERSECT, SUB };

			void sphere(uint32_t radius, uint32_t origin_x, uint32_t origin_y, uint32_t origin_z, noob::voxel_world::csg_type op, bool smooth = false);
			void cube(uint32_t bottom_left_x, uint32_t bottom_left_y, uint32_t bottom_left_z, uint32_t top_right_x, uint32_t top_right_y, uint32_t top_right_z, noob::voxel_world::csg_type op);
			// Cylinder is aligned up/down the y-axis (standing up)
			void cylinder(uint32_t radius, uint32_t height, uint32_t origin_x, uint32_t origin_y, uint32_t origin_z, noob::voxel_world::csg_type op, bool smooth = false);

			//  ---------- management functions -----------
			// Mostly useful for when you've copied a smaller world into a larger one
			bool global_translate(uint32_t x_direction, uint32_t y_direction, uint32_t z_direction);

			// Trashes current world. Save first
			void set_world_size(uint32_t height, uint32_t width, uint32_t depth);

			uint64_t memory_consumption();


		protected:
			// Used to check if the world can receive data without condition checks
			bool bounds_clean(const std::string& donor, const std::string& target);

			std::map<std::string, std::unique_ptr<PolyVox::RawVolume<uint8_t>>> volumes;
		
			// Currently-held world 
			std::unique_ptr<PolyVox::RawVolume<uint8_t>> main_world;
	};
}
