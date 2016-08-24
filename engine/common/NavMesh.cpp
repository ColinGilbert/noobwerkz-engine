#include "NavMesh.hpp"

noob::navigation::~navigation() noexcept(true)
{
	cleanup_temporaries();

	dtFreeNavMesh(navmesh);
	navmesh = nullptr;
}

void noob::navigation::add_geom(const noob::basic_mesh&) noexcept(true)
{

}

void noob::navigation::cleanup_temporaries() noexcept(true)
{
	rcFreeHeightField(heightfield);
	heightfield = nullptr;
	rcFreeCompactHeightfield(compact_heightfield);
	compact_heightfield = nullptr;
	rcFreeContourSet(contours);
	contours = nullptr;
	rcFreePolyMesh(polymesh);
	polymesh = nullptr;
	rcFreePolyMeshDetail(polymesh_detail);
	polymesh_detail = nullptr;
}


void noob::navigation::clear_geom() noexcept(true)
{

}



bool noob::navigation::build() noexcept(true)
{
	return false;
}

