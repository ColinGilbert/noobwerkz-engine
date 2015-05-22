#include "Voxels.hpp"

#include "Logger.hpp"

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256
#define DEFAULT_DEPTH 256

void noob::voxel_world::init()
{
	world = std::unique_ptr<PolyVox::RawVolume<int8_t>>(new PolyVox::RawVolume<int8_t>(PolyVox::Region(0, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH)));
	// world_b = std::unique_ptr<PolyVox::RawVolume<int8_t>>(new PolyVox::RawVolume<int8_t>(PolyVox::Region(0, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH)));
}

void noob::voxel_world::extract_region(const noob::voxel_world::region& reg, const std::string& filename)
{
	PolyVox::Region world_region = world->getEnclosingRegion();

	PolyVox::Region bounding_box(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);
	bounding_box.cropTo(world_region);

	auto mesh = extractMarchingCubesMesh(&*world, bounding_box);
	auto decoded_mesh = PolyVox::decodeMesh(mesh);

	auto num_indices = decoded_mesh.getNoOfIndices();
	auto num_vertices = decoded_mesh.getNoOfVertices();

	std::ofstream file;
	file.open(filename);

	file << "OFF " << std::endl;
	file << num_vertices << " " << (num_indices / 3) << " 0" << std::endl;
	for (int i = 0; i < num_vertices; i++)
	{
		auto vertex = decoded_mesh.getVertex(i);

		file << vertex.position.getX() << " " << vertex.position.getY() << " " << vertex.position.getZ() << std::endl;
	}
	for (int i = 0; i < num_indices; i = i + 3)
	{
		auto index_one = decoded_mesh.getIndex(i);
		auto index_two = decoded_mesh.getIndex(i+1);
		auto index_three = decoded_mesh.getIndex(i+2);

		file << "3 " << index_one << " " << index_two << " " << index_three << std::endl;
	}
	file.close();
}

// ---------------- basic shapes --------------
void noob::voxel_world::sphere(uint32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::op_type op) 
{
	PolyVox::Region world_region = world->getEnclosingRegion();


	PolyVox::Region bounding_box(origin_x - radius, origin_y - radius, origin_z - radius, origin_x + radius, origin_y + radius, origin_z + radius);
	bounding_box.cropTo(world_region);

	int32_t lower_x = bounding_box.getLowerX();
	int32_t lower_y = bounding_box.getLowerY();
	int32_t lower_z = bounding_box.getLowerZ();

	int32_t upper_x = bounding_box.getUpperX();
	int32_t upper_y = bounding_box.getUpperY();
	int32_t upper_z = bounding_box.getUpperZ();

	for (int32_t x = lower_x; x < upper_x; x++)
	{
		for (int32_t y = lower_y; y < upper_y; y++)
		{
			for (int32_t z = lower_z; z < upper_z; z++)
			{
				uint32_t distance_from_origin = std::sqrt(std::pow(x - origin_x, 2) + std::pow(y - origin_y, 2) + std::pow(z - origin_z, 2));
				
				if (distance_from_origin < radius)
				{
					switch (op)
					{
						case ADD:
							world->setVoxel(x, y, z, 255);
							break;
						case SUB:
							world->setVoxel(x, y, z, 0);
							break;
					}
				}
			}
		}
	}
}

void noob::voxel_world::cube(const noob::voxel_world::region& reg, noob::voxel_world::op_type op)
{
	PolyVox::Region world_region = world->getEnclosingRegion();

	PolyVox::Region bounding_box(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);
	bounding_box.cropTo(world_region);

	int32_t bb_lower_x = bounding_box.getLowerX();
	int32_t bb_lower_y = bounding_box.getLowerY();
	int32_t bb_lower_z = bounding_box.getLowerZ();

	int32_t bb_upper_x = bounding_box.getUpperX();
	int32_t bb_upper_y = bounding_box.getUpperY();
	int32_t bb_upper_z = bounding_box.getUpperZ();

	for (int32_t x = bb_lower_x; x < bb_upper_x; x++)
	{
		for (int32_t y = bb_lower_y; y < bb_upper_y; y++)
		{
			for (int32_t z = bb_lower_z; z < bb_upper_z; z++)
			{
				switch (op)
				{
					case ADD:
						world->setVoxel(x, y, z, 255);
						break;
					case SUB:
						world->setVoxel(x, y, z, 0);
						break;
				}
			}
		}
	}
}

