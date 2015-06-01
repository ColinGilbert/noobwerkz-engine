#include "Voxels.hpp"

#include "Logger.hpp"



#include <map>
#include <memory>
#include <array>
#include <cmath>
#include <fstream>

/*
#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256
#define DEFAULT_DEPTH 256
*/
#include <OpenMesh/Core/IO/MeshIO.hh> // Must be included prior to an OpenMesh class header
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

/*
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>
#include <PolyVox/RawVolume.h>

std::unique_ptr<PolyVox::RawVolume<int8_t>> universe;
*/

void noob::universe::init()
{
	// universe = std::unique_ptr<PolyVox::RawVolume<int8_t>>(new PolyVox::RawVolume<int8_t>(PolyVox::Region(0, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH)));
	// universe_b = std::unique_ptr<PolyVox::RawVolume<int8_t>>(new PolyVox::RawVolume<int8_t>(PolyVox::Region(0, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH)));
}
noob::universe::region::region(int32_t _lower_x, int32_t _lower_y, int32_t _lower_z, int32_t _upper_x, int32_t _upper_y, int32_t _upper_z)
{
	lower_x = _lower_x;
	lower_y = _lower_y;
	lower_z = _lower_z;
	upper_x = _upper_x;
	upper_y = _upper_y;
	upper_z = _upper_z;
}

void noob::universe::extract_region(const noob::universe::region& reg, const std::string& filename)
{
/*
	PolyVox::Region universe_region = universe->getEnclosingRegion();
	PolyVox::Region bounding_box(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);
	bounding_box.cropTo(universe_region);
	auto polyvox_mesh = extractMarchingCubesMesh(&*universe, bounding_box);
	auto polyvox_decoded_mesh = PolyVox::decodeMesh(polyvox_mesh);
	polyvox_decoded_mesh.removeUnusedVertices();
	auto num_indices = polyvox_decoded_mesh.getNoOfIndices();
	auto num_vertices = polyvox_decoded_mesh.getNoOfVertices();
	auto polyvox_vertices = polyvox_decoded_mesh.getVertices();
	auto polyvox_indices = polyvox_decoded_mesh.getIndices();
*/
/*
	logger::log("Mesh decoded");

	// TODO: Use PolyVox::PositionMaterialNormal to represent verts

	{
		std::stringstream ss;
		ss << "num verts = " << num_vertices << " num indices = " << num_indices;
		
		if ( num_indices % 3 ) ss << "; triangles";
		else ss << "; non-triangles";
		
		logger::log(ss.str());
	}
	OpenMesh::TriMesh_ArrayKernelT<> openmesh_mesh;
	std::vector<OpenMesh::TriMesh_ArrayKernelT<>::VertexHandle> openmesh_vertices;
	openmesh_vertices.reserve(num_vertices);

	logger::log("openmesh ready for input");

	for (auto i = 0; i < num_vertices; i++)
	{
		auto v = polyvox_vertices[i];
		
		openmesh_vertices[0] = openmesh_mesh.add_vertex(OpenMesh::TriMesh_ArrayKernelT<>::Point(v.position.getX(), v.position.getY(), v.position.getZ()));
	}

	logger::log("verts in");

	for (auto i = 0; i < num_indices; i += 3)
	{
		std::vector<OpenMesh::TriMesh_ArrayKernelT<>::VertexHandle> openmesh_face_vertices;	
		
		for (auto j = 0; j < 3; ++j)
		{
			openmesh_face_vertices.push_back(openmesh_vertices[i+j]); //OpenMesh::TriMesh_ArrayKernelT<>::VertexHandle(OpenMesh::TriMesh_ArrayKernelT<>::Point(polyvox_vertex.position.getX(), polyvox_vertex.position.getY(), polyvox_vertex.position.getZ())));
		}
		openmesh_mesh.add_face(openmesh_face_vertices);
	}

	logger::log("faces in");
*/
/*
	std::ofstream file;
	std::string temp_filename = filename + "_temp";
	file.open(temp_filename);

	file << "OFF " << std::endl;
	file << num_vertices << " " << (num_indices / 3) << " 0" << std::endl;

	for (int i = 0; i < num_vertices; i++)
	{
		auto vertex = polyvox_decoded_mesh.getVertex(i);

		file << vertex.position.getX() << " " << vertex.position.getY() << " " << vertex.position.getZ() << std::endl;
	}
	for (int i = 0; i < num_indices; i = i + 3)
	{
		auto index_one = polyvox_decoded_mesh.getIndex(i);
		auto index_two = polyvox_decoded_mesh.getIndex(i+1);
		auto index_three = polyvox_decoded_mesh.getIndex(i+2);

		file << "3 " << index_one << " " << index_two << " " << index_three << std::endl;
	}
	file.close();

*/

}


// ---------------- basic shapes --------------

