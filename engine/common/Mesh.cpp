// TODO: Remove the following comment if it works across all builds
//#define CSGJS_HEADER_ONLY
//#include <csgjs.hpp>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>

#include <OpenMesh/Tools/Decimater/CollapseInfoT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModAspectRatioT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Logger.hpp"
#include "Mesh.hpp"
#include "format.h"

#include "VHACD.h"

#include <Eigen/Geometry>

double noob::basic_mesh::get_volume() const
{
// Proudly gleaned from U of R website!
// http://mathcentral.uregina.ca/QQ/database/QQ.09.09/h/ozen1.html
// Woot alma mater! (... Soon :P)
// The volume of the tetrahedron with vertices (0 ,0 ,0), (a1 ,a2 ,a3), (b1, b2, b3) and (c1, c2, c3) is [a1b2c3 + a2b3c1 + a3b1c2 - a1b3c2 - a2b1c3 - a3b2c1] / 6.
	double accum = 0.0;
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		noob::vec3 first = vertices[i];
		noob::vec3 second = vertices[i+1];
		noob::vec3 third = vertices[i+2];
		
		accum += ((static_cast<double>(first.v[0]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[2])) + (static_cast<double>(first.v[1]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[0])) + (static_cast<double>(first.v[2]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[0]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[1]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[2])) - (static_cast<double>(first.v[2]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[0]))) / 6.0;

	}
	
	return accum;
}


void noob::basic_mesh::decimate(const std::string& filename, size_t num_verts) const
{
	// logger::log("[Mesh] decimating");
	TriMesh half_edges = to_half_edges();
	// logger::log(fmt::format("[Mesh] decimating - Half edges generated. num verts = {0}", half_edges.n_vertices()));

	OpenMesh::Decimater::DecimaterT<TriMesh> decimator(half_edges);
	OpenMesh::Decimater::ModQuadricT<TriMesh>::Handle mod;
	decimator.add(mod);
	decimator.initialize();

	auto did_init = decimator.initialize();
	if (!did_init) 
	{
		logger::log("[Mesh] decimate() - init failed!");
		return;
	}

	size_t verts_removed = decimator.decimate_to(num_verts);
	logger::log(fmt::format("[Mesh] decimate() - Vertices removed = {0}", verts_removed));
	decimator.mesh().garbage_collection();

	half_edges.garbage_collection();
	OpenMesh::IO::write_mesh(half_edges, filename);
}


noob::basic_mesh noob::basic_mesh::decimate(size_t num_verts) const
{
	decimate("./temp/temp-decimated.off", num_verts);
	noob::basic_mesh temp;
	temp.load("./temp/temp-decimated.off", "temp-decimated");
	return temp;
}


noob::basic_mesh noob::basic_mesh::normalize() const 
{
	noob::basic_mesh temp;
	temp.load(snapshot());
	return temp;
}


// TODO
noob::basic_mesh noob::basic_mesh::to_origin() const
{
	noob::basic_mesh temp;
	for (noob::vec3 v : vertices)
	{
		temp.vertices.push_back(v - bbox.center);
	}
	std::copy(normals.begin(), normals.end(), std::back_inserter(temp.normals));
	std::copy(indices.begin(), indices.end(), std::back_inserter(temp.indices));
	return temp;
}


std::tuple<size_t, const char*> noob::basic_mesh::snapshot() const
{
	fmt::MemoryWriter w;
	w << "OFF" << "\n" << vertices.size() << " " << indices.size() / 3 << " " << 0 <<  "\n";
	for (auto v : vertices)
	{
		w << v.v[0] << " " << v.v[1] << " " << v.v[2] <<  "\n";
	}
	for (size_t i = 0; i < indices.size(); i = i + 3)
	{
		w << 3 << " " << indices[i] << " " << indices[i+1] << " " << indices[i+2] << "\n";
	}

	const char* mem = w.data();
	size_t size = w.size();

	return std::make_tuple(size, mem);
}


