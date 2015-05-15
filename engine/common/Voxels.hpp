// Use-case is for a single voxel world, as one would see in an editor environment
// TODO: Replace map with a tree structure that copies-on-write
#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <array>

#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>
#include <PolyVox/RawVolume.h>

namespace noob
{
	class voxel_world
	{
		public:
			// bool set_world(const std::string& name);
			void init();
			// Note: Replaces old saves
			//void save(const std::string& name);

			// OFF format mesh
			void extract_world_mesh(const std::string& filename);
			void extract_region(const std::array<int32_t,6>& region);
			// ---------------- basic shapes --------------
			enum csg_type { UNION, INTERSECT, SUB };

			void sphere(int32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::csg_type op);
			void cube(const std::array<int32_t,6>& region, noob::voxel_world::csg_type op);
			// Cylinder is aligned up/down the y-axis (standing up)
			void cylinder(int32_t radius, int32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::csg_type op);

			//  ---------- management functions -----------
			// Mostly useful for when you've copied a smaller world into a larger one
			bool global_translate(int32_t x_direction, int32_t y_direction, int32_t z_direction);
			bool copy_region(const std::array<int32_t,6>& region_from, const std::array<int32_t,6>& region_to);

			uint64_t memory_consumption();


		protected:
			// Used to check if the world can receive data without condition checks
			bool bounds_clean(const std::string& donor, const std::string& target);

			std::map<std::string, std::shared_ptr<PolyVox::RawVolume<uint8_t>>> volumes;
		
			// Currently-held world 
			std::unique_ptr<PolyVox::RawVolume<uint8_t>> main_world;
	};
}