void noob::universe::sphere(uint32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::universe::op_type op) 
{
/*

	PolyVox::Region universe_region = universe->getEnclosingRegion();

	PolyVox::Region bounding_box(origin_x - radius, origin_y - radius, origin_z - radius, origin_x + radius, origin_y + radius, origin_z + radius);
	bounding_box.cropTo(universe_region);
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
							universe->setVoxel(x, y, z, 255);
							break;
						case SUB:
							universe->setVoxel(x, y, z, 0);
							break;
					}
				}
			}
		}
	}
	*/
}

void noob::universe::cube(const noob::universe::region& reg, noob::universe::op_type op)
{
	/*
	PolyVox::Region universe_region = universe->getEnclosingRegion();

	PolyVox::Region bounding_box(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);
	bounding_box.cropTo(universe_region);

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
						universe->setVoxel(x, y, z, 255);
						break;
					case SUB:
						universe->setVoxel(x, y, z, 0);
						break;
				}
			}
		}
	}
	*/
}

// Cylinder is aligned up/down the y-axis (standing up)
void noob::universe::cylinder(uint32_t radius, uint32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::universe::op_type op)
{
	/*
	PolyVox::Region universe_region = universe->getEnclosingRegion();

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

	bounding_box.cropTo(universe_region);

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
							universe->setVoxel(x, y, z, density);
							break;
						case SUB:
							int16_t current_voxel = static_cast<int16_t>(universe->getVoxel(x, y, z));
							universe->setVoxel(x, y, z, static_cast<uint8_t>(std::max(current_voxel - static_cast<int16_t>(density), 0)));
							break;
					}
				}
			}
		}
	}

*/
}

noob::universe::region noob::universe::get_acceptable_region(const noob::universe::region& reg)
{
	/*

	PolyVox::Region user_region(reg.lower_x, reg.lower_y, reg.lower_z, reg.upper_x, reg.upper_y, reg.upper_z);

	user_region.cropTo(universe->getEnclosingRegion());

	noob::universe::region acceptable_region;
	acceptable_region.lower_x = user_region.getLowerX();
	acceptable_region.lower_y = user_region.getLowerY();
	acceptable_region.lower_z = user_region.getLowerZ();
	acceptable_region.upper_x = user_region.getUpperX();
	acceptable_region.upper_y = user_region.getUpperY();
	acceptable_region.upper_z = user_region.getUpperZ();

	return acceptable_region;
	*/
}
	
void noob::universe::apply_value(int32_t x, int32_t y, int32_t z, int8_t value)
{
	/*
	if (universe->getEnclosingRegion().containsPoint(x, y, z))
	{
		universe->setVoxel(x, y, z, value);
	}
	*/
}

void noob::universe::clear_universe()
{
	/*
	PolyVox::Region r = universe->getEnclosingRegion();
	noob::universe::region reg;
	reg.lower_x = r.getLowerX();
	reg.lower_y = r.getLowerY();
	reg.lower_z = r.getLowerZ();
	reg.upper_x = r.getUpperX();
	reg.upper_y = r.getUpperY();
	reg.upper_z = r.getUpperZ();

	cube(reg, noob::universe::SUB);
	*/
}




/*
//  ---------- management functions -----------
// Mostly useful for when you've copied a smaller universe into a larger one
void noob::universe::global_translate(int32_t x_direction, int32_t y_direction, int32_t z_direction)
{
	copy_a_to_b();

	// We use universe's region as universe_b is of the same size
	PolyVox::Region universe_region = universe->getEnclosingRegion();

	int32_t universe_width = universe_region.getWidthInVoxels();
	int32_t universe_height = universe_region.getHeightInVoxels();
	int32_t universe_depth = universe_region.getWidthInVoxels();

	int32_t lower_x = universe_region.getLowerX();
	int32_t lower_y = universe_region.getLowerY();
	int32_t lower_z = universe_region.getLowerZ();

	int32_t upper_x = universe_region.getUpperX();
	int32_t upper_y = universe_region.getUpperY();
	int32_t upper_z = universe_region.getUpperZ();

	PolyVox::Region new_region(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);

	new_region.shift(x_direction, y_direction, z_direction);

	new_region.cropTo(universe_region);

	for(int x = lower_x; x <= upper_x; x++)
	{
		for(int y = lower_y; y <= lower_y; y++)
		{
			for(int z = lower_z; z <= upper_z; z++)
			{
				if (new_region.containsPoint(x, y, z))
				{
					universe->setVoxel(x, y, z, universe_b->getVoxel(x - x_direction, y - y_direction, z - z_direction));
				}
				else
				{
					universe->setVoxel(x, y, z, 0);
				}
			}
		}
	} 
}
*/

/*
   bool noob::universe::load_universe(const std::string& name) 
   {
// std::map<std::string, std::unique_ptr<PolyVox::RawVolume<int8_t>>> volumes;
auto it = volumes->find(name);
if (it != volumes.end())
{
universe = it->second();	
return true;
}

else return false;
}

void noob::universe::save(const std::string& name) 
{

} */
/*
void noob::universe::copy_a_to_b()	
{
	// Regions are the same size
	PolyVox::Region region = universe->getEnclosingRegion();

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
				universe_b->setVoxel(x, y, z, universe->getVoxel(x, y, z));
			}
		}
	}
}
*/