// Cylinder is aligned up/down the y-axis (standing up)
void noob::voxel_world::cylinder(uint32_t radius, uint32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::voxel_world::op_type op)
{
	PolyVox::Region world_region = world->getEnclosingRegion();

	int32_t safe_height;

	if (height % 2 != 0)
	{
		safe_height = height - 1;
	}
	else
	{
		safe_height = height;
	}

	PolyVox::Region bounding_box(origin_x - radius, origin_y - (safe_height / 2) - 1, origin_z - radius, origin_x + radius, origin_y + (safe_height / 2) + 1, origin_z + radius);

	bounding_box.cropTo(world_region);

	int32_t lower_x = bounding_box.getLowerX();
	int32_t lower_y = bounding_box.getLowerY();
	int32_t lower_z = bounding_box.getLowerZ();

	int32_t upper_x = bounding_box.getUpperX();
	int32_t upper_y = bounding_box.getUpperY();
	int32_t upper_z = bounding_box.getUpperZ();

	for (int32_t x = lower_x; x < upper_x; x++)
	{
		for (int32_t y = lower_y; y < upper_y; y++)
		{
			for (int32_t z = lower_z; z < upper_z; z++)
			{
				int32_t distance_from_origin = std::sqrt(std::pow(x - origin_x, 2) + std::pow(z - origin_z, 2));
				
				if (distance_from_origin <= radius)
				{
					uint8_t density = static_cast<uint8_t>(std::abs(distance_from_origin - radius));
					switch (op)
					{
						case ADD:
							world->setVoxel(x, y, z, density);
							break;
						case SUB:
							int16_t current_voxel = static_cast<int16_t>(world->getVoxel(x, y, z));
							world->setVoxel(x, y, z, static_cast<uint8_t>(std::max(current_voxel - static_cast<int16_t>(density), 0)));
							break;
					}
				}
			}
		}
	}


}

noob::voxel_world::region noob::voxel_world::get_acceptable_region(const noob::voxel_world::region& reg)
{

	PolyVox::Region user_region(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);

	user_region.cropTo(world->getEnclosingRegion());

	noob::voxel_world::region acceptable_region;
	acceptable_region.lower_x = user_region.getLowerX();
	acceptable_region.lower_y = user_region.getLowerY();
	acceptable_region.lower_z = user_region.getLowerZ();
	acceptable_region.upper_x = user_region.getUpperX();
	acceptable_region.upper_y = user_region.getUpperY();
	acceptable_region.upper_z = user_region.getUpperZ();

	return acceptable_region;
}
	
void noob::voxel_world::apply_value(int32_t x, int32_t y, int32_t z, int8_t value)
{
	if (world->getEnclosingRegion().containsPoint(x, y, z))
	{
		world->setVoxel(x, y, z, value);
	}
}

void noob::voxel_world::clear_world()
{
	PolyVox::Region r = world->getEnclosingRegion();
	noob::voxel_world::region reg;
	reg.lower_x = r.getLowerX();
	reg.lower_y = r.getLowerY();
	reg.lower_z = r.getLowerZ();
	reg.upper_x = r.getUpperX();
	reg.upper_y = r.getUpperY();
	reg.upper_z = r.getUpperZ();

	cube(reg, noob::voxel_world::SUB);
}




/*
//  ---------- management functions -----------
// Mostly useful for when you've copied a smaller world into a larger one
void noob::voxel_world::global_translate(int32_t x_direction, int32_t y_direction, int32_t z_direction)
{
	copy_a_to_b();

	// We use world's region as world_b is of the same size
	PolyVox::Region world_region = world->getEnclosingRegion();

	int32_t world_width = world_region.getWidthInVoxels();
	int32_t world_height = world_region.getHeightInVoxels();
	int32_t world_depth = world_region.getWidthInVoxels();

	int32_t lower_x = world_region.getLowerX();
	int32_t lower_y = world_region.getLowerY();
	int32_t lower_z = world_region.getLowerZ();

	int32_t upper_x = world_region.getUpperX();
	int32_t upper_y = world_region.getUpperY();
	int32_t upper_z = world_region.getUpperZ();

	PolyVox::Region new_region(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);

	new_region.shift(x_direction, y_direction, z_direction);

	new_region.cropTo(world_region);

	for(int x = lower_x; x <= upper_x; x++)
	{
		for(int y = lower_y; y <= lower_y; y++)
		{
			for(int z = lower_z; z <= upper_z; z++)
			{
				if (new_region.containsPoint(x, y, z))
				{
					world->setVoxel(x, y, z, world_b->getVoxel(x - x_direction, y - y_direction, z - z_direction));
				}
				else
				{
					world->setVoxel(x, y, z, 0);
				}
			}
		}
	} 
}
*/

/*
   bool noob::voxel_world::load_world(const std::string& name) 
   {
// std::map<std::string, std::unique_ptr<PolyVox::RawVolume<int8_t>>> volumes;
auto it = volumes->find(name);
if (it != volumes.end())
{
world = it->second();	
return true;
}

else return false;
}

void noob::voxel_world::save(const std::string& name) 
{

} */
/*
void noob::voxel_world::copy_a_to_b()	
{
	// Regions are the same size
	PolyVox::Region region = world->getEnclosingRegion();

	int32_t lower_x = region.getLowerX();
	int32_t lower_y = region.getLowerY();
	int32_t lower_z = region.getLowerZ();

	int32_t upper_x = region.getUpperX();
	int32_t upper_y = region.getUpperY();
	int32_t upper_z = region.getUpperZ();

	for (int32_t x = lower_x; x < upper_x; ++x)
	{
		for (int32_t y = lower_y; y < upper_y; ++y)
		{
			for (int32_t z = lower_z; z < upper_z; ++z)
			{
				world_b->setVoxel(x, y, z, world->getVoxel(x, y, z));
			}
		}
	}
}
*/
