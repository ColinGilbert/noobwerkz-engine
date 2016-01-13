// This class extends the voxel world by providing dynamically-resized voxel worlds. Future: Multithreading and serialization.
// NOTE: Will need to be std::move'd into things
#pragma once

#include <functional>
#include <memory>
#include <map>

#include <PolyVox/RawVolume.h>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Logger.hpp"

namespace noob
{
	class voxel_world
	{
		public:
			//typedef PolyVox::RawVolume<uint8_t> 8_bit_world;
			//typedef PolyVox::RawVolume<bool> boolean_world;

			// voxel_world(size_t x = 256, size_t y = 256, size_t z =256) : world_width(x), world_height(y), world_depth(z), world(std::make_unique<PolyVox::RawVolume<uint8_t>>(PolyVox::Region(0, 0, 0, world_width, world_height, world_depth))) {}
			
			void init(size_t x, size_t y, size_t z);

			void set(size_t x, size_t y, size_t z, uint8_t value);

			// TODO: is functor the correct name for this?
			void apply_to_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, std::function<uint8_t(size_t, size_t, size_t)> functor);

			void sphere(size_t radius, size_t origin_x, size_t origin_y, size_t origin_z, bool fill);

			void box(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, bool fill);

			uint8_t get(size_t x, size_t y, size_t z) const;

			uint8_t safe_get(size_t x, size_t y, size_t z) const;

			noob::basic_mesh extract_smooth() const;
			noob::basic_mesh extract_cubic() const;
			noob::basic_mesh extract_cubic(uint8_t bitmask) const;

			noob::basic_mesh extract_region_smooth(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const;
			noob::basic_mesh extract_region_cubic(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const;
			noob::basic_mesh extract_region_cubic(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, uint8_t bitmask) const;


		protected:
			size_t world_width, world_height, world_depth;
			std::unique_ptr<PolyVox::RawVolume<uint8_t>> world;
	};
}