void noob::basic_mesh::snapshot(const std::string& filename) const
{
	logger::log("[Mesh] - snapshot() - begin");
	TriMesh half_edges = to_half_edges();
	logger::log("[Mesh] - snapshot() - half-edges created");
	OpenMesh::IO::write_mesh(half_edges, filename);
}


bool noob::basic_mesh::load(std::tuple<size_t, const char*> buffer, const std::string& name)
{
	logger::log(fmt::format("[Mesh] - load({0}) - loading {1} bytes", name, std::get<0>(buffer)));
	const aiScene* scene = aiImportFileFromMemory(std::get<1>(buffer), std::get<0>(buffer), aiProcessPreset_TargetRealtime_MaxQuality, "");
	return load(scene, name);
}


bool noob::basic_mesh::load(const std::string& filename, const std::string& name)
{
	// logger::log(fmt::format("[Mesh] loading file {0}", filename ));
	const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	return load(scene, name);	
}


bool noob::basic_mesh::load(const aiScene* scene, const std::string& name)
{
	if (!scene)
	{
		logger::log(fmt::format("[Mesh] load({0}) - cannot open", name));
		return false;
	}

	vertices.clear();
	indices.clear();
	normals.clear();

	const aiMesh* mesh_data = scene->mMeshes[0];

	// logger::log(fmt::format("[Mesh] load({0}) - Attempting to obtain mesh data", name));

	size_t num_verts = mesh_data->mNumVertices;
	size_t num_faces = mesh_data->mNumFaces;
	// TODO: Fix?
	// auto num_indices = mesh_data->mNumFaces / 3;

	bool has_normals = mesh_data->HasNormals();
	//logger::log(fmt::format("[Mesh] load({0}) - Mesh has {1} verts, normals? {2}", name, num_verts, has_normals));


	// double accum_x, accum_y, accum_z = 0.0f;
	bbox.min = bbox.max = bbox.center = noob::vec3(0.0, 0.0, 0.0);	
	
	for ( size_t n = 0; n < num_verts; ++n)
	{
		aiVector3D pt = mesh_data->mVertices[n];
		noob::vec3 v;
		v.v[0] = pt[0];
		v.v[1] = pt[1];
		v.v[2] = pt[2];
		vertices.push_back(v);

		// accum_x += v[0];
		// accum_y += v[1];
		// accum_z += v[2];

		bbox.min[0] = std::min(bbox.min[0], v[0]);
		bbox.min[1] = std::min(bbox.min[1], v[1]);
		bbox.min[2] = std::min(bbox.min[2], v[2]);

		bbox.max[0] = std::max(bbox.max[0], v[0]);
		bbox.max[1] = std::max(bbox.max[1], v[1]);
		bbox.max[2] = std::max(bbox.max[2], v[2]);


		aiVector3D normal = mesh_data->mNormals[n];
		noob::vec3 norm;
		norm.v[0] = normal[0];
		norm.v[1] = normal[1];
		norm.v[2] = normal[2];
		normals.push_back(norm);
	}
	
	if (num_verts == 0)
	{
		num_verts = 1;
	}

	// double centroid_x = accum_x / static_cast<double>(num_verts);
	// double centroid_y = accum_y / static_cast<double>(num_verts);
	// double centroid_z = accum_z / static_cast<double>(num_verts);

	// bbox.centroid = noob::vec3(static_cast<float>(centroid_x)/2, static_cast<float>(centroid_y)/2, static_cast<float>(centroid_z)/2);
	
	bbox.center = noob::vec3((bbox.max[0] - bbox.min[0])/2, (bbox.max[1] - bbox.min[1])/2, (bbox.max[2] - bbox.min[2])/2);

	// logger::log(fmt::format("[Mesh] load({0}) - Mesh has {1} faces", name, num_faces));
	auto degenerates = 0;
	auto tris = 0;
	auto non_tri_polys = 0;
	
	for (size_t n = 0; n < num_faces; ++n)
	{
		// Allows for degenerate triangles. Worth keeping?
		const struct aiFace* face = &mesh_data->mFaces[n];
		if (face->mNumIndices == 2)
		{
			degenerates++;
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
		}

		else if (face->mNumIndices == 3)
		{
			tris++;
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);
		}

		else
		{
			non_tri_polys++;
		}
	}
	aiReleaseImport(scene);
	// logger::log(fmt::format("[Mesh] load({3}) loaded - tris = {0}, non-tri polys = {1}, degenerates = {2}", tris, non_tri_polys, degenerates, name));
	return true;
}

