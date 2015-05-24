// TODO: Reorganize to as to remove all references to bgfx from this class (split model from view.)
#pragma once

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

	static noob::mesh csg(const noob::mesh& a, const noob::mesh& b, const noob::csg_op op);
}
