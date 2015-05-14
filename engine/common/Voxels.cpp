#include "Voxels.hpp"

void noob::voxel_world::extract_world_mesh(const std::string& filename)
{
	
}

void extract_region(uint32_t bottom_left_x, uint32_t bottom_left_y, uint32_t bottom_left_z, uint32_t upper_right_x, uint32_t upper_right_y, uint32_t upper_right_z)
{

}

// ---------------- basic shapes --------------
// enum csg_type { UNION, INTERSECT, SUB }
void noob::voxel_world::sphere(uint32_t radius, uint32_t origin_x, uint32_t origin_y, uint32_t origin_z, noob::voxel_world::csg_type op, bool smooth) 
{
	
}

void noob::voxel_world::cube(uint32_t bottom_left_x, uint32_t bottom_left_y, uint32_t bottom_left_z, uint32_t top_right_x, uint32_t top_right_y, uint32_t top_right_z, noob::voxel_world::csg_type op)
{
	
}

// Cylinder is aligned up/down the y-axis (standing up)
void noob::voxel_world::cylinder(uint32_t radius, uint32_t height, uint32_t origin_x, uint32_t origin_y, uint32_t origin_z, noob::voxel_world::csg_type op, bool smooth)
{
	
}

//  ---------- management functions -----------
// Mostly useful for when you've copied a smaller world into a larger one
bool noob::voxel_world::global_translate(uint32_t x_direction, uint32_t y_direction, uint32_t z_direction)
{
	
}

// Trashes current world. Save first
void noob::voxel_world::set_world_size(uint32_t height, uint32_t width, uint32_t depth)
{
	
}

bool noob::voxel_world::load_world(const std::string& name) 
{
	
}

void noob::voxel_world::save_world(const std::string& name) 
{
	
}