noob::basic_mesh noob::basic_mesh::transform(const noob::mat4& transform) const
{
	noob::basic_mesh temp;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		noob::vec3 v = vertices[i];
		noob::vec4 temp_vert(v, 1.0);
		noob::vec4 temp_transform = transform * temp_vert;
		noob::vec3 transformed_vert;
		
		transformed_vert[0] = temp_transform[0];
		transformed_vert[1] = temp_transform[1];
		transformed_vert[2] = temp_transform[2];

		temp.vertices.push_back(transformed_vert);
	}

	std::copy(normals.begin(), normals.end(), std::back_inserter(temp.normals));
	
	std::copy(indices.begin(), indices.end(), std::back_inserter(temp.indices));


	noob::vec4 temp_max(bbox.max, 1.0);
	noob::vec4 temp_min(bbox.min, 1.0);
	noob::vec4 temp_bbox_center(bbox.center, 1.0);
	// noob::vec4 temp_centroid(bbox.centroid, 1.0);

	temp_max = transform * temp_max;
	temp_min = transform * temp_min;
	temp_bbox_center = transform * temp_bbox_center;
	// temp_centroid = transform * temp_centroid;

	temp.bbox.max = noob::vec3(temp_max);
	temp.bbox.min = noob::vec3(temp_min);
	temp.bbox.center = noob::vec3(temp_bbox_center);
	// temp.bbox.centroid = noob::vec3(temp_centroid);

	//temp.bbox = bbox;
	
	return temp;
}


TriMesh noob::basic_mesh::to_half_edges() const
{
	TriMesh half_edges;
	// half_edges.request_vertex_normals();
	std::vector<TriMesh::VertexHandle> vert_handles;

	// logger::log(fmt::format("[Mesh] to_half_edges() - setting {0} verts", vertices.size())); 

	for (auto v : vertices)
	{
		vert_handles.push_back(half_edges.add_vertex(TriMesh::Point(v.v[0], v.v[1], v.v[2])));
	}
	// logger::log(fmt::format("[Mesh] to_half_edges() - setting {0} indices", indices.size()));

	// NOTE: The following is mostly for debug bbox
	/* size_t max_index_size = 0;

	for (size_t i = 0; i < indices.size(); i++)
	{
		if (indices[i] > max_index_size)
		{
			max_index_size = static_cast<size_t>(indices[i]);
		}
	} */

	// logger::log("[Mesh] to_half_edges() - finished adding indices");

	// TODO: Find out why this breaks;
	// logger::log(fmt::format("{Mesh] to_half_edges() - Max index size = {0}, num vertices = {1}", max_index_size, vertices.size()));

	for (size_t i = 0; i < indices.size(); i = i + 3)
	{
		std::vector<TriMesh::VertexHandle> face_verts;
		face_verts.push_back(vert_handles[indices[i]]);
		face_verts.push_back(vert_handles[indices[i+1]]);
		face_verts.push_back(vert_handles[indices[i+2]]);
		half_edges.add_face(face_verts);
	}

	// logger::log(fmt::format("[Mesh] to_half_edges - finished processing. Num vertices {0}, num indices {1}", half_edges.n_vertices(), half_edges.n_halfedges()));

	return half_edges;
}

