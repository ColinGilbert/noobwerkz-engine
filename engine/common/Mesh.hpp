// TODO: Reorganize to as to remove all references to bgfx from this class (split model from view.)
#pragma once
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <bgfx.h>

namespace noob
{
	enum csg_op
	{
		UNION, DIFFERENCE, INTERSECTION
	};

	struct mesh 
	{
		std::vector<noob::graphics::mesh_vertex> vertices;
		std::vector<uint16_t> indices;
		bgfx::VertexBufferHandle vertex_buffer;
		bgfx::IndexBufferHandle index_buffer;
		int32_t material_index;
		OpenMesh::TriMesh_ArrayKernelT<> half_edge_mesh;
	};
	noob::mesh simplify();
	noob::mesh subdivide();



	// TODO: Take those away from the mesh class itself
	static void init_csg_meshes();
	static noob::mesh get_plane(const noob::vec2& lower_left, const noob::vec3& upper_right);
	static noob::mesh get_cube(const noob::vec3& lower_left, const noob::vec3& upper_right);
	
	// Considered a "spheroid", as it is not geometrically correct - it is currently a subdivided cube.
	static noob::mesh get_spheroid(const float radius);
	static noob::mesh csg(const noob::mesh& a, const noob::mesh& b, const noob::csg_op op);
}
