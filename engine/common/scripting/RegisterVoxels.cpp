#include "RegisterVoxels.hpp"

void noob::register_voxels(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("voxel_world", sizeof(noob::voxel_world), asOBJ_VALUE); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_voxel_world_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	// r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_CONSTRUCT,  "void f(uint, uint, uint)", asFUNCTION(as_voxel_world_constructor_wrapper_uint_3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_voxel_world_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void init(uint, uint, uint)", asMETHOD(noob::voxel_world, init), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void set(uint, uint, uint, uint8)", asMETHOD(noob::voxel_world, set), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("voxel_world", "apply_to_region(uint, uint, uint, uint, uint, uint, std::function<uint8(uint, uint, uint)>)", asMETHOD(noob::voxel_world, apply_to_region), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void sphere(uint, uint, uint, uint, bool)", asMETHOD(noob::voxel_world, sphere), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void box(uint, uint, uint, uint, uint, uint, uint8)", asMETHOD(noob::voxel_world, box), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void get(uint, uint, uint)", asMETHOD(noob::voxel_world, safe_get), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void get_unsafe(uint, uint, uint)", asMETHOD(noob::voxel_world, get), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_smooth()", asMETHOD(noob::voxel_world, extract_smooth), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_cubic()", asMETHODPR(noob::voxel_world, extract_cubic, (void) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_cubic(uint8)", asMETHODPR(noob::voxel_world, extract_cubic, (uint8_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_smooth(uint, uint, uint, uint, uint, uint)", asMETHOD(noob::voxel_world, extract_region_smooth), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_cubic(uint, uint, uint, uint, uint, uint)", asMETHODPR(noob::voxel_world, extract_region_cubic, (size_t, size_t, size_t, size_t, size_t, size_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_cubic(uint, uint, uint, uint, uint, uint, uint8)", asMETHODPR(noob::voxel_world, extract_region_cubic, (size_t, size_t, size_t, size_t, size_t, size_t, uint8_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );

}