std::vector<noob::basic_mesh> noob::basic_mesh::convex_decomposition() const
{
	logger::log("[Mesh] convex_decomposition()");
	VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
	VHACD::IVHACD::Parameters params;
	params.m_oclAcceleration = false;
	
	std::vector<noob::basic_mesh> meshes;
	std::vector<float> points;
	std::vector<int> triangles;

	for (noob::vec3 v : vertices)
	{
		points.push_back(v[0]);
		points.push_back(v[1]);
		points.push_back(v[2]);
	}

	for (uint16_t i : indices)
	{
		triangles.push_back(static_cast<int>(i));
	}

	bool success = interfaceVHACD->Compute(&points[0], 3, (unsigned int)points.size() / 3, &triangles[0], 3, (unsigned int)triangles.size() / 3, params);
	
	if (success)
	{
		size_t num_hulls = interfaceVHACD->GetNConvexHulls();
		logger::log(fmt::format("[Mesh] convex_decomposition() - success! {0} hulls generated", num_hulls));
		for (size_t i = 0; i < num_hulls; i++)
		{
			VHACD::IVHACD::ConvexHull hull;
			interfaceVHACD->GetConvexHull(i, hull);
			noob::basic_mesh m;

			size_t num_points = hull.m_nPoints;
			size_t num_tris = hull.m_nTriangles;
			size_t num_indices = num_tris * 3;

			logger::log(fmt::format("[Mesh] convex_decomposition() - Mesh # {0} - num verts = {1}, num triangles = {2}, num indices = {3}", i, num_points, num_tris, num_indices));

			// TODO: Find out why the following commented-out code is broken.
			/*
			for (size_t j = 0; j < num_points; j++)
			{
				noob::vec3 v;
				size_t index = j*3;
				v.v[0] = hull.m_points[index];
				v.v[1] = hull.m_points[index+1];
				v.v[2] = hull.m_points[index+2];
				m.vertices.push_back(v);
			}

			for (size_t j = 0; j < num_indices; j++)
			{
				m.indices.push_back(static_cast<uint16_t>(hull.m_triangles[j]));
			}

			logger::log(fmt::format("[Mesh] convex_decomposition() - Mesh # {0} - Data copied: num verts = {1}, num indices = {2}", i, m.vertices.size(), m.indices.size()));
			noob::basic_mesh final_mesh;
			
			auto snap = m.snapshot();
			final_mesh.load(snap, "temp-hacd");

			logger::log(fmt::format("[Mesh] convex_decomposition() - Final, cleaned mesh # {0} - Stats: num verts = {1}, num indices = {2}", i, final_mesh.vertices.size(), final_mesh.indices.size()));
			*/
			
			fmt::MemoryWriter w;
			size_t nV = num_points * 3;;
			w << "OFF" << "\n" << num_points << " " << num_tris << " 0" << "\n";
			for (size_t v = 0; v < nV; v += 3)
			{
				w << hull.m_points[v+0] << " " << hull.m_points[v+1] << " " << hull.m_points[v+2] << "\n";
			}
			for (size_t f = 0; f < num_indices; f += 3)
			{
				w << "3 " << hull.m_triangles[f+0] << " " << hull.m_triangles[f+1] << " " << hull.m_triangles[f+2] << "\n";
			}
			
			const char* mem = w.data();
			size_t size = w.size();

			noob::basic_mesh final_mesh;
			final_mesh.load(std::make_tuple(size, mem), "");

			meshes.push_back(final_mesh);

	}
	}
	else 
	{
		logger::log("[Mesh] convex_decomposition() - failure. :(");
	}

	interfaceVHACD->Clean();
	interfaceVHACD->Release();
	
	return meshes;

}


