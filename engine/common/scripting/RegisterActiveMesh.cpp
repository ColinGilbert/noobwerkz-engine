#include "RegisterActiveMesh.hpp"

void noob::register_active_mesh(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("face_h", sizeof(noob::active_mesh::face_h), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::face_h>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::face_h>("face_h", script_engine);

	r = script_engine->RegisterObjectType("vertex_h", sizeof(noob::active_mesh::vertex_h), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::vertex_h>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::vertex_h>("vertex_h", script_engine);

	r = script_engine->RegisterObjectType("edge_h", sizeof(noob::active_mesh::edge_h), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::edge_h>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::edge_h>("edge_h", script_engine);

	r = script_engine->RegisterObjectType("halfedge_h", sizeof(noob::active_mesh::halfedge_h), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::halfedge_h>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::halfedge_h>("halfedge_h", script_engine);

	r = script_engine->RegisterObjectType("active_mesh", sizeof(active_mesh), asOBJ_VALUE | asGetTypeTraits<noob::active_mesh>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh>("active_mesh", script_engine);
	r = script_engine->RegisterObjectBehaviour("active_mesh", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_active_mesh_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("active_mesh", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_active_mesh_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "active_mesh& opAssign(const active_mesh& in)", asMETHODPR(noob::active_mesh, operator=, (const noob::active_mesh&),noob::active_mesh&), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void reset()", asMETHOD(noob::active_mesh, reset), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void from_basic_mesh(const basic_mesh& in)", asMETHOD(noob::active_mesh, from_basic_mesh), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vertex_h add_vertex(const vec3& in)", asMETHOD(noob::active_mesh, add_vertex), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void add_face(const vector_vertex_h& in)", asMETHODPR(noob::active_mesh, add_face, (const std::vector<noob::active_mesh::vertex_h>&), noob::active_mesh::face_h), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool vertex_exists(const vec3& in) const", asMETHODPR(noob::active_mesh, vertex_exists, (const noob::vec3&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool vertex_exists(vertex_h) const", asMETHODPR(noob::active_mesh, vertex_exists, (const noob::active_mesh::vertex_h) const, bool), asCALL_THISCALL); assert( r >= 0 );
	// std::tuple<bool, noob::vec3> get_vertex(PolyMesh::VertexHandle) const;
	r = script_engine->RegisterObjectMethod("active_mesh", "vec3 get_face_normal(face_h) const", asMETHOD(noob::active_mesh, get_face_normal), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vertex_h get_vertex_handle(const vec3& in) const", asMETHOD(noob::active_mesh, get_vertex_handle), asCALL_THISCALL); assert(r >= 0);
	// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex/
	r = script_engine->RegisterObjectMethod("active_mesh", "bool face_exists(const vector_vertex_h& in) const", asMETHODPR(noob::active_mesh, face_exists, (const std::vector<noob::active_mesh::vertex_h>&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool face_exists(face_h) const", asMETHODPR(noob::active_mesh, face_exists, (const noob::active_mesh::face_h) const, bool), asCALL_THISCALL); assert( r >= 0 );
	// const std::tuple<bool, std::vector<noob::vec3>> get_face(PolyMesh::VertexHandle) const; 
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_vertices() const", asMETHOD(noob::active_mesh, num_vertices), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_half_edges() const", asMETHOD(noob::active_mesh, num_half_edges), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_edges() const", asMETHOD(noob::active_mesh, num_edges), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_faces() const", asMETHOD(noob::active_mesh, num_faces), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_vertex_h get_verts_in_face(face_h) const", asMETHOD(noob::active_mesh, get_verts_in_face), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_edge_h get_adjacent_edges(face_h, face_h) const", asMETHOD(noob::active_mesh, get_adjacent_edges), asCALL_THISCALL); assert(r >= 0);	
	r = script_engine->RegisterObjectMethod("active_mesh", "basic_mesh to_basic_mesh() const", asMETHOD(noob::active_mesh, to_basic_mesh), asCALL_THISCALL); assert(r >= 0);
	// std::vector<PolyMesh::FaceHandle> get_adjacent_faces(PolyMesh::FaceHandle) const;
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_face_h get_adjacent_faces(face_h) const", asMETHOD(noob::active_mesh, get_adjacent_faces), asCALL_THISCALL); assert(r >= 0);
	// Destructive utiiities.
	r = script_engine->RegisterObjectMethod("active_mesh", "void make_hole(face_h)", asMETHOD(noob::active_mesh, make_hole), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void fill_holes()", asMETHOD(noob::active_mesh, fill_holes), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void extrude_face(face_h, float, const vec3& in)", asMETHOD(noob::active_mesh, extrude_face), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void split_face(face_h, const plane& in)", asMETHOD(noob::active_mesh, split_face), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void move_vertex(vertex_h, const vec3& in)", asMETHOD(noob::active_mesh, move_vertex), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void move_vertices(const vector_vertex_h& in, const vec3& in)", asMETHOD(noob::active_mesh, move_vertices), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void merge_adjacent_coplanars()", asMETHOD(noob::active_mesh, merge_adjacent_coplanars), asCALL_THISCALL); assert(r >= 0);
}
