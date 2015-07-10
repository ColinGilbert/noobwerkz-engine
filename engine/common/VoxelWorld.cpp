#include "VoxelWorld.hpp"

#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>

#include "Logger.hpp"


void noob::voxel_world::init()
{
//	pager = std::unique_ptr<PolyVox::FilePager<uint8_t>>(new PolyVox::FilePager<uint8_t>("./temp/volumetric"));
//	world = std::unique_ptr<PolyVox::PagedVolume<uint8_t>>(new PolyVox::PagedVolume<uint8_t>(pager.get()));
	world = std::unique_ptr<PolyVox::RawVolume<uint8_t>>(new PolyVox::RawVolume<uint8_t>(PolyVox::Region(0, 0, 0, 512, 512, 512)));
}


void noob::voxel_world::clear_world()
{
	init();
}


void noob::voxel_world::set(size_t x, size_t y, size_t z, uint8_t value)
{
	world->setVoxel(x, y, z, value);
}


// TODO: is functor the correct name for this?
void noob::voxel_world::apply_to_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, std::function<uint8_t(size_t, size_t, size_t)> functor)
{
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
	PolyVox::Region world_region = world->getEnclosingRegion();
	bounding_box.cropTo(world_region);

	size_t bb_lower_x = bounding_box.getLowerX();
	size_t bb_lower_y = bounding_box.getLowerY();
	size_t bb_lower_z = bounding_box.getLowerZ();
	size_t bb_upper_x = bounding_box.getUpperX();
	size_t bb_upper_y = bounding_box.getUpperY();
	size_t bb_upper_z = bounding_box.getUpperZ();


	for (size_t x = bb_lower_x; x < bb_upper_x; ++x)
	{
		for (size_t y = bb_lower_y; y < bb_upper_y; ++y)
		{
			for (size_t z = bb_lower_z; z < bb_upper_z; ++z)
			{
				set(x, y, z, functor(x, y, z));
			}
		}
	}
}


void noob::voxel_world::sphere(size_t radius, size_t origin_x, size_t origin_y, size_t origin_z, bool fill)
{
	PolyVox::Region bounding_box(origin_x - radius, origin_y - radius, origin_z - radius, origin_x + radius, origin_y + radius, origin_z + radius);
	PolyVox::Region world_region = world->getEnclosingRegion();
	bounding_box.cropTo(world_region);


	size_t lower_x = bounding_box.getLowerX();
	size_t lower_y = bounding_box.getLowerY();
	size_t lower_z = bounding_box.getLowerZ();
	size_t upper_x = bounding_box.getUpperX();
	size_t upper_y = bounding_box.getUpperY();
	size_t upper_z = bounding_box.getUpperZ();

	for (size_t x = lower_x; x < upper_x; ++x)
	{
		for (size_t y = lower_y; y < upper_y; ++y)
		{
			for (size_t z = lower_z; z < upper_z; ++z)
			{
				float distance_from_origin = std::sqrt(std::pow(static_cast<float>(x) - static_cast<float>(origin_x), 2) + std::pow(static_cast<float>(y) - static_cast<float>(origin_y), 2) + std::pow(static_cast<float>(z) - static_cast<float>(origin_z), 2));
				if (distance_from_origin < static_cast<float>(radius))
				{
					if (fill == true)
					{
						set(x, y, z, 255);
					}
					else
					{
						set(x, y, z, 0);
					}
				}
			}
		}
	}
}


void noob::voxel_world::cube(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, bool fill)
{
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
	PolyVox::Region world_region = world->getEnclosingRegion();
	bounding_box.cropTo(world_region);

	size_t bb_lower_x = bounding_box.getLowerX();
	size_t bb_lower_y = bounding_box.getLowerY();
	size_t bb_lower_z = bounding_box.getLowerZ();
	size_t bb_upper_x = bounding_box.getUpperX();
	size_t bb_upper_y = bounding_box.getUpperY();
	size_t bb_upper_z = bounding_box.getUpperZ();

	for (size_t x = bb_lower_x; x < bb_upper_x; ++x)
	{
		for (size_t y = bb_lower_y; y < bb_upper_y; ++y)
		{
			for (size_t z = bb_lower_z; z < bb_upper_z; ++z)
			{
				if (fill == true)
				{
					set(x, y, z, 255);
				}
				else
				{
					set(x, y, z, 0);
				}
			}
		}
	}
}


void noob::voxel_world::cylinder(size_t radius, size_t height, size_t origin_x, size_t origin_y, size_t origin_z, noob::cardinal_axis axis, bool fill)
{


/*
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);

	for (size_t x = lower_x; x < upper_x; ++x)
	{
		for (size_t y = lower_y; y < upper_y; ++y)
		{
			for (size_t z = lower_z; z < upper_z; ++z)
			{
				float distance_from_origin = std::sqrt(std::pow(x - origin_x, 2) + std::pow(z - origin_z, 2));
				if (distance_from_origin <= radius)
				{
					if (fill == true)
					{
						set(x, y, z, 255);
					}
					else
					{
						set(x, y, z, 0);
					}
				}
			}
		}
	}
*/
}


uint8_t noob::voxel_world::get(size_t x, size_t y, size_t z) const
{
	return world->getVoxel(x, y, z);
}


noob::mesh noob::voxel_world::extract_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const
{
	noob::mesh noob_mesh;
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
	PolyVox::Region world_region = world->getEnclosingRegion();
	bounding_box.cropTo(world_region);

	auto mesh = extractMarchingCubesMesh(&*world, bounding_box);
	auto decoded_mesh = PolyVox::decodeMesh(mesh);
	size_t num_indices = decoded_mesh.getNoOfIndices();
	size_t num_vertices = decoded_mesh.getNoOfVertices();

	for (size_t i = 0; i < num_vertices; i++)
	{
		auto vertex = decoded_mesh.getVertex(i);
		noob_mesh.vertices.push_back(noob::vec3(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//auto vert = half_edges.add_vertex(TriMesh::Point(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//verts.push_back(vert);
	}

	for (size_t i = 0; i < num_indices; i++)
	{
		noob_mesh.indices.push_back(decoded_mesh.getIndex(i));
	}

	noob::mesh normalized;

	noob_mesh.snapshot("temp/extracted-temp.off");
	normalized.load("temp/extracted-temp.off", "extracted-temp");
	return normalized;
}

/*
bool noob::voxel_world::save(const std::string& filename) const
{
	
}
*/

/*
std::unique_ptr<noob::voxel_world> noob::voxel_world::clone() const
{

}
*/