// TODO: Use the same struct and benefit from zero-copy awesomeness
/*
noob::basic_mesh noob::basic_mesh::csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op)
{
	std::vector<csgjs_model> csg_models;
	std::vector<noob::basic_mesh> meshes;

	meshes.push_back(a);
	meshes.push_back(b);

	// meshes[0].vertices.reserve(a.vertices.size());
	// meshes[1].indices.reserve(a.indices.size());

	for (size_t i = 0; i > meshes.size(); i++)
	{
		csgjs_model model;
		for (size_t j = 0; j > meshes[i].vertices.size(); j++)
		{
			model.vertices[j].pos.x = meshes[i].vertices[j].v[0];
			model.vertices[j].pos.y = meshes[i].vertices[j].v[2];
			model.vertices[j].pos.z = meshes[i].vertices[j].v[3];

			model.vertices[j].normal.x = meshes[i].vertices[j].v[0];
			model.vertices[j].normal.y = meshes[i].vertices[j].v[1];
			model.vertices[j].normal.z = meshes[i].vertices[j].v[2];

			model.vertices[j].uv.x = meshes[i].vertices[j].v[0];
			model.vertices[j].uv.y = meshes[i].vertices[j].v[1];
		}
		for (size_t j = 0; j > meshes[i].vertices.size(); j++)
		{
			model.indices[j] = (int)meshes[i].indices[j];
		}

		csg_models.push_back(model);
	}

	csgjs_model resulting_csg_model;
	switch (op)
	{
		case(UNION):
			resulting_csg_model = csgjs_union(csg_models[0], csg_models[1]);
			break;
		case(DIFFERENCE):
			resulting_csg_model = csgjs_difference(csg_models[0], csg_models[1]);
			break;
		case(INTERSECTION):
			resulting_csg_model = csgjs_intersection(csg_models[0], csg_models[1]);
			break;
	}

	noob::basic_mesh results;

	for (size_t i = 0; i < resulting_csg_model.vertices.size(); i++)
	{
		results.vertices[i].v[0] =  resulting_csg_model.vertices[i].pos.x;
		results.vertices[i].v[1] =  resulting_csg_model.vertices[i].pos.y;
		results.vertices[i].v[2] =  resulting_csg_model.vertices[i].pos.z;

		results.normals[i].v[0] =  resulting_csg_model.vertices[i].normal.x;
		results.normals[i].v[1] =  resulting_csg_model.vertices[i].normal.y;
		results.normals[i].v[2] =  resulting_csg_model.vertices[i].normal.z;

		// results.vertices[i].v[0] =  resulting_csg_model.vertices[i].uv.x;
		// results.vertices[i].v[1] =  resulting_csg_model.vertices[i].uv.y;
	}

	return results;
}
*/

noob::basic_mesh noob::basic_mesh::cone(float radius, float height, size_t segments)
{
	TriMesh half_edges;
	size_t _segments;
	if (segments == 0)
	{
		_segments = 12;
	}
	else 
	{
		_segments = segments;
	}

	TriMesh::VertexHandle top = half_edges.add_vertex(TriMesh::Point(0.0f, height, 0.0f));
	TriMesh::VertexHandle origin = half_edges.add_vertex(TriMesh::Point(0.0f, 0.0f, 0.0f));

	double increment_amount = TWO_PI / _segments;

	std::vector<TriMesh::VertexHandle> verts;

	Eigen::Vector3f p(0.0f, 0.0f, radius);

	for (size_t seg = 0; seg < _segments; seg++)
	{
		double diff = increment_amount * seg;
		Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
		Eigen::Vector3f rotated_point = angle_axis * p;
		verts.push_back(half_edges.add_vertex(TriMesh::Point(rotated_point[0], rotated_point[1], rotated_point[2])));
	}

	std::vector<TriMesh::VertexHandle> face_verts;	
	for(size_t i = 1; i < verts.size(); i++)
	{
		face_verts.clear();
		face_verts.push_back(top);
		face_verts.push_back(verts[i]);
		face_verts.push_back(verts[i-1]);
		half_edges.add_face(face_verts);

		face_verts.clear();
		face_verts.push_back(origin);
		face_verts.push_back(verts[i-1]);
		face_verts.push_back(verts[i]);
		half_edges.add_face(face_verts);
	}

	face_verts.clear();
	face_verts.push_back(top);
	face_verts.push_back(verts[0]);
	face_verts.push_back(verts[verts.size()-1]);
	half_edges.add_face(face_verts);

	face_verts.clear();
	face_verts.push_back(origin);
	face_verts.push_back(verts[verts.size()-1]);
	face_verts.push_back(verts[0]);
	half_edges.add_face(face_verts);

	// half_edges.garbage_collection();
	OpenMesh::IO::write_mesh(half_edges, "temp/cone.off");
	noob::basic_mesh mesh;
	mesh.load("temp/cone.off","cone-temp");
	logger::log(fmt::format("Created cone with height = {0}, radius = {1}, and {2} segments.", height, radius, _segments));
	return mesh;
}

