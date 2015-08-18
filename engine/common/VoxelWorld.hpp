#pragma once
#include <functional>
#include <memory>
#include <map>

//#include <PolyVox/PagedVolume.h>
//#include <PolyVox/FilePager.h>
#include <PolyVox/RawVolume.h>

#include "MathFuncs.hpp"
#include "Mesh.hpp"


namespace noob
{
	class voxel_world
	{
		public:
			// Stateful
			void init();
			void clear_world();
			void set(size_t x, size_t y, size_t z, uint8_t value);
			void apply_to_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, std::function<uint8_t(size_t, size_t, size_t)> functor);
			
			void sphere(size_t radius, size_t origin_x, size_t origin_y, size_t origin_z, bool fill = true); 
			void cube(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, bool fill = true);
			void cylinder(size_t radius, size_t height, size_t origin_x, size_t origin_y, size_t origin_z, const noob::cardinal_axis axis, bool fill = true);

			// Stateless
			uint8_t get(size_t x, size_t y, size_t z) const;
			noob::basic_mesh extract_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const;
			// bool save(const std::string& filename) const;
			// std::unique_ptr<noob::voxel_world> clone() const;
		protected:
			//std::unique_ptr<PolyVox::PagedVolume<uint8_t>> world;
			//std::unique_ptr<PolyVox::FilePager<uint8_t>> pager;
			std::unique_ptr<PolyVox::RawVolume<uint8_t>> world;
			float x_min, y_min, z_min, x_max, y_max, z_max;
	};
}
