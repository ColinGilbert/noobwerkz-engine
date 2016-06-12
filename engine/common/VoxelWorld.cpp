#include "VoxelWorld.hpp"

#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>



/*
noob::voxel_world::voxel_world()
{
	world_width = 256;
	world_height = 256;
	world_depth = 256;
	world = std::make_unique<PolyVox::RawVolume<uint8_t>>(PolyVox::Region(0, 0, 0, world_width, world_height, world_depth));
}
*/

void noob::voxel_world::init(size_t x, size_t y, size_t z)
{
	world_width = x;
	world_height = y;
	world_depth = z;
	world = std::make_unique<PolyVox::RawVolume<uint8_t>>(PolyVox::Region(0, 0, 0, world_width, world_height, world_depth));				
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
	logger::log(fmt::format("[VoxelWorld] sphere({0}, {1}, {2}, {3}, {4})", radius, origin_x, origin_y, origin_z, fill));
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


void noob::voxel_world::box(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, bool fill)
{
	logger::log(fmt::format("[VoxelWorld] box({0}, {1}, {2}, {3}, {4}, {5}, {6})", lower_x, lower_y, lower_z, upper_x, upper_y, upper_z, fill));
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


uint8_t noob::voxel_world::get(size_t x, size_t y, size_t z) const
{
	return world->getVoxel(x, y, z);
}


uint8_t noob::voxel_world::safe_get(size_t x, size_t y, size_t z) const
{
	if (x < world_width && y < world_width && z < world_depth)
	{
		return world->getVoxel(x, y, z);
	}
	return 0;
}


noob::basic_mesh noob::voxel_world::extract_smooth() const
{
	return extract_region_smooth(0, 0, 0, world_width, world_height, world_depth);
}


noob::basic_mesh noob::voxel_world::extract_cubic() const
{
	return extract_region_cubic(0, 0, 0, world_width, world_height, world_depth);
}
		

noob::basic_mesh noob::voxel_world::extract_cubic(uint8_t bitmask) const
{
	return extract_region_cubic(0, 0, 0, world_width, world_height, world_depth, bitmask);
}


noob::basic_mesh noob::voxel_world::extract_region_smooth(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const
{
	logger::log("[VoxelWorld] extracting smooth region");
	noob::basic_mesh world_mesh;
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
		world_mesh.vertices.push_back(noob::vec3(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//auto vert = half_edges.add_vertex(TriMesh::Point(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//verts.push_back(vert);
	}

	for (size_t i = 0; i < num_indices; i++)
	{
		world_mesh.indices.push_back(decoded_mesh.getIndex(i));
	}

	// world_mesh.normalize();
	return world_mesh;
}


noob::basic_mesh noob::voxel_world::extract_region_cubic(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const
{
	logger::log("[VoxelWorld] extracting cubic region");
	noob::basic_mesh world_mesh;
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
	PolyVox::Region world_region = world->getEnclosingRegion();
	bounding_box.cropTo(world_region);

	// auto mesh = extractCubicMesh(&*world, bounding_box);
	auto mesh = extractCubicMesh(&*world, world->getEnclosingRegion());
	auto decoded_mesh = PolyVox::decodeMesh(mesh);
	size_t num_indices = decoded_mesh.getNoOfIndices();
	size_t num_vertices = decoded_mesh.getNoOfVertices();

	for (size_t i = 0; i < num_vertices; i++)
	{
		auto vertex = decoded_mesh.getVertex(i);
		world_mesh.vertices.push_back(noob::vec3(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//auto vert = half_edges.add_vertex(TriMesh::Point(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//verts.push_back(vert);
	}

	for (size_t i = 0; i < num_indices; i++)
	{
		world_mesh.indices.push_back(decoded_mesh.getIndex(i));
	}

	// world_mesh.normalize();
	return world_mesh;
}


noob::basic_mesh noob::voxel_world::extract_region_cubic(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, uint8_t bitmask) const
{
	logger::log(fmt::format("[VoxelWorld] extracting cubic region with bitmask {0}", bitmask));

	// Collect the voxels into a temporary world.
	PolyVox::RawVolume<bool> temp_world(PolyVox::Region(0, 0, 0, world_width, world_height, world_depth));
	for (size_t x = lower_x; x < std::min(world_width, upper_x); ++x)
	{
		for (size_t y = lower_y; y < std::min(world_height, upper_y); ++y)
		{
			for (size_t z = lower_z; z < std::min(world_depth, upper_z); ++z)
			{
				temp_world.setVoxel(x, y, z, world->getVoxel(x, y, z) & bitmask);
			}
		}
	}

	noob::basic_mesh world_mesh;
	PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
	PolyVox::Region world_region = temp_world.getEnclosingRegion();
	bounding_box.cropTo(world_region);

	auto mesh = extractCubicMesh(&*world, bounding_box);
	auto decoded_mesh = PolyVox::decodeMesh(mesh);
	size_t num_indices = decoded_mesh.getNoOfIndices();
	size_t num_vertices = decoded_mesh.getNoOfVertices();

	for (size_t i = 0; i < num_vertices; i++)
	{
		auto vertex = decoded_mesh.getVertex(i);
		world_mesh.vertices.push_back(noob::vec3(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//auto vert = half_edges.add_vertex(TriMesh::Point(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
		//verts.push_back(vert);
	}

	for (size_t i = 0; i < num_indices; i++)
	{
		world_mesh.indices.push_back(decoded_mesh.getIndex(i));
	}

	// world_mesh.normalize();
	return world_mesh;
}