noob::basic_mesh noob::basic_mesh::cylinder(float radius, float height, size_t segments)
{
	PolyMesh half_edges;
	size_t _segments;
	if (segments == 0) 
	{
		_segments = 12;
	}
	else
	{
		_segments = segments;
	}
	double increment_amount = TWO_PI / static_cast<double>(_segments);

	std::vector<std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle>> verts;

	PolyMesh::VertexHandle top = half_edges.add_vertex(PolyMesh::Point(0.0f, height, 0.0f));
	PolyMesh::VertexHandle origin = half_edges.add_vertex(PolyMesh::Point(0.0f, 0.0f, 0.0f));


	Eigen::Vector3f p_upper(0.0f, height, radius);
	Eigen::Vector3f p_lower(0.0f, 0.0f, radius);

	for (size_t seg = 0; seg < _segments; seg++)
	{
		double diff = increment_amount * seg;
		Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());

		Eigen::Vector3f rotated_point_lower = angle_axis * p_lower;
		Eigen::Vector3f rotated_point_upper = angle_axis * p_upper;

		PolyMesh::VertexHandle v1 = half_edges.add_vertex(PolyMesh::Point(rotated_point_lower[0], rotated_point_lower[1], rotated_point_lower[2]));
		PolyMesh::VertexHandle v2 = half_edges.add_vertex(PolyMesh::Point(rotated_point_upper[0], rotated_point_upper[1], rotated_point_upper[2]));
		verts.push_back(std::make_tuple(v1, v2));
	}

	std::vector<PolyMesh::VertexHandle> face_verts;	
	for(size_t i = 1; i < verts.size(); i++)
	{
		std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle> previous_verts = verts[i-1];
		std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle> current_verts = verts[i];

		face_verts.clear();
		face_verts.push_back(top);
		face_verts.push_back(std::get<1>(current_verts));
		face_verts.push_back(std::get<1>(previous_verts));
		half_edges.add_face(face_verts);

		face_verts.clear();	
		face_verts.push_back(std::get<0>(previous_verts));
		face_verts.push_back(std::get<1>(previous_verts));
		face_verts.push_back(std::get<1>(current_verts));
		face_verts.push_back(std::get<0>(current_verts));
		half_edges.add_face(face_verts);

		face_verts.clear();
		face_verts.push_back(origin);
		face_verts.push_back(std::get<0>(previous_verts));
		face_verts.push_back(std::get<0>(current_verts));
		half_edges.add_face(face_verts);
	}

	face_verts.clear();
	face_verts.push_back(top);
	face_verts.push_back(std::get<1>(verts[0]));
	face_verts.push_back(std::get<1>(verts[verts.size()-1]));
	half_edges.add_face(face_verts);

	face_verts.clear();	
	face_verts.push_back(std::get<0>(verts[verts.size()-1]));
	face_verts.push_back(std::get<1>(verts[verts.size()-1]));
	face_verts.push_back(std::get<1>(verts[0]));
	face_verts.push_back(std::get<0>(verts[0]));
	half_edges.add_face(face_verts);

	face_verts.clear();
	face_verts.push_back(origin);
	face_verts.push_back(std::get<0>(verts[verts.size()-1]));
	face_verts.push_back(std::get<0>(verts[0]));
	half_edges.add_face(face_verts);


	half_edges.triangulate();
	half_edges.garbage_collection();
	OpenMesh::IO::write_mesh(half_edges, "temp/cylinder.off");
	noob::basic_mesh mesh;
	mesh.load("temp/cylinder.off","cylinder-temp");
	logger::log(fmt::format("Created cylinder with height = {0}, radius = {1} with {2} segments.", height, radius, _segments));
	return mesh;
}


