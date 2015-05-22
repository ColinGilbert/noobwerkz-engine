#include "TerrainStreamer.hpp"

#include <csgjs.hpp>
#include <OpenMesh/Core/IO/MeshIO.hh>

void noob::terrain::load_terrain_mesh(const std::string& filename, size_t x_dim, size_t y_dim)
{
	world_mesh.request_vertex_normals();

	// Assure we have vertex normals
	if (!world_mesh.has_vertex_normals())
	{
		logger::log("ERROR: Standard vertex property 'Normals' not available!");
	}

	OpenMesh::IO::Options opt;
	if (!OpenMesh::IO::read_mesh(filename, opt))
	{
		std::stringstream ss;
		ss << "Error loading mesh from file: " << filename;
		logger::log(ss.str());
		return;
	}

	// If the file did not provide vertex normals, then calculate them
	if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	{
		// We need face normals to update the vertex normals
		world_mesh.request_face_normals();

		// Let the mesh update the normals
		world_mesh.update_normals();

		// Dispose the face normals, as we don't need them anymore
		world_mesh.release_face_normals();
	}
}


noob::drawable* noob::terrain::get_terrain(const terrain::area& screen_dims)
{

}
