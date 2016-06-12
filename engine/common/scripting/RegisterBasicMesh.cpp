#include "RegisterBasicMesh.hpp"

#include "MeshUtils.hpp"

void noob::register_basic_mesh(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("basic_mesh", sizeof(basic_mesh), asOBJ_VALUE | asGetTypeTraits<noob::basic_mesh>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::basic_mesh>("basic_mesh", script_engine);
	r = script_engine->RegisterObjectBehaviour("basic_mesh", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_basic_mesh_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("basic_mesh", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_basic_mesh_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "basic_mesh& opAssign(const basic_mesh& in)", asMETHODPR(noob::basic_mesh, operator=, (const noob::basic_mesh&),noob::basic_mesh&), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_vertex(uint)", asMETHOD(noob::basic_mesh, get_vertex), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_normal(uint)", asMETHOD(noob::basic_mesh, get_normal), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_texcoord(uint)", asMETHOD(noob::basic_mesh, get_texcoord), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "uint get_index(uint)", asMETHOD(noob::basic_mesh, get_index), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_vertex(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_vertex), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void set_normal(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_normal), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void set_texcoord(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_texcoord), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_index(uint, uint)", asMETHOD(noob::basic_mesh, set_index), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "double get_volume()", asMETHOD(noob::basic_mesh, get_volume), asCALL_THISCALL); assert( r >= 0 );
	// void decimate(const std::string& filename, size_t num_verts) const;
	// noob::basic_mesh decimate(size_t num_verts) const;
	r = script_engine->RegisterObjectMethod("basic_mesh", "string save()", asMETHODPR(noob::basic_mesh, save, (void) const, std::string), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void save(const string& in)", asMETHODPR(noob::basic_mesh, save, (const std::string&) const, void), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "bool load_mem(const string& in, const string& in)", asMETHODPR(noob::basic_mesh, load_mem, (const std::string&, const std::string&), bool), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void transform(const mat4& in)", asMETHOD(noob::basic_mesh, transform), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void normalize()", asMETHOD(noob::basic_mesh, normalize), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void to_origin()", asMETHOD(noob::basic_mesh, to_origin), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void translate(const vec3& in)", asMETHOD(noob::basic_mesh, translate), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void rotate(const versor& in)", asMETHOD(noob::basic_mesh, rotate), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("basic_mesh", "void scale(const vec3& in)", asMETHOD(noob::basic_mesh, scale), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "bbox get_bbox() const", asMETHOD(noob::basic_mesh, get_bbox), asCALL_THISCALL); assert( r >= 0 );

	// Mesh-related globals
	r = script_engine->RegisterGlobalFunction("basic_mesh sphere_mesh(float)", asFUNCTION(mesh_utils::sphere), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh box_mesh(float, float, float)", asFUNCTION(mesh_utils::box), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh cone_mesh(float, float)", asFUNCTION(mesh_utils::cone), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh cylinder_mesh(float, float)", asFUNCTION(mesh_utils::cylinder), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh hull_mesh(const vector_vec3& in)", asFUNCTION(mesh_utils::hull), asCALL_CDECL); assert(r >= 0);
}