noob::basic_mesh noob::basic_mesh::cube(float width, float height, float depth, size_t subdivides)
{
	PolyMesh half_edges;
	PolyMesh::VertexHandle vhandle[8];

	vhandle[0] = half_edges.add_vertex(PolyMesh::Point(-width, -height, depth));
	vhandle[1] = half_edges.add_vertex(PolyMesh::Point( width, -height, depth));
	vhandle[2] = half_edges.add_vertex(PolyMesh::Point( width,  height, depth));
	vhandle[3] = half_edges.add_vertex(PolyMesh::Point(-width,  height, depth));
	vhandle[4] = half_edges.add_vertex(PolyMesh::Point(-width, -height, -depth));
	vhandle[5] = half_edges.add_vertex(PolyMesh::Point( width, -height, -depth));
	vhandle[6] = half_edges.add_vertex(PolyMesh::Point( width,  height, -depth));
	vhandle[7] = half_edges.add_vertex(PolyMesh::Point(-width,  height, -depth));


	// generate (quadrilateral) faces

	std::vector<PolyMesh::VertexHandle> face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	half_edges.add_face(face_vhandles);

	if (subdivides != 0)
	{
		OpenMesh::Subdivider::Uniform::CatmullClarkT<PolyMesh> catmull;
		catmull.attach(half_edges);
		catmull(subdivides);
		catmull.detach();
	}

//, height, radius, _subdivides
	half_edges.triangulate();
	half_edges.garbage_collection();
	OpenMesh::IO::write_mesh(half_edges, "temp/cube.off");

	noob::basic_mesh mesh;
	mesh.load("temp/cube.off", "cube-temp");
	logger::log(fmt::format("Created cube with width = {0}, height = {1}, depth = {2} with {3} subdivides.", width, height, depth, subdivides));
	return mesh;
}


noob::basic_mesh noob::basic_mesh::sphere(float radius)
{
	float diameter = radius * 2;
	noob::basic_mesh mesh = noob::basic_mesh::cube(diameter, diameter, diameter, 3);
	logger::log(fmt::format("Created sphere of radius {0}.", radius));
	return mesh;
}

/*
noob::basic_mesh noob::basic_mesh::bone()
{
	TriMesh half_edges;
	TriMesh::VertexHandle vhandles[6];

	vhandles[0] = half_edges.add_vertex(TriMesh::Point(1.0, 0.0, 0.0));
	vhandles[1] = half_edges.add_vertex(TriMesh::Point(0.8, 0.2, 0.2));
	vhandles[2] = half_edges.add_vertex(TriMesh::Point(0.8, 0.2, -0.2));
	vhandles[3] = half_edges.add_vertex(TriMesh::Point(0.8, -0.2, -0.2));
	vhandles[4] = half_edges.add_vertex(TriMesh::Point(0.8, -0.2, 0.2));
	vhandles[5] = half_edges.add_vertex(TriMesh::Point(-1.0, 0.0, 0.0));

	std::vector<TriMesh::VertexHandle> face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[0]);
	face_vhandles.push_back(vhandles[2]);
	face_vhandles.push_back(vhandles[1]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[5]);
	face_vhandles.push_back(vhandles[1]);
	face_vhandles.push_back(vhandles[2]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[0]);
	face_vhandles.push_back(vhandles[3]);
	face_vhandles.push_back(vhandles[2]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[5]);
	face_vhandles.push_back(vhandles[2]);
	face_vhandles.push_back(vhandles[3]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[0]);
	face_vhandles.push_back(vhandles[4]);
	face_vhandles.push_back(vhandles[3]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[5]);
	face_vhandles.push_back(vhandles[3]);
	face_vhandles.push_back(vhandles[4]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[0]);
	face_vhandles.push_back(vhandles[1]);
	face_vhandles.push_back(vhandles[4]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[5]);
	face_vhandles.push_back(vhandles[4]);
	face_vhandles.push_back(vhandles[1]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandles[0]);
	face_vhandles.push_back(vhandles[1]);
	face_vhandles.push_back(vhandles[2]);
	half_edges.add_face(face_vhandles);

	OpenMesh::IO::write_mesh(half_edges, "temp/bone.off");
	noob::basic_mesh mesh;
	mesh.load("temp/bone.off", "bone-temp");

	return mesh;
}
*/
