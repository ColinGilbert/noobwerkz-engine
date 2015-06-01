#include "World.hpp"

#include "Logger.hpp"

#include <map>
#include <memory>
#include <array>
#include <cmath>
#include <fstream>

#include <OpenMesh/Core/IO/MeshIO.hh> // Must be included prior to an OpenMesh class header
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

void noob::world::init()
{
}


noob::world::cubic_region::cubic_region(int32_t _lower_x, int32_t _lower_y, int32_t _lower_z, int32_t _upper_x, int32_t _upper_y, int32_t _upper_z)
{
	lower_x = _lower_x;
	lower_y = _lower_y;
	lower_z = _lower_z;
	upper_x = _upper_x;
	upper_y = _upper_y;
	upper_z = _upper_z;
}


// ---------------- basic shapes --------------

void noob::world::sphere(uint32_t radius, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::world::op_type op) 
{

}


void noob::world::cube(const noob::world::cubic_region& reg, noob::world::op_type op)
{

}


// Cylinder is aligned up/down the y-axis (standing up)
void noob::world::cylinder(uint32_t radius, uint32_t height, int32_t origin_x, int32_t origin_y, int32_t origin_z, noob::world::op_type op)
{

}


void noob::world::clear_world()
{

}
