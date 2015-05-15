#include "Voxels.hpp"

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256
#define DEFAULT_DEPTH 256


void noob::voxel_world::extract_world_mesh(const std::string& filename)
{
	
}

void extract_region(const std::array<int32_t,6>& region)
{

}

// ---------------- basic shapes --------------
// enum csg_type { UNION, INTERSECT, SUB }
void noob::voxel_world::sphere(int32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::csg_type op) 
{
	// Equation of a sphere 4/3*r*pi
	int32_t side_dim = std::abs(radius) * 2;
	
	

//	PolyVox::Region bounding_box((origin_x);
//	bounding_box

}

void noob::voxel_world::cube(const std::array<int32_t, 6>& region, noob::voxel_world::csg_type op)
{
	
}

// Cylinder is aligned up/down the y-axis (standing up)
void noob::voxel_world::cylinder(int32_t radius, int32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::csg_type op)
{
	
}

//  ---------- management functions -----------
// Mostly useful for when you've copied a smaller world into a larger one
bool noob::voxel_world::global_translate(int32_t x_direction, int32_t y_direction, int32_t z_direction)
{
	PolyVox::Region world_region = main_world->getEnclosingRegion();
	
	int32_t world_width = world_region.getWidthInVoxels();
	int32_t world_height = world_region.getHeightInVoxels();
	int32_t world_depth = world_region.getWidthInVoxels();
	



	return false;
}

void noob::voxel_world::init()
{
	main_world = std::unique_ptr<PolyVox::RawVolume<uint8_t>>(new PolyVox::RawVolume<uint8_t>(PolyVox::Region(0, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH)));
}
/*
bool noob::voxel_world::load_world(const std::string& name) 
{
	// std::map<std::string, std::unique_ptr<PolyVox::RawVolume<uint8_t>>> volumes;
	auto it = volumes->find(name);
	if (it != volumes.end())
	{
		main_world = it->second();	
		return true;
	}

	else return false;
}

void noob::voxel_world::save(const std::string& name) 
{
	
} */
